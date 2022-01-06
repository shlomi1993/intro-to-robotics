#include <Krembot/controller/krembot_controller.h>
#include "controllers/foraging/krembot.ino.h"

#define NOT_INITIALIZED -999

class foraging_1_controller : public foraging_controller {
private:
    enum State { spiralMove, spiralTurn, move, rtb, softTurn, hardTurn };
    State state = spiralMove;
    SandTimer sandTimer;
    SandTimer spiralStartTimer, spiralEndTimer;
    int8_t turning_speed = 100;
    int8_t direction = 1;
    Colors ourColor, opponentColor;
    Colors ourBaseColor, opponentBaseColor;
    int spiral_turning_time = 200;
    int adder = 0;

    bool nestLocationIsEstimated = true;
    bool firstFoodCollected = false;
    bool directionSet = false;
public:
    void setup() override;
    void loop() override;
};


REGISTER_CONTROLLER(foraging_1_controller, "foraging_1_controller")