#include "krembot.ino.h"
#include <random>

using namespace std;

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
}

void foraging_redteam_controller::loop() {
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

    switch (state) {

        case State::move: {
            if (hasFood) { state = State::rtb_move; }
            else if (bumped) {
                turning_speed = randomize(50, 100);
                turning_time = randomize(100, 440);
                direction = (randomize(0, 10) % 2 == 0) ? -1 : 1;
                sandTimer.start(turning_time);
                state = State::turn;
            }
            else {
                krembot.Base.drive(100, 0);
            }
            break;
        }

        case State::turn: {
            if (sandTimer.finished()) {
                state = State::move;
            } else {
                krembot.Base.drive(0, direction * turning_speed);
            }
            break;
        }

        case State::rtb_move: {
            if (!hasFood) { state = State::move; }
            else if (nestOnRight) {
                krembot.Base.drive(30, -100);
            } else if (nestOnLeft) {
                krembot.Base.drive(30, 100);
            } else if (bumped) {
                turning_speed = randomize(50, 100);
                turning_time = randomize(100, 440);
                direction = (randomize(0, 10) % 2 == 0) ? -1 : 1;
                sandTimer.start(turning_time);
                state = State::rtb_turn;
            } else {
                krembot.Base.drive(100, 0);
            }
            break;
        }

        case State::rtb_turn: {
            if (sandTimer.finished()) {
                state = State::rtb_move;
            } else {
                krembot.Base.drive(0, direction * turning_speed);
            }
            break;
        }

    }

}

