// Shlomi Ben-Shushan & Yiftach Neuman

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "kdtree.hpp"

#ifndef ARGOS3_USER_LOGGER_H
#define ARGOS3_USER_LOGGER_H

// Logger class contains logging-to-file functions for debugging.
class logger {
private:
    static std::string node_to_string(Kdtree::KdNode node);
    static std::string point_to_string(std::vector<double> point);
public:
    static void reset_logger(const std::string &file_path);
    static void grid_to_file(const std::string &file_path, int **grid, int _height, int _width);
    static void log_to_file(const std::string &file_path, const std::string &msg);
    static void adj_mat_to_file(double **mat, int dim);
    static void print_nodes(const Kdtree::KdNodeVector &nodes);
    static void path_to_file(std::vector<std::vector<double>> path, int **grid, int size);
    static void path_as_list_to_file(const std::vector<std::vector<double>> &path);
    static void nodes_to_file(Kdtree::KdNodeVector nodes, int **grid, int size);
};


#endif //ARGOS3_USER_LOGGER_H
