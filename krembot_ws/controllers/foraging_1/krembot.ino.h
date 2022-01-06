#include <Krembot/controller/krembot_controller.h>
#include "controllers/foraging/krembot.ino.h"

class foraging_1_controller : public foraging_controller {
private:
    enum State { spiralMove, spiralTurn, move, rtb, softTurn, hardTurn };
    State state;
    SandTimer sandTimer;
    SandTimer spiralStartTimer, spiralEndTimer;
    millis_time_t spiral_turning_time;
    millis_time_t turning_time;
    int8_t turning_speed;
    int8_t direction;
    int8_t ourColor, theirColor;
    int8_t ourBaseColor, opponentBaseColor;
    int8_t adder;
public:
    void setup() override;
    void loop() override;
};

REGISTER_CONTROLLER(foraging_1_controller, "foraging_1_controller")