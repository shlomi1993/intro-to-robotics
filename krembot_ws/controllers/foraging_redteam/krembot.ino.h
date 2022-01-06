#include <Krembot/controller/krembot_controller.h>
#include "controllers/foraging/krembot.ino.h"

#define NOT_INITIALIZED -999

class foraging_redteam_controller : public foraging_controller {
private:
    enum State { move, turn, rtb_move, rtb_turn };
    State state = move;
    SandTimer sandTimer;
    int turning_speed = 100;
    int turning_time = 100;
    int direction = 1;
    Colors ourColor, opponentColor;
    Colors ourBaseColor, opponentBaseColor;
public:
    void setup() override;
    void loop() override;

    bool isTowardNest();

    CDegrees get_position_to_destination(CVector2 cur_pos, CVector2 next_dst);
};


REGISTER_CONTROLLER(foraging_redteam_controller, "foraging_redteam_controller")