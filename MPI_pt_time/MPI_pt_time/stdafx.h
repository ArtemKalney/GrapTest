#pragma once

#include <stdio.h>
#include <mpi.h>
#include <tchar.h>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <stack>
#include <ctime>
using namespace std;

#define HOST_PROCESSOR 0
#define MAX_DIMENSIONAL 6

#define STOP_TAG 0
#define I_AM_FREE_TAG 1
#define I_NEED_HELP_TAG 2
#define GPINFO_TAG 3
#define SUM_TAG 4
#define NUM0_TAG 5
#define NUM2_TAG 6
#define NUM3_TAG 7
#define NUM4_TAG 8
#define NUM5_TAG 9
#define CHR_TAG 10
#define CH1_TAG 11
#define CH2_TAG 12
#define CHRS_TAG 13
#define DECOMP1_TAG 14
#define DECOMP2_TAG 15
#define DECOMP3_TAG 16
#define BRIDGES_TAG 17
#define PENDUNT_TAG 18
#define FACTORS_TAG 19