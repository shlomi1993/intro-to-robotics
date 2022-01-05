#include <Krembot/controller/krembot_controller.h>
#include "controllers/foraging/krembot.ino.h"

#define NOT_INITIALIZED -999

class foraging_1_controller : public foraging_controller {
private:
    enum State { move, hardTurn, softTurn, spiralTurn, block };
    State state = move;
    SandTimer sandTimer;
    int turning_speed = 100;
    int direction = 1;
    Colors ourColor, opponentColor;
    Colors ourBaseColor, opponentBaseColor;
    bool nestLocationIsEstimated = true;
    bool firstFoodCollected = false;
    bool directionSet = false;
    SandTimer spiralStartTimer, spiralEndTimer;
    int spiral_turning_time = 200;
    int adder = 0;
public:
    void setup() override;
    void loop() override;

    bool isTowardNest();

    CDegrees get_position_to_destination(CVector2 cur_pos, CVector2 next_dst);
};


REGISTER_CONTROLLER(foraging_1_controller, "foraging_1_controller")