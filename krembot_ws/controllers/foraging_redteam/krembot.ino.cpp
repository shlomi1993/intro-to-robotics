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

void foraging_redteam_controller::setup() {
    krembot.setup();
    writeTeamColor();
    teamName = "foraging_redteam_controller";
    state = move;
    turning_time = 220;
    turning_speed = 100;
    direction = 1;
    if (foragingMsg.ourColor == "green") {
        ourColor = GREEN;
        ourBaseColor = CYAN;
    } else {
        ourColor = RED;
        ourBaseColor = MAGENTA;
    }
}

void foraging_redteam_controller::loop() {
    krembot.loop();

    int8_t colorFront = rgba_to_color(krembot.RgbaFront.readRGBA());
    int8_t colorFrontLeft = rgba_to_color(krembot.RgbaFrontLeft.readRGBA());
    int8_t colorFrontRight = rgba_to_color(krembot.RgbaFrontRight.readRGBA());
    int8_t colorLeft = rgba_to_color(krembot.RgbaLeft.readRGBA());
    int8_t colorRight = rgba_to_color(krembot.RgbaRight.readRGBA());
    BumpersRes bumpers = krembot.Bumpers.read();

    bool nestAhead = colorFront == ourBaseColor;
    bool nestOnRight = (colorFrontRight == ourBaseColor || colorRight == ourBaseColor) && !nestAhead;
    bool nestOnLeft = (colorFrontLeft == ourBaseColor || colorLeft == ourBaseColor) && !nestAhead;
    bool bumped = bumpers.front == BumperState::PRESSED || bumpers.front_left == BumperState::PRESSED ||
                  bumpers.front_right == BumperState::PRESSED;

    // DEBUG
    // cout << krembot.getName() << ": " << nestOnLeft << nestAhead << nestOnRight << ((hasFood) ? " holds food" : "") << endl;

    switch (state) {

        case State::move: {
            if (hasFood) {
                state = State::rtb;
            } else if (bumped) {
                turning_time = (millis_time_t) randomize(100, 500);
                direction = (rand() % 2 == 0) ? -1 : 1;
                sandTimer.start(turning_time);
                state = State::turn;
            } else {
                krembot.Base.drive(100, 0);
            }
            break;
        }

        case State::rtb: {
            if (!hasFood) {
                state = State::move;
            } else if (nestOnRight) {
                krembot.Base.drive(30, -100);
            } else if (nestOnLeft) {
                krembot.Base.drive(30, 100);
            } else if (bumped) {
                turning_time = (millis_time_t) randomize(100, 500);
                direction = (rand() % 2 == 0) ? -1 : 1;
                sandTimer.start(turning_time);
                state = State::turn;
            } else {
                krembot.Base.drive(100, 0);
            }
            break;
        }

        case State::turn: {
            if (sandTimer.finished()) {
                state = (hasFood) ? State::rtb : State::move;
            } else {
                krembot.Base.drive(0, (int8_t) (direction * turning_speed));
            }
            break;
        }

    }

}