// Shlomi Ben-Shushan 311408264
// Yiftach Neuman 208305359

#include "krembot.ino.h"
#include <string>
#include <random>

using namespace std;

// Initialize a SandTimer for rotation time settings.
SandTimer timer;

// Initialize a random generator to sample random floats from a known distribution.
default_random_engine generator;

// Initialize rotation speed variable globally to keep the same speed in
// different loop iterations.
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

    // Call Krembot's loop function.
    krembot.loop();

    // If timer is not running, then drive and show green light.
    if (timer.finished()) {

        // Find if one of  front bumpers pressed.
        // Note that it doesn't matter if one of the other bumpers pressed
        // because the driving is forward. We found that the rear bumpers never
        // pressed, but sometimes the right or the left bumpers are pressed. We
        // decided to ignore side presses because the driving is forward, so we
        // can save turnings.
        BumpersRes bumpers = krembot.Bumpers.read();
        bool bumped = bumpers.front == BumperState::PRESSED ||
                      bumpers.front_left == BumperState::PRESSED ||
                      bumpers.front_right == BumperState::PRESSED;

        // If the robot didn't bump, then drive with green light.
        if (!bumped) {
            krembot.Base.drive(100, 0);
            krembot.Led.write(0,255,0);
        }

        // Otherwise, the robot did bump, so set timer for a random time and
        // generate random rotation speed, and in the next iteration the
        // algorithm will enter the "else" where it makes the robot turning.
        else {

            // Stop krembot.
            krembot.Base.stop();

            // Sample speed between -100 and 100 and time from 1 ms to 3000 ms
            // from uniform distributions.
            uniform_int_distribution<int> speed_distribution(-100,100);
            rot_speed = speed_distribution(generator);
            uniform_int_distribution<int> time_distribution(1,3000);
            millis_time_t rot_time = time_distribution(generator);

            // Set new rotation period and start time.
            timer.setPeriod(rot_time);
            timer.start();

            // Set LED color to red to indicate "turning".
            krembot.Led.write(255,0,0);

        }

    }

    // Whenever the timer is running, turn and show red light.
    else {
        krembot.Base.drive(0, rot_speed);
        krembot.Led.write(255,0,0);
    }
}