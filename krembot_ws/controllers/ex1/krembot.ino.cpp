// Shlomi Ben-Shushan & Yiftach Neuman

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
