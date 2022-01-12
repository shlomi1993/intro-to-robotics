#include "footbot_foraging.h"
#include <argos3/core/utility/configuration/argos_configuration.h>
#include <argos3/core/utility/math/vector2.h>
#include <argos3/core/utility/logging/argos_log.h>

CFootBotForaging::SFoodData::SFoodData() :
    HasFoodItem(false),
    FoodItemIdx(0),
    TotalFoodItems(0) {}

void CFootBotForaging::SFoodData::Reset() {
    HasFoodItem = false;
    FoodItemIdx = 0;
    TotalFoodItems = 0;
}

CFootBotForaging::SDiffusionParams::SDiffusionParams() :
    GoStraightAngleRange(CRadians(-1.0f), CRadians(1.0f)) {}

void CFootBotForaging::SDiffusionParams::Init(TConfigurationNode& t_node) {
    try {
        CRange<CDegrees> cGoStraightAngleRangeDegrees(CDegrees(-10.0f), CDegrees(10.0f));
        GetNodeAttribute(t_node, "go_straight_angle_range", cGoStraightAngleRangeDegrees);
        GoStraightAngleRange.Set(ToRadians(cGoStraightAngleRangeDegrees.GetMin()),
                               ToRadians(cGoStraightAngleRangeDegrees.GetMax()));
        GetNodeAttribute(t_node, "delta", Delta);
    } catch(CARGoSException& ex) {
        THROW_ARGOSEXCEPTION_NESTED("Error initializing controller diffusion parameters.", ex);
    }
}

void CFootBotForaging::SWheelTurningParams::Init(TConfigurationNode& t_node) {
    try {
        TurningMechanism = NO_TURN;
        CDegrees cAngle;
        GetNodeAttribute(t_node, "hard_turn_angle_threshold", cAngle);
        HardTurnOnAngleThreshold = ToRadians(cAngle);
        GetNodeAttribute(t_node, "soft_turn_angle_threshold", cAngle);
        SoftTurnOnAngleThreshold = ToRadians(cAngle);
        GetNodeAttribute(t_node, "no_turn_angle_threshold", cAngle);
        NoTurnAngleThreshold = ToRadians(cAngle);
        GetNodeAttribute(t_node, "max_speed", MaxSpeed);
    } catch(CARGoSException& ex) {
        THROW_ARGOSEXCEPTION_NESTED("Error initializing controller wheel turning parameters.", ex);
    }
}

CFootBotForaging::SStateData::SStateData() : ProbRange(0.0f, 1.0f) {}

void CFootBotForaging::SStateData::Init(TConfigurationNode& t_node) {
    try {
        GetNodeAttribute(t_node, "initial_rest_to_explore_prob", InitialRestToExploreProb);
        GetNodeAttribute(t_node, "initial_explore_to_rest_prob", InitialExploreToRestProb);
        GetNodeAttribute(t_node, "food_rule_explore_to_rest_delta_prob", FoodRuleExploreToRestDeltaProb);
        GetNodeAttribute(t_node, "food_rule_rest_to_explore_delta_prob", FoodRuleRestToExploreDeltaProb);
        GetNodeAttribute(t_node, "collision_rule_explore_to_rest_delta_prob", CollisionRuleExploreToRestDeltaProb);
        GetNodeAttribute(t_node, "social_rule_rest_to_explore_delta_prob", SocialRuleRestToExploreDeltaProb);
        GetNodeAttribute(t_node, "social_rule_explore_to_rest_delta_prob", SocialRuleExploreToRestDeltaProb);
        GetNodeAttribute(t_node, "minimum_resting_time", MinimumRestingTime);
        GetNodeAttribute(t_node, "minimum_unsuccessful_explore_time", MinimumUnsuccessfulExploreTime);
        GetNodeAttribute(t_node, "minimum_search_for_place_in_nest_time", MinimumSearchForPlaceInNestTime);
    }
    catch(CARGoSException& ex) {
        THROW_ARGOSEXCEPTION_NESTED("Error initializing controller state parameters.", ex);
    }
}

void CFootBotForaging::SStateData::Reset() {
    State = STATE_RESTING;
    InNest = true;
    RestToExploreProb = InitialRestToExploreProb;
    ExploreToRestProb = InitialExploreToRestProb;
    TimeExploringUnsuccessfully = 0;
    TimeRested = MinimumRestingTime;
    TimeSearchingForPlaceInNest = 0;
}

CFootBotForaging::CFootBotForaging() :
   m_pcWheels(NULL),
   m_pcLEDs(NULL),
   m_pcRABA(NULL),
   m_pcRABS(NULL),
   m_pcProximity(NULL),
   m_pcLight(NULL),
   m_pcGround(NULL),
   m_pcRNG(NULL) {}

void CFootBotForaging::Init(TConfigurationNode& t_node) {
    try {
        m_pcWheels    = GetActuator<CCI_DifferentialSteeringActuator>("differential_steering");
        m_pcLEDs      = GetActuator<CCI_LEDsActuator                >("leds"                 );
        m_pcRABA      = GetActuator<CCI_RangeAndBearingActuator     >("range_and_bearing"    );
        m_pcRABS      = GetSensor  <CCI_RangeAndBearingSensor       >("range_and_bearing"    );
        m_pcProximity = GetSensor  <CCI_FootBotProximitySensor      >("footbot_proximity"    );
        m_pcLight     = GetSensor  <CCI_FootBotLightSensor          >("footbot_light"        );
        m_pcGround    = GetSensor  <CCI_FootBotMotorGroundSensor    >("footbot_motor_ground" );
        m_sDiffusionParams.Init(GetNode(t_node, "diffusion"));
        m_sWheelTurningParams.Init(GetNode(t_node, "wheel_turning"));
        m_sStateData.Init(GetNode(t_node, "state"));
    }
    catch(CARGoSException& ex) {
        THROW_ARGOSEXCEPTION_NESTED("Error initializing the foot-bot foraging controller for robot \"" << GetId() << "\"", ex);
    }
    m_pcRNG = CRandom::CreateRNG("argos");
    Reset();
}

void CFootBotForaging::ControlStep() {
    switch (m_sStateData.State) {
        case SStateData::STATE_RESTING: {
            Rest();
            break;
        }
        case SStateData::STATE_EXPLORING: {
            Explore();
            break;
        }
        case SStateData::STATE_RETURN_TO_NEST: {
            ReturnToNest();
            break;
        }
        default: {
            LOGERR << "We can't be here, there's a bug!" << std::endl;
        }
    }
}

void CFootBotForaging::Reset() {
    m_sStateData.Reset();
    m_sFoodData.Reset();
    m_pcLEDs->SetAllColors(CColor::RED);
    m_eLastExplorationResult = LAST_EXPLORATION_NONE;
    m_pcRABA->ClearData();
    m_pcRABA->SetData(0, LAST_EXPLORATION_NONE);
}

void CFootBotForaging::UpdateState() {
    m_sStateData.InNest = false;
    const CCI_FootBotMotorGroundSensor::TReadings& tGroundReads = m_pcGround->GetReadings();
    if (tGroundReads[2].Value > 0.25f && tGroundReads[2].Value < 0.75f &&
        tGroundReads[3].Value > 0.25f && tGroundReads[3].Value < 0.75f) {
        m_sStateData.InNest = true;
    }
}

CVector2 CFootBotForaging::CalculateVectorToLight() {
    const CCI_FootBotLightSensor::TReadings& tLightReads = m_pcLight->GetReadings();
    CVector2 cAccumulator;
    for (size_t i = 0; i < tLightReads.size(); ++i) {
        cAccumulator += CVector2(tLightReads[i].Value, tLightReads[i].Angle);
    }
    if (cAccumulator.Length() > 0.0f) {
        return CVector2(1.0f, cAccumulator.Angle());
    } else {
        return CVector2();
    }
}

CVector2 CFootBotForaging::DiffusionVector(bool& b_collision) {
    const CCI_FootBotProximitySensor::TReadings& tProxReads = m_pcProximity->GetReadings();
    CVector2 cDiffusionVector;
    for (size_t i = 0; i < tProxReads.size(); ++i) {
        cDiffusionVector += CVector2(tProxReads[i].Value, tProxReads[i].Angle);
    }
    if (m_sDiffusionParams.GoStraightAngleRange.WithinMinBoundIncludedMaxBoundIncluded(cDiffusionVector.Angle())
    && cDiffusionVector.Length() < m_sDiffusionParams.Delta) {
        b_collision = false;
        return CVector2::X;
    } else {
        b_collision = true;
        cDiffusionVector.Normalize();
        return -cDiffusionVector;
    }
}

void CFootBotForaging::SetWheelSpeedsFromVector(const CVector2& c_heading) {
    CRadians cHeadingAngle = c_heading.Angle().SignedNormalize();
    Real fHeadingLength = c_heading.Length();
    Real fBaseAngularWheelSpeed = Min<Real>(fHeadingLength, m_sWheelTurningParams.MaxSpeed);
    if (m_sWheelTurningParams.TurningMechanism == SWheelTurningParams::HARD_TURN) {
        if (Abs(cHeadingAngle) <= m_sWheelTurningParams.SoftTurnOnAngleThreshold) {
            m_sWheelTurningParams.TurningMechanism = SWheelTurningParams::SOFT_TURN;
        }
    }
    if (m_sWheelTurningParams.TurningMechanism == SWheelTurningParams::SOFT_TURN) {
        if (Abs(cHeadingAngle) > m_sWheelTurningParams.HardTurnOnAngleThreshold) {
            m_sWheelTurningParams.TurningMechanism = SWheelTurningParams::HARD_TURN;
        } else if (Abs(cHeadingAngle) <= m_sWheelTurningParams.NoTurnAngleThreshold) {
            m_sWheelTurningParams.TurningMechanism = SWheelTurningParams::NO_TURN;
        }
    }
    if (m_sWheelTurningParams.TurningMechanism == SWheelTurningParams::NO_TURN) {
        if (Abs(cHeadingAngle) > m_sWheelTurningParams.HardTurnOnAngleThreshold) {
            m_sWheelTurningParams.TurningMechanism = SWheelTurningParams::HARD_TURN;
        } else if (Abs(cHeadingAngle) > m_sWheelTurningParams.NoTurnAngleThreshold) {
            m_sWheelTurningParams.TurningMechanism = SWheelTurningParams::SOFT_TURN;
        }
    }
    Real fSpeed1, fSpeed2;
    switch (m_sWheelTurningParams.TurningMechanism) {
        case SWheelTurningParams::NO_TURN: {
            fSpeed1 = fBaseAngularWheelSpeed;
            fSpeed2 = fBaseAngularWheelSpeed;
            break;
        }
        case SWheelTurningParams::SOFT_TURN: {
            Real fSpeedFactor = (m_sWheelTurningParams.HardTurnOnAngleThreshold - Abs(cHeadingAngle)) / m_sWheelTurningParams.HardTurnOnAngleThreshold;
            fSpeed1 = fBaseAngularWheelSpeed - fBaseAngularWheelSpeed * (1.0 - fSpeedFactor);
            fSpeed2 = fBaseAngularWheelSpeed + fBaseAngularWheelSpeed * (1.0 - fSpeedFactor);
            break;
        }
        case SWheelTurningParams::HARD_TURN: {
            fSpeed1 = -m_sWheelTurningParams.MaxSpeed;
            fSpeed2 =  m_sWheelTurningParams.MaxSpeed;
            break;
        }
    }
    Real fLeftWheelSpeed, fRightWheelSpeed;
    if (cHeadingAngle > CRadians::ZERO) {
        fLeftWheelSpeed  = fSpeed1;
        fRightWheelSpeed = fSpeed2;
    } else {
        fLeftWheelSpeed  = fSpeed2;
        fRightWheelSpeed = fSpeed1;
    }
    m_pcWheels->SetLinearVelocity(fLeftWheelSpeed, fRightWheelSpeed);
}

void CFootBotForaging::Rest() {
    if (m_sStateData.TimeRested > m_sStateData.MinimumRestingTime &&
        m_pcRNG->Uniform(m_sStateData.ProbRange) < m_sStateData.RestToExploreProb) {
        m_pcLEDs->SetAllColors(CColor::GREEN);
        m_sStateData.State = SStateData::STATE_EXPLORING;
        m_sStateData.TimeRested = 0;
    } else {
        ++m_sStateData.TimeRested;
        if (m_sStateData.TimeRested == 1) {
            m_pcRABA->SetData(0, LAST_EXPLORATION_NONE);
        }
        const CCI_RangeAndBearingSensor::TReadings& tPackets = m_pcRABS->GetReadings();
        for (size_t i = 0; i < tPackets.size(); ++i) {
            switch(tPackets[i].Data[0]) {
                case LAST_EXPLORATION_SUCCESSFUL: {
                    m_sStateData.RestToExploreProb += m_sStateData.SocialRuleRestToExploreDeltaProb;
                    m_sStateData.ProbRange.TruncValue(m_sStateData.RestToExploreProb);
                    m_sStateData.ExploreToRestProb -= m_sStateData.SocialRuleExploreToRestDeltaProb;
                    m_sStateData.ProbRange.TruncValue(m_sStateData.ExploreToRestProb);
                    break;
                }
                case LAST_EXPLORATION_UNSUCCESSFUL: {
                    m_sStateData.ExploreToRestProb += m_sStateData.SocialRuleExploreToRestDeltaProb;
                    m_sStateData.ProbRange.TruncValue(m_sStateData.ExploreToRestProb);
                    m_sStateData.RestToExploreProb -= m_sStateData.SocialRuleRestToExploreDeltaProb;
                    m_sStateData.ProbRange.TruncValue(m_sStateData.RestToExploreProb);
                    break;
                }
            }
        }
    }
}

void CFootBotForaging::Explore() {
    bool bReturnToNest(false);
    if (m_sFoodData.HasFoodItem) {
        m_sStateData.ExploreToRestProb -= m_sStateData.FoodRuleExploreToRestDeltaProb;
        m_sStateData.ProbRange.TruncValue(m_sStateData.ExploreToRestProb);
        m_sStateData.RestToExploreProb += m_sStateData.FoodRuleRestToExploreDeltaProb;
        m_sStateData.ProbRange.TruncValue(m_sStateData.RestToExploreProb);
        m_eLastExplorationResult = LAST_EXPLORATION_SUCCESSFUL;
        bReturnToNest = true;
    } else if (m_sStateData.TimeExploringUnsuccessfully > m_sStateData.MinimumUnsuccessfulExploreTime) {
        if (m_pcRNG->Uniform(m_sStateData.ProbRange) < m_sStateData.ExploreToRestProb) {
            m_eLastExplorationResult = LAST_EXPLORATION_UNSUCCESSFUL;
            bReturnToNest = true;
        } else {
            m_sStateData.ExploreToRestProb += m_sStateData.FoodRuleExploreToRestDeltaProb;
            m_sStateData.ProbRange.TruncValue(m_sStateData.ExploreToRestProb);
            m_sStateData.RestToExploreProb -= m_sStateData.FoodRuleRestToExploreDeltaProb;
            m_sStateData.ProbRange.TruncValue(m_sStateData.RestToExploreProb);
        }
    }
    if (bReturnToNest) {
        m_sStateData.TimeExploringUnsuccessfully = 0;
        m_sStateData.TimeSearchingForPlaceInNest = 0;
        m_pcLEDs->SetAllColors(CColor::BLUE);
        m_sStateData.State = SStateData::STATE_RETURN_TO_NEST;
    } else {
        ++m_sStateData.TimeExploringUnsuccessfully;
        UpdateState();
        bool bCollision;
        CVector2 cDiffusion = DiffusionVector(bCollision);
        if (bCollision) {
            m_sStateData.ExploreToRestProb += m_sStateData.CollisionRuleExploreToRestDeltaProb;
            m_sStateData.ProbRange.TruncValue(m_sStateData.ExploreToRestProb);
            m_sStateData.RestToExploreProb -= m_sStateData.CollisionRuleExploreToRestDeltaProb;
            m_sStateData.ProbRange.TruncValue(m_sStateData.RestToExploreProb);
        }
        if (m_sStateData.InNest) {
            SetWheelSpeedsFromVector(m_sWheelTurningParams.MaxSpeed * cDiffusion -
            m_sWheelTurningParams.MaxSpeed * 0.25f * CalculateVectorToLight());
        }
        else {
            SetWheelSpeedsFromVector(m_sWheelTurningParams.MaxSpeed * cDiffusion);
        }
    }
}

void CFootBotForaging::ReturnToNest() {
    UpdateState();
    if (m_sStateData.InNest) {
        if (m_sStateData.TimeSearchingForPlaceInNest > m_sStateData.MinimumSearchForPlaceInNestTime) {
            m_pcWheels->SetLinearVelocity(0.0f, 0.0f);
            m_pcRABA->SetData(0, m_eLastExplorationResult);
            m_pcLEDs->SetAllColors(CColor::RED);
            m_sStateData.State = SStateData::STATE_RESTING;
            m_sStateData.TimeSearchingForPlaceInNest = 0;
            m_eLastExplorationResult = LAST_EXPLORATION_NONE;
        return;
        } else {
            ++m_sStateData.TimeSearchingForPlaceInNest;
        }
    } else {
        m_sStateData.TimeSearchingForPlaceInNest = 0;
    }
    bool bCollision;
    SetWheelSpeedsFromVector(m_sWheelTurningParams.MaxSpeed * DiffusionVector(bCollision) +
        m_sWheelTurningParams.MaxSpeed * CalculateVectorToLight());
}

REGISTER_CONTROLLER(CFootBotForaging, "footbot_foraging_controller")