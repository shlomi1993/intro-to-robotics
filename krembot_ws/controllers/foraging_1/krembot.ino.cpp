// Shlomi Ben-Shushan 311408264
// Yiftach Neuman 208305359

#include "krembot.ino.h"
#include <random>

#define GREEN 10
#define RED 20
#define CYAN 30
#define MAGENTA 40
#define UNDEFINED 50

using namespace std;

/*
 * Function:    rgba_to_color
 * Input:       RGBAResult object
 * Output:      int8_t (unsigned char) that represents color.
 * Operation:   Interprets the RGBA object and returns the pre-defined number that represents a color.
 */
int8_t rgba_to_color(RGBAResult x) {
    if (x.Red == 0 && x.Green == 255 && x.Blue == 0) return GREEN;
    if (x.Red == 255 && x.Green == 0 && x.Blue == 0) return RED;
    if (x.Red == 255 && x.Green == 0 && x.Blue == 255) return MAGENTA;
    if (x.Red == 0 && x.Green == 255 && x.Blue == 255) return CYAN;
    return UNDEFINED;
}

/*
 * Function:    randomize
 * Input:       Two integers that delimit the sampling range.
 * Output:      Uniformly random integer.
 * Operation:   Generates uniformly distributed randomized number within the sampling range.
 */
int randomize(int from, int to) {
    random_device seed;
    default_random_engine generator(seed());
    uniform_int_distribution<int> distribution(from, to);
    return distribution(generator);
}

/*
 * Function:    setup
 * Input:       None.
 * Output:      Void.
 * Operation:   Initializes Krembot's attributes.
 */
void foraging_1_controller::setup() {
    krembot.setup();

    // Register for a group and interpret the environment accordingly.
    writeTeamColor();
    teamName = "foraging_1_controller";
    if (foragingMsg.ourColor == "green") {
        ourColor = GREEN;
        ourBaseColor = CYAN;
        theirColor = RED;
    } else {
        ourColor = RED;
        ourBaseColor = MAGENTA;
        theirColor = GREEN;
    }

    // Basic setup.
    state = spiralMove;
    direction = 1;
    time_until_next_turn = 200;
    adder = 0;
    spiralTurnTimer.start(time_until_next_turn);
    spiralStartTimer.start((millis_time_t) randomize(120000, 240000));

}

/*
 * Function:    loop
 * Input:       None.
 * Output:      Void.
 * Operation:   Determines what the Krembot will do in each call.
 */
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

    // Interpret sensing data.
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
    // cout<<krembot.getName()<<": "<<nestOnLeft<<nestAhead<<nestOnRight<<((hasFood) ? " holds food" : "")<<endl;

    if (spiralStartTimer.finished()) {
        state = State::spiralMove;
        spiralStartTimer.start((millis_time_t) randomize(90000, 180000));
    }

    switch (state) {

        // In the spiralling phase, the robot scans its surroundings for food in a spiral motion.
        // This phase ends when the robot finds food or bumped into something.
        case State::spiralMove: {
            if (hasFood) {
                state = State::rtb;
            } else if (bumped) {
                state = State::move;
            } else if (spiralTurnTimer.finished()) {
                sandTimer.start(220);
                state = State::spiralTurn;
            } else {
                krembot.Base.drive(100, 0);
            }
            break;
        }

        // To implement spiral movement, the robot has to turn in 90 degrees and double its range every second time.
        case State::spiralTurn: {
            if (sandTimer.finished()) {
                if (adder % 2)
                    time_until_next_turn += 200;
                ++adder;
                spiralTurnTimer.start(time_until_next_turn);
                state = State::spiralMove;
            } else {
                krembot.Base.drive(0, 100);
            }
            break;
        }

        // After the spiralling phase, the robot is wandering until it finds food. If it senses a teammate ahead, it
        // will soft-turn in order to spread robots in the arena or make way to food-carrying teammates. If it senses an
        // obstacle or bumped into something, it will hard-turn.
        case State::move: {
            if (hasFood) {
                state = State::rtb;
            } else if (bumped) {
                bool leftPressed = bumpers.front_left == BumperState::PRESSED;
                bool rightPressed = bumpers.front_right == BumperState::PRESSED;
                if (leftPressed && !rightPressed) {
                    direction = -1;
                    sandTimer.start((millis_time_t) randomize(200, 500));
                } else if (!leftPressed && rightPressed) {
                    direction = 1;
                    sandTimer.start((millis_time_t) randomize(200, 500));
                } else {
                    direction = (randomize(0, 10) % 2 == 0) ? -1 : 1;
                    sandTimer.start((millis_time_t) randomize(300, 580));
                }
                state = State::hardTurn;
            } else if (teammateAhead) {
                if (colorFrontLeft == UNDEFINED && (colorFrontRight == ourColor)) direction = 1;
                else if (colorFrontRight == UNDEFINED && (colorFrontLeft == ourColor) ) direction = -1;
                else direction = (randomize(0, 10) % 2 == 0) ? -1 : 1;
                sandTimer.start((millis_time_t) randomize(200, 400));
                state = State::softTurn;
            } else if (obstacleAhead) {
                float rightDist = krembot.RgbaFrontRight.readRGBA().Distance;
                float leftDist = krembot.RgbaFrontLeft.readRGBA().Distance;
                direction = (rightDist < leftDist) ? 1 : -1;
                sandTimer.start(220);
                state = State::hardTurn;
            } else {
                krembot.Base.drive(100, 0);
            }
            break;
        }

        // Once the robot finds food, it switches to RTB state, i.e., Return-To-Base. This state is similar to move
        // state except that it makes the robot turn to the nest if it senses its color around, and if it senses a robot
        // ahead, it will slow down to let him pass (unless it bumped into it, and then it will hard-turn).
        case State::rtb: {
            if (!hasFood) {
                state = State::move;
            } else if (nestOnRight) {
                krembot.Base.drive(30, -100);
            } else if (nestOnLeft) {
                krembot.Base.drive(30, 100);
            } else if (bumped) {
                bool leftPressed = bumpers.front_left == BumperState::PRESSED;
                bool rightPressed = bumpers.front_right == BumperState::PRESSED;
                if (leftPressed && !rightPressed) {
                    direction = -1;
                    sandTimer.start((millis_time_t) randomize(200, 500));
                } else if (!leftPressed && rightPressed) {
                    direction = 1;
                    sandTimer.start((millis_time_t) randomize(200, 500));
                } else {
                    direction = (randomize(0, 10) % 2 == 0) ? -1 : 1;
                    sandTimer.start((millis_time_t) randomize(300, 580));
                }
                state = State::hardTurn;
            } else if (obstacleAhead) {
                float rightDist = krembot.RgbaFrontRight.readRGBA().Distance;
                float leftDist = krembot.RgbaFrontLeft.readRGBA().Distance;
                direction = (rightDist < leftDist) ? 1 : -1;
                sandTimer.start(220);
                state = State::hardTurn;
            } else {
                krembot.Base.drive(100, 0);
            }
            break;
        }

        // In hard-turn state the robot's wheels spins in opposite directions.
        case State::hardTurn: {
            if (sandTimer.finished()) {
                state = (hasFood) ? State::rtb : State::move;
            } else {
                krembot.Base.drive(0, (int8_t) (direction * 100));
            }
            break;
        }

        // In soft-turn state the robot's wheels spins in the same directions, but one spins faster than the other.
        case State::softTurn: {
            if (hasFood) {
                state = State::rtb;
            } else if (sandTimer.finished()) {
                state = State::move;
            } else {
                krembot.Base.drive(50, (int8_t) (direction * 100));
            }
            break;
        }

    }

}

