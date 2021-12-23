// Shlomi Ben-Shushan 311408264
// Yiftach Neuman 208305359

#include "krembot.ino.h"

#define SAMPLES 12000    // Number of vertices that will give a solution with high probability in reasonable time.
#define REDUCTION_FACTOR 2  // Make calculations easier.
#define LOGGER              // Debug logs written to text files.
#define K 30                // Reasonable K for KNN algorithm.

using namespace std;
using namespace Kdtree;

int col, row, height, width;
int **occupancyGrid;
Real resolution;
CVector2 origin;
CVector2 pos;
CDegrees degreeX;

// Krembot's setup function - initialize the robot and prepare for the mission.
void PRM_controller::setup() {

    // Initialize parent and turn on yellow light that means "working".
    krembot.setup();
    krembot.Led.write(255, 255, 0);

    // Initialize global variables.
    occupancyGrid = mapMsg.occupancyGrid;
    resolution = mapMsg.resolution;
    origin = mapMsg.origin;
    height = mapMsg.height;
    width = mapMsg.width;

    // Setup starting and goal positions.
    vector<double> starting_pos{ 1, 1 };
    vector<double> goal_pos{ -2, -2 };
    vector<int> starting_cell = coord_to_cell(starting_pos, resolution * REDUCTION_FACTOR);
    vector<int> goal_cell = coord_to_cell(goal_pos, resolution * REDUCTION_FACTOR);
    vector<double> starting_cell_double(starting_cell.begin(), starting_cell.end());
    vector<double> goal_cell_double(goal_cell.begin(), goal_cell.end());

    // Inflate obstacles and create new grid.
    int ** inflated_obstacles_grid = create_new_grid(occupancyGrid, height, width, robotSize / resolution);
    this->new_grid = lower_grid_resolution(inflated_obstacles_grid,REDUCTION_FACTOR,height,width);

    // Sample random points from the arena and convert them to KD-Nodes.
    vector<double *> vec = sample_n_free_points(SAMPLES, height / REDUCTION_FACTOR, width / REDUCTION_FACTOR,
                                                new_grid, uniform_sample);
    KdNodeVector nodes = insert_points_to_nodes(vec);

    // Create a KD-Tree out of the nodes, find the adjacent-matrix find the shortest path from starting point to goal.
    Kdtree::KdTree kd_tree(&nodes);
    double **adj_matrix = calculate_adj_matrix(kd_tree, new_grid, l2_distance);
    this->path = find_shortest_path(starting_cell_double, goal_cell_double, adj_matrix, kd_tree);

#ifdef LOGGER
    logger::reset_logger("logger.txt");
    logger::grid_to_file("grid.txt", occupancyGrid, height, width);
    logger::grid_to_file("inflated.txt", inflated_obstacles_grid, height, width);
    logger::grid_to_file("new_grid.txt", new_grid, height / REDUCTION_FACTOR, width / REDUCTION_FACTOR);
    logger::log_to_file("logger.txt", "created new grid:");
    logger::nodes_to_file(kd_tree.allnodes, new_grid, width / REDUCTION_FACTOR);
    logger::adj_mat_to_file(adj_matrix, SAMPLES);
    logger::path_to_file(path, new_grid, width / REDUCTION_FACTOR);
    logger::path_as_list_to_file(path);
#endif

}

// Krembot loop function - Logic:
//  1) If current position is goal - stop (and switch to green light).
//  2) Find next position.
//  3) Change angle to be toward the next position.
//  4) Drive to the next position.
//  5) Back to 1.
void PRM_controller::loop() {

    // Call parent's loop() and update globals.
    krembot.loop();
    pos = posMsg.pos;
    degreeX = posMsg.degreeX;

    // Reaching to goal stopping condition.
    if (this->close_enough(pos, this->goal)) {
        krembot.Base.stop();
        krembot.Led.write(0, 255, 0);
        this->reached_goal = true;
    }

    // Turning condition.
    if ((this->close_enough(pos, this->next_stop)) && !reached_goal) {
        krembot.Base.stop();
        this->next_stop = this->get_next_pos();
        this->right_angle = get_position_to_destination(pos, this->next_stop);
        this->setup_angle = true;
    }

    // Turning in progress...
    if (this->setup_angle) {
        krembot.Base.drive(0, ANGULAR_SPEED);
    }

    // Driving condition.
    if (((NormalizedDifference(degreeX.UnsignedNormalize(),
                               this->right_angle.UnsignedNormalize()).UnsignedNormalize())
         < CDegrees(MAXIMAL_DEGREE_DIFF)) && !reached_goal) {
        krembot.Base.drive(100, 0);
        this->setup_angle = false;
    }
}

// get_next_pos(): returns the next position that the robot will have to reach.
CVector2 PRM_controller::get_next_pos() {

    // If there is no path from starting position to the goal, return empty path.
    if (path.empty()) {
        LOG << "Couldn't find path." << endl;
        krembot.Led.write(255, 0, 0);
        return this->starting_position;
    }

    // If passed to the waypoints, goal is being reached.
    if (path_count == path.size()) {
        return this->goal;
    }

    // Get and return the next position.
    vector<double> next_coord = this->path.at(this->path_count);
    this->path_count = this->path_count + 1;
    int x = (int) next_coord[0];
    int y = (int) next_coord[1];
    CVector2 next_pos = cell_to_cord(x, y, REDUCTION_FACTOR);
    return next_pos;
}

// close_enough(): tells if the robot is in "touching-distance" from the destination (goal or waypoint).
bool PRM_controller::close_enough(CVector2 current_pos, CVector2 next_pos) {

    // Create vectors of source and destinations.
    vector<double> src, dst;
    int src_i, src_j, dst_i, dst_j;
    pos_to_cord(current_pos, &src_i, &src_j);
    pos_to_cord(next_pos, &dst_i, &dst_j);
    src.push_back(src_i);
    src.push_back(src_j);
    dst.push_back(dst_i);
    dst.push_back(dst_j);

    // Use l2_distance function as it more precise than l1_distance because the robot can drive in diagonals.
    double distance = l2_distance(src, dst, this->new_grid,true);

    // Return true if in "touching-distance:, and false otherwise.
    return ((0 <= distance) && (distance <= 2));

}

// get_position_to_destination(): returns the angle to be towards the next position (or goal).
CDegrees PRM_controller::get_position_to_destination(CVector2 cur_pos, CVector2 next_dst) {
    double dx = next_dst.GetX() - cur_pos.GetX();
    double dy = next_dst.GetY() - cur_pos.GetY();
    double angle = atan2(dy, dx);
    return CDegrees(angle * 180 / ARGOS_PI).UnsignedNormalize();
}

// create_new_grid(): creates new inflated obstacles grid.
int **PRM_controller::create_new_grid(int **grid, int grid_height, int grid_width, double size) {

    // Initialize grid of zeros.
    int **created_grid = new int *[grid_height];
    for (int j = 0; j < grid_height; j++) {
        created_grid[j] = new int[grid_width];
    }
    for (int i = 0; i < grid_height; i++) {
        for (int j = 0; j < grid_width; j++) {
            created_grid[i][j] = 0;
        }
    }

    // Inflate obstacles by placing 1's in every place that the original grid has 1's and around it.
    for (int i = grid_height - 1; i >= 0; i--) {
        for (int j = 0; j < grid_width; j++) {
            if (grid[i][j] == 1) {
                int height_max = i + (int) size + 1;
                int height_min = i - (int) size - 1;
                int width_max = j + (int) size + 1;
                int width_min = j - (int) size - 1;
                for (int ii = min(grid_height - 1, height_max);
                     ii >= max(0, height_min); ii--) {
                    for (int jj = max(width_min, 0);
                         jj < min(grid_width, width_max - 1); jj++) {
                        created_grid[ii][jj] = 1;
                    }
                }
            }
        }
    }

    // Return the newly formed grid.
    return created_grid;
}

// lower_grid_resolution(): create a low resolution grid out of the original one.
int **PRM_controller::lower_grid_resolution(int **grid, int reduction_factor, int grid_height, int grid_width) {
    assert(grid_height % reduction_factor == 0);

    // Define new dimensions.
    int new_height = grid_height / reduction_factor;
    int new_width = grid_width / reduction_factor;
    int **low_res_grid = new int *[new_height];

    // Place 1 in every place that there is a 1 somewhere in the high-resolution cells of the original grid.
    for (int j = 0; j < new_height; j++) {
        low_res_grid[j] = new int[new_width];
    }
    for (int i = 0; i < grid_height; i++) {
        for (int j = 0; j < grid_width; j++) {
            low_res_grid[i / reduction_factor][j / reduction_factor] = max(0, grid[i][j]);
        }
    }

    // Return the newly formed grid.
    return low_res_grid;
}

// cell_to_cord(): converts the (i,j)-cell to an (x,y) coordinates.
CVector2 PRM_controller::cell_to_cord(int cell_i, int cell_j, int reduction_factor) {
    double half_cell = reduction_factor * resolution / 2;
    Real x_cord = half_cell + origin.GetX() + reduction_factor * resolution * cell_j;
    Real y_cord = half_cell + origin.GetY() + reduction_factor * resolution * cell_i;
    return {x_cord, y_cord};    // Return the coordinate in the middle of the cell.
}

// uniform_sample(): uniformly sample random position (i, j).
void PRM_controller::uniform_sample(int &i, int &j, int max_height, int max_width) {
    std::random_device rd;  // Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> height_dis(0, max_height);
    std::uniform_real_distribution<> weight_dis(0, max_width);
    i = (int) height_dis(gen);
    j = (int) weight_dis(gen);
}

// sample_n_free_points(): generates N random obstacle-free points - position in the arena.
// Sampling function is given by the argument "sampler".
vector<double *> PRM_controller::sample_n_free_points(int n_points, int max_height, int max_width, int **grid,
                                                      void (*sampler)(int &, int &, int, int)) {
    vector<double *> free_points;
    for (int i = 0; i < n_points; i++) {
        int new_col, new_row;
        sampler(new_row, new_col, max_height, max_width);
        while (grid[new_row][new_col] != 0)
            sampler(new_row, new_col, max_height, max_width);
        auto point = new double[2];
        point[0] = new_row;
        point[1] = new_col;
        free_points.push_back(point);
    }
    return free_points;
}

// insert_points_to_nodes(): wrap each point from "points" in a KD-Node so the algorithm can use them to form KD-Tree.
KdNodeVector PRM_controller::insert_points_to_nodes(vector<double *> points) {
    KdNodeVector nodes;
    for (auto it = points.begin(); it != points.end(); it++) {
        vector<double> point(2);
        point[0] = it[0][0];
        point[1] = it[0][1];
        nodes.push_back(KdNode(point));
    }
    return nodes;
}

// calculate_adj_matrix(): create adjacent-matrix by using the grid, the KNN and a distance-metric-function.
double **PRM_controller::calculate_adj_matrix(KdTree &kd_tree, int **&grid,
                                              double (*distance_metric)(vector<double>, vector<double>, int **,bool)) {
    KdNodeVector points_list = kd_tree.allnodes;
    auto **adj_matrix = new double *[points_list.size()];
    for (int i = 0; i < points_list.size(); i++) {
        adj_matrix[i] = new double[points_list.size()];
        KdNodeVector results;
        kd_tree.k_nearest_neighbors(points_list.at(i).point, K, &results, nullptr);
        for (auto neighbour_it = results.begin(); neighbour_it != results.end(); neighbour_it++) {
            double distance = distance_metric(points_list.at(i).point, neighbour_it[0].point, grid, false);
            if (distance > 0) adj_matrix[i][neighbour_it->_id] = distance;
        }
    }
    return adj_matrix;
}

// obstacle_in_the_middle(): returns true if there is an obstacle on the way from current position to the next position.
// Logic:
//  1) Sample enough points along the way from current position to the next position.
//  2) for each sample:
//      a) check if there is an occupied cell.
//      b) if so, return true - there is an obstacle on the way.
//  3) return false - the way is clear,
// Note: special treatment given to the case where both position have the same x-value.
bool PRM_controller::obstacle_in_the_middle(vector<double> src, vector<double> dst, int **grid) {
    double distance = sqrt(pow((src[0] - dst[0]), 2) + pow(src[1] - dst[1], 2));
    int n_samples = 3 * (int) distance;
    double x_diff = dst[0] - src[0];
    if (x_diff != 0) {
        double slope = ((src[1] - dst[1]) / (src[0] - dst[0]));
        double n = src[1] - slope * src[0];
        for (int i = 0; i < n_samples; i++) {
            int x = (int) (src[0] + ((double) i / n_samples) * x_diff);
            int y = (int) (x * slope + n);
            if (grid[x][y] == 1) return true;
        }
    } else {
        double y_diff = dst[1] - src[1];
        for (int i = 0; i < n_samples; i++) {
            int x = (int) src[0];
            int y = (int) (src[1] + ((double) i / n_samples) * y_diff);
            if (grid[x][y] == 1) return true;
        }
    }
    return false;
}

// l1_distance(): L1 distance metric -- unused function due to the selection of L2.
__attribute__((unused)) double PRM_controller::l1_distance(vector<double> src, vector<double> dst, int **grid, bool driving) {
    if ((src[0] == dst[0]) && (src[1] == dst[1])) return 0;
    if (obstacle_in_the_middle(src, dst, grid) && !driving) return -1;
    return abs(src[0] - dst[0])+ abs(src[1] - dst[1]);
}

// l2_distance(): L2 distance metric -- the better metric due to the fact that the robot can move in diagonals.
double PRM_controller::l2_distance(vector<double> src, vector<double> dst, int **grid, bool driving) {
    if ((src[0] == dst[0]) && (src[1] == dst[1])) return 0;
    if (obstacle_in_the_middle(src, dst, grid) && !driving) return -1;
    return sqrt(pow((src[0] - dst[0]), 2) + pow(src[1] - dst[1], 2));
}

// find_shortest_path(): using Dijkstra algorithtm to find the shortest path from the first node to the last.
vector<vector<double>> PRM_controller::find_shortest_path(const vector<double>& src, const vector<double>& dst,
                                          double **adj_matrix, KdTree &kd_tree) {
    KdNodeVector first_node;
    kd_tree.k_nearest_neighbors(src, 1, &first_node);
    KdNodeVector last_node;
    kd_tree.k_nearest_neighbors(dst, 1, &last_node);
    stack<int> nodes_id_path;
    nodes_id_path = dijkstra(first_node.at(0), last_node.at(0), adj_matrix);
    vector<vector<double>> path = convert_path_from_id_to_coords(nodes_id_path, kd_tree);
    return path;
}

// dijkstra(): Implementation of Dijkstra algorithm to find the shortest path between two points.
stack<int> PRM_controller::dijkstra(const KdNode& src, const KdNode&  dst, double **adj_matrix) {
    int previous_node[SAMPLES];
    double distances[SAMPLES];
    int unvisited[SAMPLES];
    fill_n(previous_node, SAMPLES, 99999);
    fill_n(distances, SAMPLES, 999999);
    fill_n(unvisited, SAMPLES, 1);
    distances[src._id] = 0;
    stack<int> path;
    while (unvisited[dst._id] == 1) {
        int next_node = find_index_of_unvisited_min_index(distances, unvisited);
        if (next_node == -1){
            LOG << "Couldn't find path." << endl;
            return path;
        }
        unvisited[next_node] = 0;
        vector<int> neighbours_of_node = get_neighbours(next_node, adj_matrix);
        for (auto it = neighbours_of_node.begin(); it != neighbours_of_node.end(); it++) {
            double alternative_distance = distances[next_node] + adj_matrix[next_node][it[0]];
            if (alternative_distance < distances[it[0]]) {
                distances[it[0]] = alternative_distance;
                previous_node[it[0]] = next_node;
            }
        }
    }
    int previous_step = dst._id;
    while (previous_step != src._id) {
        path.push(previous_step);
        previous_step = previous_node[previous_step];
    }
    path.push(previous_node[previous_step]);
    return path;
}

// find_index_of_unvisited_min_index(): Dijkstra helper function that returns the index of the nest unvisited node.
int PRM_controller::find_index_of_unvisited_min_index(const double *array, const int *unvisited) {
    double min = 999999;
    int best_index = -1;
    int i = 0;
    for (; i < SAMPLES; i++) {
        if (array[i] < min && unvisited[i] == 1) {
            min = array[i];
            best_index = i;
        }
    }
    return best_index;
}

// get_neighbours(): returns a vector of indexes of the neighbors of a given node ug adj_matrix.
vector<int> PRM_controller::get_neighbours(int node_index, double **adj_matrix) {
    vector<int> neighbours;
    for (int i = 0; i < SAMPLES; i++) {
        if ((adj_matrix[node_index][i] > 0) && adj_matrix[node_index][i] < 10) {
            neighbours.push_back(i);

        }
    }
    return neighbours;
}

// convert_path_from_id_to_coords(): convert each Node-ID on the path to coordinates, and return them in a vector.
vector<vector<double>> PRM_controller::convert_path_from_id_to_coords(stack<int> nodes_id_path, KdTree &kd_tree) {
    vector<vector<double>> coords_path;
    KdNodeVector nodes = kd_tree.allnodes;
    while (!nodes_id_path.empty()) {
        int id = nodes_id_path.top();
        nodes_id_path.pop();
        KdNode node_of_id;
        for (auto it = nodes.begin(); it != nodes.end(); it++) {
            if (it[0]._id == id) {
                coords_path.push_back(it[0].point);
                break;
            }
        }
    }
    return coords_path;
}

// pos_to_cord(): converts CVector2 to i and j integers.
void PRM_controller::pos_to_cord(CVector2 position, int *i, int *j) {
    *i = (int) ((position.GetX() - origin.GetX()) / (resolution * REDUCTION_FACTOR));
    *j = (int) ((position.GetY() - origin.GetY()) / (resolution * REDUCTION_FACTOR));
}

// coord_to_cell(): get the cell that contains the given coordinates.
vector<int> PRM_controller::coord_to_cell(vector<double> coord, double res) {
    row = (int) ((coord[0] - origin.GetX()) / res);
    col = (int) ((coord[1] - origin.GetY()) / res);
    vector<int> cell{ row, col };
    return cell;
}