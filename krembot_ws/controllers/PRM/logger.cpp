// Shlomi Ben-Shushan 311408264
// Yiftach Neuman 208305359

#include "logger.h"

using namespace std;
using namespace Kdtree;

// node_to_string(): represents KD-Node as string.
string logger::node_to_string(KdNode node) {
    string id = to_string(node._id);
    string x = to_string(node.point[0]);
    string y = to_string(node.point[1]);
    return "id: " + id + "point: (" + x + ',' + y + ")\n";
}

// point_to_string(): represents point as string.
string logger::point_to_string(vector<double> point) {
    return "(" + to_string(point[0]) + ',' + to_string(point[1]) + ")";
}

// grid_to_file(): save grid in a text file.
void logger::grid_to_file(const string& file_path, int **grid, int _height, int _width) {
    ofstream m_cOutput;
    m_cOutput.open(file_path, ios_base::trunc | ios_base::out);
    for (int row = _height - 1; row >= 0; row--) {
        for (int col = 0; col < _width; col++) {
            m_cOutput << grid[row][col] << ' ';
        }
        m_cOutput << endl;
    }
    m_cOutput.close();
}

// reset_logger(): clear given logger file.
void logger::reset_logger(const string& file_path) {
    ofstream m_cOutput;
    m_cOutput.open(file_path, ios_base::trunc | ios_base::out);
    m_cOutput << endl;
    m_cOutput.close();
}

// log_to_file(): add log to a logger file.
void logger::log_to_file(const string& file_path, const string& msg) {
    ofstream m_cOutput;
    m_cOutput.open(file_path, ios_base::app | ios_base::out);
    m_cOutput << msg << endl;
    m_cOutput.close();
}

// adj_mat_to_file(): save adjacent-matrix to a file.
void logger::adj_mat_to_file(double **mat, int dim) {
    ofstream m_cOutput;
    m_cOutput.open("logs/adj_mat.txt", ios_base::trunc | ios_base::out);
    for (int i = 0; i < dim; i++) {
        for (int j = 0; j < dim; j++) {
            m_cOutput << mat[i][j] << ' ';
        }
        m_cOutput << '\n';
    }
    m_cOutput.close();
}

// print_nodes(): Get KD-Tree nodes and save them in a text file.
void logger::print_nodes(const Kdtree::KdNodeVector &nodes) {
    log_to_file("logs/logger.txt", "started_printing nodes:\n");
    for (const auto & node : nodes) {
        log_to_file("logs/logger.txt", node_to_string(node));
    }
}

// path_to_file(): log given path to a text file.
void logger::path_to_file(vector<vector<double>> path, int **grid, int size) {
    // Note: size should be width / REDUCTION_FACTOR;
    int **new_grid = new int *[size];
    for (int i = 0; i < size; i++) {
        new_grid[i] = new int[size];
        for (int j = 0; j < size; j++) {
            new_grid[i][j] = grid[i][j];
        }
    }
    int step = 2;
    for (auto it = path.begin(); it != path.end(); it++) {
        new_grid[(int) it[0][0]][(int) it[0][1]] = step++;
    }
    grid_to_file("logs/path.txt", new_grid, size, size);
}

// path_as_list_to_file(): log given path as list to a text file.
void logger::path_as_list_to_file(const vector<vector<double>>& path) {
    string str;
    for (auto & it : path) {
        str += point_to_string(it);
    }
    ofstream m_cOutput;
    m_cOutput.open("logs/path_as_list.txt", ios_base::trunc | ios_base::out);
    m_cOutput << str << endl;
}

// nodes_to_file(): save nodes in a text file.
void logger::nodes_to_file(KdNodeVector nodes, int **grid, int size) {
    // Note: size should be width / REDUCTION_FACTOR;
    int **new_grid = new int *[size];
    for (int i = 0; i < size; i++) {
        new_grid[i] = new int[size];
        for (int j = 0; j < size; j++) {
            new_grid[i][j] = grid[i][j];
        }
    }
    for (auto it = nodes.begin(); it != nodes.end(); it++) {
        new_grid[(int) it[0].point[0]][(int) it[0].point[1]] = it[0]._id;
    }
    grid_to_file("logs/nodes_in_grid.txt", new_grid, size, size);
}