#pragma once

#include <stdio.h>
#include <mpi.h>
#include <stdio.h>
#include <tchar.h>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <ctime>
#include <stack>
using namespace std;

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
	void printstr();
	void outprintstr();
};

#define HOST_PROCESSOR 0

#define STOP_TAG 0
#define I_AM_FREE_TAG 1
#define I_NEED_HELP_TAG 2
#define SUM_TAG 3
#define GPINFO_TAG 4
#define NUM_TAG 5
#define NUM2_TAG 6
#define NUM3_TAG 7
#define NUM4_TAG 8
#define NUM5_TAG 9