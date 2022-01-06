#include "krembot.ino.h"
#include <random>

using namespace std;

int randomize(int from, int to) {
    random_device seed;
    default_random_engine generator(seed());
    uniform_int_distribution<int> distribution(from, to);
    return distribution(generator);
}

void foraging_1_controller::setup() {
    krembot.setup();
    cout << krembot.getId() << " : " << krembot.getName() << endl;
    writeTeamColor();
    teamName = "foraging_1_controller";
    spiralStartTimer.start(200);
    spiralEndTimer.start(0);
}

void foraging_1_controller::loop() {
    krembot.loop();

    Colors colorFront = krembot.RgbaFront.readColor();
    Colors colorFrontLeft = krembot.RgbaFrontLeft.readColor();
    Colors colorFrontRight = krembot.RgbaFrontRight.readColor();
    Colors colorLeft = krembot.RgbaLeft.readColor();
    Colors colorRight = krembot.RgbaRight.readColor();
    float distance = krembot.RgbaFront.readRGBA().Distance;
    BumpersRes bumpers = krembot.Bumpers.read();
    bool bumped = bumpers.front == BumperState::PRESSED ||
                  bumpers.front_left == BumperState::PRESSED ||
                  bumpers.front_right == BumperState::PRESSED;

    if (foragingMsg.ourColor == "green") {
        ourColor = Colors::Green;
        ourBaseColor = Colors::Blue;
        opponentColor = Colors::Red;
    } else {
        ourColor = Colors::Red;
        ourBaseColor = Colors::Magenta;
        opponentColor = Colors::Green;
    }

    bool obstacleAhead = distance < 10 && (colorFront == Colors::None || colorFrontLeft == Colors::None || colorFrontRight == Colors::None);
    bool teammateAhead = colorFront == ourColor || colorFrontLeft == ourColor || colorFrontRight == ourColor;
    bool opponentAhead = colorFront == opponentColor || colorFrontLeft == opponentColor || colorFrontRight == opponentColor;
    bool robotAhead = teammateAhead || opponentAhead;
    bool nestAhead = colorFront == ourBaseColor;
    bool nestOnRight = (colorFrontRight == ourBaseColor || colorRight == ourBaseColor) && !nestAhead;
    bool nestOnLeft = (colorFrontLeft == ourBaseColor || colorLeft == ourBaseColor) && !nestAhead;

//    if (nestOnLeft || nestAhead || nestOnRight)
//        cout << nestOnLeft << nestAhead << nestOnRight << " and hasFood=" << hasFood << endl;

    switch (state) {

        case State::spiralMove: {
            if (hasFood) { state = State::rtb; }
            else if (bumped) { state = State::move; }
            else if (spiralStartTimer.finished()) {
                spiralEndTimer.start(220);
                state = State::spiralTurn;
            } else {
                krembot.Base.drive(100, 0);
            }
            break;
        }

        case State::spiralTurn: {
            if (spiralEndTimer.finished()) {
                spiral_turning_time = spiral_turning_time + adder * 200;
                adder = (adder + 1) % 2;
                spiralStartTimer.start(spiral_turning_time);
                state = State::spiralMove;
            }
            else {
                krembot.Base.drive(0, 100);
            }
            break;
        }

        case State::move: {
            if (hasFood) { state = State::rtb; }
            else if (obstacleAhead) {
                float rightDist = krembot.RgbaFrontRight.readRGBA().Distance;
                float leftDist = krembot.RgbaFrontLeft.readRGBA().Distance;
                direction = (rightDist < leftDist) ? 1 : -1;
                sandTimer.start(220);
                state = State::hardTurn;
            }
            else if (teammateAhead) {
                if (colorFrontRight == ourColor && colorFrontLeft == Colors::None) direction = 1;
                else if (colorFrontRight == Colors::None && colorFrontLeft == ourColor) direction = -1;
                else direction = (randomize(0, 10) % 2 == 0) ? -1 : 1;
                sandTimer.start((millis_time_t) randomize(100, 300));
                state = State::softTurn;
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
            if (hasFood) { state = State::rtb; }
            else if (sandTimer.finished()) {
                state = State::move;
            } else {
                krembot.Base.drive(50, direction * turning_speed);
            }
            break;
        }

        case State::rtb: {
            if (!hasFood) {
                state = State::move;
            }
            else if (nestOnRight) {
                krembot.Base.drive(30, -100);
            } else if (nestOnLeft) {
                krembot.Base.drive(30, 100);
            } else if (obstacleAhead) {
                direction = (randomize(0, 10) % 2 == 0) ? -1 : 1;
                sandTimer.start(220);
                state = State::avoidObstacle;
            } else if (robotAhead) {
                float rightDist = krembot.RgbaFrontRight.readRGBA().Distance;
                float leftDist = krembot.RgbaFrontLeft.readRGBA().Distance;
                direction = (rightDist < leftDist) ? 1 : -1;
                if (bumped) {
                    direction = (randomize(0, 10) % 2 == 0) ? -1 : 1;
                    sandTimer.start(220);
                    state = State::avoidObstacle;
                } else {
                    krembot.Base.drive(50, 0);
                }
            } else {
                krembot.Base.drive(100, 0);
            }
            break;
        }

        case State::avoidObstacle: {
            if (sandTimer.finished()) {
                state = State::rtb;
            } else {
                krembot.Base.drive(0, direction * turning_speed);
            }
            break;
        }

    }

}

