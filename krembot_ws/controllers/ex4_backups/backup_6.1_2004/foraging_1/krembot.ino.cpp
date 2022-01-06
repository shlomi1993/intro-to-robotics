#include "krembot.ino.h"
#include <random>

#define GREEN 10
#define RED 20
#define CYAN 30
#define MAGENTA 40
#define UNDEFINED 50

using namespace std;

int8_t rgba_to_color(RGBAResult x) {
    if (x.Red == 0 && x.Green == 255 && x.Blue == 0) return GREEN;
    if (x.Red == 255 && x.Green == 0 && x.Blue == 0) return RED;
    if (x.Red == 255 && x.Green == 0 && x.Blue == 255) return MAGENTA;
    if (x.Red == 0 && x.Green == 255 && x.Blue == 255) return CYAN;
    return UNDEFINED;
}

int randomize(int from, int to) {
    random_device seed;
    default_random_engine generator(seed());
    uniform_int_distribution<int> distribution(from, to);
    return distribution(generator);
}

void foraging_1_controller::setup() {
    krembot.setup();
    writeTeamColor();
    teamName = "foraging_1_controller";
    state = spiralMove;
    turning_time = 220;
    turning_speed = 100;
    direction = 1;
    if (foragingMsg.ourColor == "green") {
        ourColor = GREEN;
        ourBaseColor = CYAN;
        theirColor = RED;
    } else {
        ourColor = RED;
        ourBaseColor = MAGENTA;
        theirColor = GREEN;
    }
    spiral_turning_time = 200;
    adder = 0;
    spiralStartTimer.start(200);
    spiralEndTimer.start(0);
}

void foraging_1_controller::loop() {
    krembot.loop();

    // Read sensors.
    int8_t colorFront = rgba_to_color(krembot.RgbaFront.readRGBA());
    int8_t colorFrontLeft = rgba_to_color(krembot.RgbaFrontLeft.readRGBA());
    int8_t colorFrontRight = rgba_to_color(krembot.RgbaFrontRight.readRGBA());
    int8_t colorLeft = rgba_to_color(krembot.RgbaLeft.readRGBA());
    int8_t colorRight = rgba_to_color(krembot.RgbaRight.readRGBA());
    float distance = krembot.RgbaFront.readRGBA().Distance;
    BumpersRes bumpers = krembot.Bumpers.read();

    // Understand information.
    bool teammateAhead = colorFront == ourColor || colorFrontLeft == ourColor || colorFrontRight == ourColor;
    bool opponentAhead = colorFront == theirColor || colorFrontLeft == theirColor || colorFrontRight == theirColor;
    bool robotAhead = teammateAhead || opponentAhead;
    bool obstacleAhead = distance < 10 && !robotAhead;
    bool nestAhead = colorFront == ourBaseColor;
    bool nestOnRight = (colorFrontRight == ourBaseColor || colorRight == ourBaseColor) && !nestAhead;
    bool nestOnLeft = (colorFrontLeft == ourBaseColor || colorLeft == ourBaseColor) && !nestAhead;
    bool bumped = bumpers.front == BumperState::PRESSED || bumpers.front_left == BumperState::PRESSED ||
                  bumpers.front_right == BumperState::PRESSED;

    // DEBUG
    cout << krembot.getName() << ": " << nestOnLeft << nestAhead << nestOnRight << ((hasFood) ? " holds food" : "") << endl;

    switch (state) {

        case State::spiralMove: {
            if (hasFood) {
                state = State::rtb;
            }
            else if (bumped) {
                state = State::move;
            }
            else if (spiralStartTimer.finished()) {
                spiralEndTimer.start(220);
                state = State::spiralTurn;
            }
            else {
                krembot.Base.drive(100, 0);
            }
            break;
        }

        case State::spiralTurn: {
            if (spiralEndTimer.finished()) {
                if (adder % 2) spiral_turning_time += 200;
                ++adder;
                spiralStartTimer.start((millis_time_t) spiral_turning_time);
                state = State::spiralMove;
            }
            else {
                krembot.Base.drive(0, 100);
            }
            break;
        }

        case State::move: {
            if (hasFood) {
                state = State::rtb;
            }
            else if (bumped) {
                bool leftPressed = bumpers.front_left == BumperState::PRESSED;
                bool rightPressed = bumpers.front_right == BumperState::PRESSED;
                if (leftPressed && !rightPressed) {
                    direction = -1;
                    turning_time = (millis_time_t) randomize(200, 500);
                }
                else if (!leftPressed && rightPressed) {
                    direction = 1;
                    turning_time = (millis_time_t) randomize(200, 500);
                }
                else {
                    direction = (randomize(0, 10) % 2 == 0) ? -1 : 1;
                    turning_time = (millis_time_t) randomize(300, 580);
                }
                sandTimer.start(turning_time);
                state = State::hardTurn;
            }
            else if (teammateAhead) {
                if (colorFrontLeft == UNDEFINED && (colorFrontRight == ourColor)) direction = 1;
                else if (colorFrontRight == UNDEFINED && (colorFrontLeft == ourColor) ) direction = -1;
                else direction = (randomize(0, 10) % 2 == 0) ? -1 : 1;
                turning_time =(millis_time_t) randomize(200, 400);
                sandTimer.start(turning_time);
                state = State::softTurn;
            }
            else if (obstacleAhead) {
                float rightDist = krembot.RgbaFrontRight.readRGBA().Distance;
                float leftDist = krembot.RgbaFrontLeft.readRGBA().Distance;
                direction = (rightDist < leftDist) ? 1 : -1;
                turning_time = 220;
                sandTimer.start(turning_time);
                state = State::hardTurn;
            }
            else {
                krembot.Base.drive(100, 0);
            }
            break;
        }

        case State::rtb: {
            if (!hasFood) {
                state = State::move;
            }
            else if (nestOnRight) {
                krembot.Base.drive(30, -100);
            }
            else if (nestOnLeft) {
                krembot.Base.drive(30, 100);
            }
            else if (bumped) {
                bool leftPressed = bumpers.front_left == BumperState::PRESSED;
                bool rightPressed = bumpers.front_right == BumperState::PRESSED;
                if (leftPressed && !rightPressed) {
                    direction = -1;
                    turning_time = (millis_time_t) randomize(200, 500);
                }
                else if (!leftPressed && rightPressed) {
                    direction = 1;
                    turning_time = (millis_time_t) randomize(200, 500);
                }
                else {
                    direction = (randomize(0, 10) % 2 == 0) ? -1 : 1;
                    turning_time = (millis_time_t) randomize(300, 580);
                }
                sandTimer.start(turning_time);
                state = State::hardTurn;
            }
            else if (obstacleAhead) {
                float rightDist = krembot.RgbaFrontRight.readRGBA().Distance;
                float leftDist = krembot.RgbaFrontLeft.readRGBA().Distance;
                direction = (rightDist < leftDist) ? 1 : -1;
                turning_time = 220;
                sandTimer.start(turning_time);
                state = State::hardTurn;
            }
            else {
                krembot.Base.drive(100, 0);
            }
            break;
        }

        case State::hardTurn: {
            if (sandTimer.finished()) {
                state = (hasFood) ? State::rtb : State::move;
            } else {
                krembot.Base.drive(0, (int8_t) (direction * turning_speed));
            }
            break;
        }

        case State::softTurn: {
            if (hasFood) {
                state = State::rtb;
            }
            else if (sandTimer.finished()) {
                state = State::move;
            }
            else {
                krembot.Base.drive(50, (int8_t) (direction * turning_speed));
            }
            break;
        }

    }

}

