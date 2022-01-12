/*
 * AUTHOR: Carlo Pinciroli <cpinciro@ulb.ac.be>
 * An example foraging controller for the foot-bot.
 * This controller is meant to be used with the XML file: experiments/foraging.argos
 */

#ifndef FOOTBOT_FORAGING_H
#define FOOTBOT_FORAGING_H

#include <argos3/core/control_interface/ci_controller.h>
#include <argos3/plugins/robots/generic/control_interface/ci_differential_steering_actuator.h>
#include <argos3/plugins/robots/generic/control_interface/ci_leds_actuator.h>
#include <argos3/plugins/robots/generic/control_interface/ci_range_and_bearing_actuator.h>
#include <argos3/plugins/robots/generic/control_interface/ci_range_and_bearing_sensor.h>
#include <argos3/plugins/robots/foot-bot/control_interface/ci_footbot_proximity_sensor.h>
#include <argos3/plugins/robots/foot-bot/control_interface/ci_footbot_light_sensor.h>
#include <argos3/plugins/robots/foot-bot/control_interface/ci_footbot_motor_ground_sensor.h>
#include <argos3/core/utility/math/rng.h>

using namespace argos;

class CFootBotForaging : public CCI_Controller {
public:
    struct SFoodData {
      bool HasFoodItem;
      size_t FoodItemIdx;
      size_t TotalFoodItems;
      SFoodData();
      void Reset();
    };
    struct SDiffusionParams {
      Real Delta;
      CRange<CRadians> GoStraightAngleRange;
      SDiffusionParams();
      void Init(TConfigurationNode& t_tree);
    };
    struct SWheelTurningParams {
        enum ETurningMechanism {
            NO_TURN = 0, // go straight
            SOFT_TURN,   // both wheels are turning forwards, but at different speeds
            HARD_TURN    // wheels are turning with opposite speeds
        } TurningMechanism;
        CRadians HardTurnOnAngleThreshold;
        CRadians SoftTurnOnAngleThreshold;
        CRadians NoTurnAngleThreshold;
        Real MaxSpeed;
        void Init(TConfigurationNode& t_tree);
    };
    struct SStateData {
        enum EState {
            STATE_RESTING = 0,
            STATE_EXPLORING,
            STATE_RETURN_TO_NEST
        } State;
        bool InNest;
        Real InitialRestToExploreProb;
        Real RestToExploreProb;
        Real InitialExploreToRestProb;
        Real ExploreToRestProb;
        CRange<Real> ProbRange;
        Real FoodRuleExploreToRestDeltaProb;
        Real FoodRuleRestToExploreDeltaProb;
        Real CollisionRuleExploreToRestDeltaProb;
        Real SocialRuleRestToExploreDeltaProb;
        Real SocialRuleExploreToRestDeltaProb;
        size_t MinimumRestingTime;
        size_t TimeRested;
        size_t MinimumUnsuccessfulExploreTime;
        size_t TimeExploringUnsuccessfully;
        size_t MinimumSearchForPlaceInNestTime;
        size_t TimeSearchingForPlaceInNest;
        SStateData();
        void Init(TConfigurationNode& t_node);
        void Reset();
    };

public:
    CFootBotForaging();
    virtual ~CFootBotForaging() {}
    virtual void Init(TConfigurationNode& t_node);
    virtual void ControlStep();
    virtual void Reset();
    virtual void Destroy() {}
    inline bool IsExploring() const {
        return m_sStateData.State == SStateData::STATE_EXPLORING;
    }
    inline bool IsResting() const {
      return m_sStateData.State == SStateData::STATE_RESTING;
    }
    inline bool IsReturningToNest() const {
      return m_sStateData.State == SStateData::STATE_RETURN_TO_NEST;
    }
    inline SFoodData& GetFoodData() {
      return m_sFoodData;
    }

private:
    void UpdateState();
    CVector2 CalculateVectorToLight();
    CVector2 DiffusionVector(bool& b_collision);
    void SetWheelSpeedsFromVector(const CVector2& c_heading);
    void Rest();
    void Explore();
    void ReturnToNest();

private:
    CCI_DifferentialSteeringActuator* m_pcWheels;
    CCI_LEDsActuator* m_pcLEDs;
    CCI_RangeAndBearingActuator*  m_pcRABA;
    CCI_RangeAndBearingSensor* m_pcRABS;
    CCI_FootBotProximitySensor* m_pcProximity;
    CCI_FootBotLightSensor* m_pcLight;
    CCI_FootBotMotorGroundSensor* m_pcGround;
    CRandom::CRNG* m_pcRNG;
    enum ELastExplorationResult {
        LAST_EXPLORATION_NONE = 0,    // nothing to report
        LAST_EXPLORATION_SUCCESSFUL,  // found food item in the last explodation
        LAST_EXPLORATION_UNSUCCESSFUL // no food found in the last exploration
    } m_eLastExplorationResult;
    SStateData m_sStateData;
    SWheelTurningParams m_sWheelTurningParams;
    SDiffusionParams m_sDiffusionParams;
    SFoodData m_sFoodData;
};

#endif