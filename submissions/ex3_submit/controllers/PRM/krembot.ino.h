// Shlomi Ben-Shushan 311408264
// Yiftach Neuman 208305359

#include <Krembot/controller/krembot_controller.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <ctime>
#include <cmath>
#include <random>
#include <queue>
#include <stack>
#include "kdtree.hpp"
#include "logger.h"

#define ANGULAR_SPEED 30
#define MAXIMAL_DEGREE_DIFF 5

struct MapMsg{
    int ** occupancyGrid{};
    Real resolution{};
    CVector2 origin;
    int height{}, width{};
};

struct PosMsg{
    CVector2 pos;
    CDegrees degreeX;
};

class PRM_controller : public KrembotController {
private:
    Real robotSize = 0.20;
    bool isFirst = true;
    bool setup_angle = false;
    CVector2 goal;
    CVector2 starting_position;
    CVector2 next_stop;
    int** new_grid{};
    int path_count = 0;
    CDegrees right_angle = CDegrees(0);
    CDegrees prev_angle;
    std::vector<std::vector<double>> path;
    bool reached_goal = false;
public:
    MapMsg mapMsg;
    PosMsg posMsg;
    ParticleObserver Particle;
    ~PRM_controller() override = default;
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
            goal = CVector2(Real(-2),Real(-2));
            starting_position = CVector2(Real(1),Real(1));
            next_stop = starting_position;
            setup();
            isFirst = false;
        }
        loop();
    }
    static void pos_to_cord(CVector2 pos, double *j, double *i);
    static CVector2 cell_to_cord(int cell_i,int cell_j,int reduction_factor);
    static std::vector<int> coord_to_cell(std::vector<double> coord,double resolution);
    static CDegrees get_position_to_destination(CVector2 pos, CVector2 dst);
    bool close_enough(CVector2 pos,CVector2 dst);
    static int ** create_new_grid(int **grid, int grid_height, int grid_width, double size);
    CVector2 get_next_pos();
    static int **lower_grid_resolution(int **grid, int reduction_factor, int grid_height, int grid_width);
    static void uniform_sample(int &i, int &j, int max_height, int max_width);
    static std::vector<double *> sample_n_free_points(int n_points, int max_height, int max_width, int **grid,
                                                          void (*sampler)(int &, int &, int, int));
    static Kdtree::KdNodeVector insert_points_to_nodes(std::vector<double *> points);
    static double **create_adj_matrix(Kdtree::KdTree &kd_tree, int **&grid,
                                         double (*distance_metric)(std::vector<double>, std::vector<double>, int **,bool));
    static bool obstacle_in_the_middle(std::vector<double> src, std::vector<double> dst, int **grid);
    static double l1_distance(std::vector<double> src, std::vector<double> dst, int **grid, bool driving=false);
    static double l2_distance(std::vector<double> src, std::vector<double> dst, int **grid, bool driving=false);
    static std::vector<std::vector<double>> find_shortest_path(const std::vector<double>& src,
                                                               const std::vector<double>& dst,
                                                               double **adj_matrix, Kdtree::KdTree &kd_tree);

    static std::stack<int> dijkstra(const Kdtree::KdNode& src, const Kdtree::KdNode&  dst, double **adj_matrix);
    static int find_index_of_unvisited_min_index(const double *array, const int *unvisited);
    static std::vector<int> get_neighbours(int node_index, double **adj_matrix);
    static std::vector<std::vector<double>> path_ids_to_coords(std::stack<int> nodes_id_path,
                                                                           Kdtree::KdTree &kd_tree);


    };

REGISTER_CONTROLLER(PRM_controller, "PRM_controller")
