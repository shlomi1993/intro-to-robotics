// Shlomi Ben-Shushan & Yiftach Neuman

#include "krembot.ino.h"
#include <random>

using namespace std;

/*
 * Globals:
 * - SandTimer for rotation time settings.
 * - rotation speed should not be changed from loop to another.
 */
SandTimer timer;
int8_t rot_speed = 0;

/*
 * randomize function:
 * Generates "truly" randomized number from "from" to "to".
 */
int randomize(int from, int to) {
    random_device seed;
    default_random_engine generator(seed());
    uniform_int_distribution<int> distribution(from, to);
    return distribution(generator);
}

/*
 * isBumped function:
 * Returns true if one of the front bumpers of the given krembot is pressed.
 */
bool isBumped(Krembot k) {
    BumpersRes bumpers = k.Bumpers.read();
    return bumpers.front == BumperState::PRESSED ||
           bumpers.front_left == BumperState::PRESSED ||
           bumpers.front_right == BumperState::PRESSED;
}

/*
 * Setup Function:
 * Initializes Krembot and turn on green LED light.
 */
void ex2_controller::setup() {
    krembot.setup();
    krembot.Led.write(0,255,0);
}

/*
 * Loop Function:
 * Command the robot to drive at full speed until one of its front bumpers
 * pressed, and then turn in a random angular speed (and direction) for a random
 * time (up to 3 seconds) and drive again.
 */
void ex2_controller::loop() {
    krembot.loop();

    // If timer is not running, then drive with green light.
    if (timer.finished()) {

        // If not bumped, drive with green light.
        if (!isBumped(krembot)) {
            krembot.Base.drive(100, 0);
            krembot.Led.write(0,255,0);
        }

        // If bumped, start timer with random time and generate a new random
        // rotation speed, for the next iteration.
        else {
            krembot.Base.stop();
            rot_speed = randomize(-100, 100);
            millis_time_t rot_time = randomize(1, 3000);
            timer.setPeriod(rot_time);
            timer.start();
            krembot.Led.write(255,0,0);
        }

    }

    // Whenever the timer is running, turn with red light.
    else {
        krembot.Base.drive(0, rot_speed);
        krembot.Led.write(255,0,0);
    }

}