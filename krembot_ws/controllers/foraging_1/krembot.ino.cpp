#include "krembot.ino.h"
#include <random>

using namespace std;

Real nestLocation[2] = { NOT_INITIALIZED, NOT_INITIALIZED };

int randomize(int from, int to) {
    random_device seed;
    default_random_engine generator(seed());
    uniform_int_distribution<int> distribution(from, to);
    return distribution(generator);
}

CDegrees foraging_1_controller::get_position_to_destination(CVector2 cur_pos, CVector2 next_dst) {
    double dx = next_dst.GetX() - cur_pos.GetX();
    double dy = next_dst.GetY() - cur_pos.GetY();
    double angle = atan2(dy, dx);
    return CDegrees(angle * 180 / ARGOS_PI).UnsignedNormalize();
}

bool foraging_1_controller::isTowardNest() {
    CVector2 nestPos = CVector2(nestLocation[0], nestLocation[0]);
    CDegrees right_angle = get_position_to_destination(posMsg.pos, nestPos);
    auto diff = NormalizedDifference(posMsg.degreeX.UnsignedNormalize(), right_angle.UnsignedNormalize()).UnsignedNormalize();
    return diff < CDegrees(5);
}

void foraging_1_controller::setup() {
    krembot.setup();
    writeTeamColor();
    teamName = "foraging_1_controller";
    spiralStartTimer.start(200);
    spiralEndTimer.start(0);
}

void foraging_1_controller::loop() {
    krembot.loop();

    if (hasFood) {
        krembot.Base.stop();
//
//
//        firstFoodCollected = true;
//
//        if (nestLocation[0] == NOT_INITIALIZED) {
//            switch (state) {
//
//                case State::move: {
//
//                    Colors colorFront = krembot.RgbaFront.readColor();
//                    Colors colorFrontLeft = krembot.RgbaFrontLeft.readColor();
//                    Colors colorFrontRight = krembot.RgbaFrontRight.readColor();
//                    Colors colorLeft = krembot.RgbaLeft.readColor();
//                    Colors colorRight = krembot.RgbaRight.readColor();
//
//                    float distance = krembot.RgbaFront.readRGBA().Distance;
//
//                    if (foragingMsg.ourColor == "green") {
//                        ourColor = Colors::Green;
//                        ourBaseColor = Colors::Blue;
//                        opponentColor = Colors::Red;
//                        opponentBaseColor = Colors::Magenta;
//                    } else {
//                        ourColor = Colors::Red;
//                        ourBaseColor = Colors::Magenta;
//                        opponentColor = Colors::Green;
//                        opponentBaseColor = Colors::Blue;
//                    }
//
//                    bool obstacleAhead = distance < 20 && (colorFront == Colors::None || colorFrontLeft == Colors::None || colorFrontRight == Colors::None);
//                    bool teammateAhead = colorFront == ourColor;
//                    bool opponentAhead = colorFront == opponentColor;
//                    bool nestAhead = colorFront == ourBaseColor;
//                    bool nestOnRight = colorFrontRight == ourBaseColor || colorRight == ourBaseColor;
//                    bool nestOnLeft = colorFrontLeft == ourBaseColor || colorLeft == ourBaseColor;
//
//                    if (obstacleAhead) {
//                        float rightDist = krembot.RgbaFrontRight.readRGBA().Distance;
//                        float leftDist = krembot.RgbaFrontLeft.readRGBA().Distance;
//                        direction = (rightDist < leftDist) ? 1 : -1;
//                        sandTimer.start(200);
//                        state = State::hardTurn;
//                    } else if (teammateAhead) {
//                        if (colorFrontRight == ourColor && colorFrontLeft == Colors::None) {
//                            direction = 1;
//                        } else if (colorFrontRight == Colors::None && colorFrontLeft == ourColor) {
//                            direction = -1;
//                        } else {
//                            direction = (randomize(0, 10) % 2 == 0) ? -1 : 1;
//                        }
//                        sandTimer.start((millis_time_t) randomize(100, 300));
//                        state = State::softTurn;
//                    } else if (opponentAhead) {
//                        sandTimer.start(200);
//                        state = State::block;
//                        direction = (rand() % 2 == 0) ? -1 : 1;
//                    } else {
//                        krembot.Base.drive(100, 0);
//                    }
//
//                    break;
//
//                }
//
//                case State::hardTurn: {
//                    if (sandTimer.finished()) {
//                        state = State::move;
//                    } else {
//                        krembot.Base.drive(0, direction * turning_speed);
//                    }
//                    break;
//                }
//
//                case State::softTurn: {
//                    if (sandTimer.finished()) {
//                        state = State::move;
//                    } else {
//                        krembot.Base.drive(50, direction * turning_speed);
//                    }
//                    break;
//                }
//
//                case State::block: {
//                    if (sandTimer.finished()) {
//                        state = State::move;
//                    } else {
//                        krembot.Base.drive(0, direction * turning_speed);
//                    }
//                    break;
//                }
//
//            }
//
//        } else {
//
//            switch (state) {
//
//                case State::move: {
//
//                    Colors colorFront = krembot.RgbaFront.readColor();
//                    Colors colorFrontLeft = krembot.RgbaFrontLeft.readColor();
//                    Colors colorFrontRight = krembot.RgbaFrontRight.readColor();
//                    Colors colorLeft = krembot.RgbaLeft.readColor();
//                    Colors colorRight = krembot.RgbaRight.readColor();
//
//                    float distance = krembot.RgbaFront.readRGBA().Distance;
//
//                    if (foragingMsg.ourColor == "green") {
//                        ourColor = Colors::Green;
//                        ourBaseColor = Colors::Blue;
//                        opponentColor = Colors::Red;
//                        opponentBaseColor = Colors::Magenta;
//                    } else {
//                        ourColor = Colors::Red;
//                        ourBaseColor = Colors::Magenta;
//                        opponentColor = Colors::Green;
//                        opponentBaseColor = Colors::Blue;
//                    }
//
//                    bool obstacleAhead = distance < 20 && (colorFront == Colors::None || colorFrontLeft == Colors::None || colorFrontRight == Colors::None);
//                    bool teammateAhead = colorFront == ourColor;
//                    bool opponentAhead = colorFront == opponentColor;
//                    bool nestAhead = colorFront == ourBaseColor;
//                    bool nestOnRight = colorFrontRight == ourBaseColor || colorRight == ourBaseColor;
//                    bool nestOnLeft = colorFrontLeft == ourBaseColor || colorLeft == ourBaseColor;
//
//                    if (directionSet) {
//                        krembot.Base.drive(100, 0);
//                    } else {
//                        krembot.Base.stop();
//                        // set direction....
//                        state = State::hardTurn;
//                    }
//                    break;
//
//                }
//
//                case State::hardTurn: {
//                    if (isTowardNest()) {
//                        directionSet = true;
//                        krembot.Base.drive(100, 0);
//                        state = State::move;
//                    } else {
//                        krembot.Base.drive(0, direction * turning_speed);
//                    }
//                    break;
//                }
//
//                case State::softTurn: {
//                    if (sandTimer.finished()) {
//                        state = State::move;
//                    } else {
//                        krembot.Base.drive(50, direction * turning_speed);
//                    }
//                    break;
//                }
//
//                case State::block: {
//                    if (sandTimer.finished()) {
//                        state = State::move;
//                    } else {
//                        krembot.Base.drive(0, direction * turning_speed);
//                    }
//                    break;
//                }
//
//            }
//
//        }



    }

    else {

        switch (state) {

            case State::move: {

                Colors colorFront = krembot.RgbaFront.readColor();
                Colors colorFrontLeft = krembot.RgbaFrontLeft.readColor();
                Colors colorFrontRight = krembot.RgbaFrontRight.readColor();
                Colors colorLeft = krembot.RgbaLeft.readColor();
                Colors colorRight = krembot.RgbaRight.readColor();

                float distance = krembot.RgbaFront.readRGBA().Distance;

                if (foragingMsg.ourColor == "green") {
                    ourColor = Colors::Green;
                    ourBaseColor = Colors::Blue;
                    opponentColor = Colors::Red;
                    opponentBaseColor = Colors::Magenta;
                } else {
                    ourColor = Colors::Red;
                    ourBaseColor = Colors::Magenta;
                    opponentColor = Colors::Green;
                    opponentBaseColor = Colors::Blue;
                }

                bool obstacleAhead = distance < 20 && (colorFront == Colors::None || colorFrontLeft == Colors::None || colorFrontRight == Colors::None);
                bool teammateAhead = colorFront == ourColor;
                bool opponentAhead = colorFront == opponentColor;
                bool nestAhead = colorFront == ourBaseColor;
                bool nestOnRight = colorFrontRight == ourBaseColor || colorRight == ourBaseColor;
                bool nestOnLeft = colorFrontLeft == ourBaseColor || colorLeft == ourBaseColor;

//                if (firstFoodCollected && nestLocationIsEstimated) {
//                    nestLocation[0] = posMsg.pos.GetX();
//                    nestLocation[1] = posMsg.pos.GetY();
//                    nestLocationIsEstimated = false;
//
//                }
//
//                if ((nestLocation[0] == NOT_INITIALIZED) && (nestAhead || nestOnRight ||nestOnLeft)) {
//                    nestLocation[0] = posMsg.pos.GetX();
//                    nestLocation[1] = posMsg.pos.GetY();
//                    nestLocationIsEstimated = true;
//                }

                if (spiralStartTimer.finished()) {
                    spiralEndTimer.start(220);
                    state = State::spiralTurn;
                }

                else if (obstacleAhead) {
                    float rightDist = krembot.RgbaFrontRight.readRGBA().Distance;
                    float leftDist = krembot.RgbaFrontLeft.readRGBA().Distance;
                    direction = (rightDist < leftDist) ? 1 : -1;
                    sandTimer.start(200);
                    state = State::hardTurn;
                }

                else if (teammateAhead) {
                    if (colorFrontRight == ourColor && colorFrontLeft == Colors::None) direction = 1;
                    else if (colorFrontRight == Colors::None && colorFrontLeft == ourColor) direction = -1;
                    else direction = (randomize(0, 10) % 2 == 0) ? -1 : 1;
                    sandTimer.start((millis_time_t) randomize(100, 300));
                    state = State::softTurn;
                }

                else if (opponentAhead) {
                    sandTimer.start(200);
                    state = State::block;
                    direction = (rand() % 2 == 0) ? -1 : 1;
                }

                else {
                    krembot.Base.drive(100, 0);
                }

                break;

            }

            case State::hardTurn: {
                if (sandTimer.finished()) {
                    state = State::move;
                } else {
                    krembot.Base.drive(0, direction * turning_speed);
                }
                break;
            }

            case State::softTurn: {
                if (sandTimer.finished()) {
                    state = State::move;
                } else {
                    krembot.Base.drive(50, direction * turning_speed);
                }
                break;
            }

            case State::block: {
                if (sandTimer.finished()) {
                    state = State::move;
                } else {
                    krembot.Base.drive(0, direction * turning_speed);
                }
                break;
            }

            case State::spiralTurn: {
                if (spiralEndTimer.finished()) {
                    spiral_turning_time = spiral_turning_time + adder * 200;
                    adder = (adder + 1) % 2;
                    spiralStartTimer.start(spiral_turning_time);
                    state = State::move;
                } else {
                    krembot.Base.drive(0, 100);
                }

                break;
            }

        }


    }

}

