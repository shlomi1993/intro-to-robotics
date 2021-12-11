#include "krembot.ino.h"

using namespace std;

int col, row;
int ** occupancyGrid;
Real resolution;
CVector2 origin;
int height, width;
CVector2 pos;
CDegrees degreeX;
int krembot_radius = 26;  // Krembot radius 25.51.

enum State {
    move,
    turn
} state = turn;

void PRM_controller::grid_to_file(const string &name, int **grid, int h, int w, int robot_i, int robot_j) {
    ofstream out;
    out.open(name, ios_base::trunc | ios_base::out);
    for (int i = h - 1; i >= 0; i--) {
        for (int j = 0; j < w; j++) {
            if (j == robot_j && i == robot_i) {
                out << 2;
            } else {
                out << grid[i][j];
            }
        }
        out << endl;
    }
    out.close();
}


void PRM_controller::inflate_obstacles(int **grid, int h, int w, int radius) {

    // Create proxy zero-grid from order h times w:
    int inflated_grid[h][w];
    for (int i = h - 1; i >= 0; i--) {
        for (int j = 0; j < w; j++) {
            inflated_grid[i][j] = 0;
        }
    }

    // Inflate the new grid to double-inflating:
    for (int i = h - 1; i >= 0; i--) {
        for (int j = 0; j < w; j++) {
            if (grid[i][j] == 1) {
                for (int x = i - radius; x < i + radius; x++) {
                    for (int y = j - radius; y < j + radius; y++) {
                        if (0 <= x && x < w && 0 <= y && y < h) { inflated_grid[x][y] = 1; }
                    }
                }
            }
        }
    }

    // Copy new grid to the original one.
    for (int i = h - 1; i >= 0; i--) {
        for (int j = 0; j < w; j++) {
            grid[i][j] = inflated_grid[i][j];
        }
    }

}

void PRM_controller::pos_to_cord(CVector2 position, int *i, int *j) {
    *i = (position.GetX() - origin.GetX()) / resolution;
    *j = (position.GetY() - origin.GetY()) / resolution;
}


void PRM_controller::pos_to_cell(CVector2 position, int *i, int *j, int size) {
    int pixel_i, pixel_j;
    pos_to_cord(position, &pixel_i, &pixel_j);
    *i = (int) ((pixel_i + 0.5 * size) / size) - 1;
    *j = (int) ((pixel_j + 0.5 * size) / size) - 1;
}


void PRM_controller::setup() {
    krembot.setup();
    krembot.Led.write(0,255,0);

    occupancyGrid = mapMsg.occupancyGrid;
    resolution = mapMsg.resolution;
    origin = mapMsg.origin;
    height = mapMsg.height;
    width = mapMsg.width;

    int i, j;
    pos_to_cell(pos, &i, &j, 3);
    grid_to_file("map.txt", occupancyGrid, height, width, i, j);

    inflate_obstacles(occupancyGrid, height, width, krembot_radius);
    grid_to_file("inflated_map.txt", occupancyGrid, height, width, i, j);

}

void PRM_controller::loop() {
    krembot.loop();

    pos = posMsg.pos;
    degreeX = posMsg.degreeX;

    krembot.Base.drive(100, 0);

}
