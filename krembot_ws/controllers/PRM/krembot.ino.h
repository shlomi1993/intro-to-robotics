#include <Krembot/controller/krembot_controller.h>
#include <queue>

struct MapMsg{
    int ** occupancyGrid;
    Real resolution;
    CVector2 origin;
    int height, width;
};

struct PosMsg{
    CVector2 pos;
    CDegrees degreeX;
};


class PRM_controller : public KrembotController {
private:
    Real robotSize = 0.20;
    bool isFirst = true;
public:
    MapMsg mapMsg;
    PosMsg posMsg;

    ParticleObserver Particle;
    ~PRM_controller() = default;
    void setup();
    void loop();

    void Init(TConfigurationNode &t_node) override {
        KrembotController::Init(t_node);
        if ( ! krembot.isInitialized() ) {
            throw std::runtime_error("krembot.ino.cpp: krembot wasn't initialized in controller");
        }
        Particle.setName(krembot.getName());
    }
    void ControlStep() override {
        if(isFirst) {
            setup();
            isFirst = false;
        }
        loop();
    }

    void save_grid_to_file(const std::string &name, int **grid, int h, int w);

    static void
    grid_to_file(const std::string &name, int **grid, int h, int w, int robot_i, int robot_j);

    static void pos_to_cord(CVector2 pos, int *j, int *i);

    static void pos_to_cell(CVector2 position, int *i, int *j, int size);

    void inflate(int **grid, int x, int y, int h, int w, int radius);

    static void inflate_obstacles(int **grid, int h, int w, int radius);

    void inflate(int **from_grid, int **to_grid, int x, int y, int h, int w, int radius);
};


REGISTER_CONTROLLER(PRM_controller, "PRM_controller")
