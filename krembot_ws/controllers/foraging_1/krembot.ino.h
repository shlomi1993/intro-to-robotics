// Shlomi Ben-Shushan & Yiftach Neuman

#include <Krembot/controller/krembot_controller.h>
#include "controllers/foraging/krembot.ino.h"

/*
 * Class:       foraging_1_controller
 * Description: The controller of forager-1 robot.
 */
class foraging_1_controller : public foraging_controller {
private:
    enum State { spiralMove, spiralTurn, move, rtb, softTurn, hardTurn, blockNest };
    State state;
    SandTimer sandTimer, spiralTurnTimer, spiralStartTimer;
    millis_time_t time_until_next_turn;
    int8_t direction;
    int8_t ourColor, theirColor, ourBaseColor, theirBaseColor;
    int8_t adder;
    bool towardOpponentNest;
public:
    void setup() override;
    void loop() override;
};

REGISTER_CONTROLLER(foraging_1_controller, "foraging_1_controller")