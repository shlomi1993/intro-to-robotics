#include <Krembot/controller/krembot_controller.h>
#include "controllers/foraging/krembot.ino.h"

class foraging_redteam_controller : public foraging_controller {
private:
    enum State { move, rtb, turn };
    State state;
    SandTimer sandTimer;
    millis_time_t turning_time;
    int8_t turning_speed;
    int8_t direction;
    int8_t ourColor, ourBaseColor;
public:
    void setup() override;
    void loop() override;
};

REGISTER_CONTROLLER(foraging_redteam_controller, "foraging_redteam_controller")