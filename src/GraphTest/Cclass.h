#pragma once

#include "Stdafx.h"

struct edge
{
    vector<double> C;

    double value;

    int power;

    int node1;

    int node2;

    // для работы с параллельной редукцией
    int simple;

    bool ex;

    void PrintEdge();
};

edge operator *(edge x, edge y);
edge operator +(edge x, edge y);
edge operator -(edge x, edge y);
edge operator ~(edge x);
edge operator *(int x, edge y);
edge operator *(edge x, int y);