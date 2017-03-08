#pragma once

#include "stdafx.h"

struct edge    
{   
    vector<int> C; 
	int power;
	int node1;
	int node2;
	int simple;
	bool ex;
};

edge operator *(edge x, edge y);
edge operator +(edge x, edge y);
edge operator -(edge x, edge y);
edge operator ~(edge x);
edge operator *(int x, edge y);
edge operator *(edge x, int y);