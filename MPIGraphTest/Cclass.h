#pragma once

#include "stdafx.h"

struct Edge {
    vector<double> C;
    int Node1;
    int Node2;
    int Power;
    int Simple;
    bool Ex;
    void PrintEdge();
};

struct Str {
    vector<vector<Edge>> Graph;
    vector<Edge> F;
    bool Ex;
    vector<bool> Visited;
    void PrintStr();
};

Edge operator *(Edge x, Edge y);
Edge operator +(Edge x, Edge y);
Edge operator -(Edge x, Edge y);
Edge operator ~(Edge x);
Edge operator *(int x, Edge y);
Edge operator *(Edge x, int y);