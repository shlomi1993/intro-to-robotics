// Shlomi Ben-Shushan 311408264
// Yiftach Neuman 208305359

#include "krembot.ino.h"
#include <string>
#include <random>

using namespace std;

/*
 * Globals:
 * - SandTimer for rotation time settings.
 * - Random Generator to generate floats from a known distribution.
 * - rotation speed should not change beause of the looping.
 */
SandTimer timer;
default_random_engine generator;
int8_t rot_speed = 0;

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

    // If timer is not running, then drive and show green light.
    if (timer.finished()) {

        // Find if one of the front bumpers pressed.
        BumpersRes bumpers = krembot.Bumpers.read();
        bool bumped = bumpers.front == BumperState::PRESSED ||
                      bumpers.front_left == BumperState::PRESSED ||
                      bumpers.front_right == BumperState::PRESSED;

        // If not, drive with green light.
        if (!bumped) {
            krembot.Base.drive(100, 0);
            krembot.Led.write(0,255,0);
        }

        // If the robot bumped, start timer with random time and generate a new
        // random rotation speed, for the next iteration.
        else {
            krembot.Base.stop();
            uniform_int_distribution<int> speed_distribution(-100,100);
            rot_speed = speed_distribution(generator);
            uniform_int_distribution<int> time_distribution(1,3000);
            millis_time_t rot_time = time_distribution(generator);
            timer.setPeriod(rot_time);
            timer.start();
            krembot.Led.write(255,0,0);
        }

    }

    // Whenever the timer is running, turn and show red light.
    else {
        krembot.Base.drive(0, rot_speed);
        krembot.Led.write(255,0,0);
    }

}