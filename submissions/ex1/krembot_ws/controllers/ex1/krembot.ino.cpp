// Shlomi Ben-Shushan 311408264
// Khen Aharon 307947515

// We didn't change anything in this file.

#include "krembot.ino.h"

void ex1_controller::setup() {
    krembot.setup();
    krembot.Led.write(0,255,0);
}

void ex1_controller::loop() {
    krembot.loop();
    float distance = krembot.RgbaFront.readRGBA().Distance;
    if (distance < 15) {
        krembot.Base.stop();
        krembot.Led.write(255,0,0);
    } else {
        krembot.Base.drive(100,0);
    }
}
