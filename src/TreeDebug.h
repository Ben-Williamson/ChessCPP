//
// Created by ben on 16/05/25.
//

#ifndef CHESS_TREEDEBUG_H
#define CHESS_TREEDEBUG_H

#include <iostream>
#include <fstream>
#include <vector>

struct Node {
    std::string label;
};

struct Edge {
    int source;
    int target;
};

class TreeDebug {
    std::ofstream output_file;
    std::vector<Node> nodes;
    std::vector<Edge> edges;

    void WriteHeaders();
    void WriteNodes();
    void WriteEdges();
    void WriteFooter();

public:
    TreeDebug();
    ~TreeDebug();

    int AddNode(std::string);
    void AddEdge(int, int);

};

#endif