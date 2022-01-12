// Shlomi Ben-Shushan 311408264
// Yiftach Neuman 208305359

#include <Krembot/controller/krembot_controller.h>
#include "controllers/foraging/krembot.ino.h"

/*
 * Class:       foraging_redteam_controller
 * Description: The controller of the red team forager robot.
 */
class foraging_redteam_controller : public foraging_controller {
private:
    enum State { move, rtb, softTurn, hardTurn };
    State state;
    SandTimer sandTimer;
    int8_t direction;
    int8_t ourColor, theirColor, ourBaseColor;
public:
    void setup() override;
    void loop() override;
};

REGISTER_CONTROLLER(foraging_redteam_controller, "foraging_redteam_controller")