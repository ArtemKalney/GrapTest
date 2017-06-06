#pragma once

#include "stdafx.h"

struct edge {
	vector<double> C;
	int node1;
	int node2;
	int power;
	int simple;
	bool ex;
	void printedge();
	void outprintedge();
};

struct str {
	vector<vector<edge>> G;
	vector<edge> F;
	bool ex;
	vector<bool> V;
	void printstr();
	void outprintstr();
};

edge operator *(edge x, edge y);
edge operator +(edge x, edge y);
edge operator -(edge x, edge y);
edge operator ~(edge x);
edge operator *(int x, edge y);
edge operator *(edge x, int y);