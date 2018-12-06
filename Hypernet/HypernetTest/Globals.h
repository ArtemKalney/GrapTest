#pragma once

#include "Branch.h"
#include "Hypernet.h"

#define MAX_DIMENSIONAL 3

extern std::vector<Branch> Bin;
extern int n, m, k;
extern int ReliableHypernetsCount, UnconnectedHypernetsCount, TwoNodesHypernetsCount, ChainsReduced,
        UnconnectedNodesReduced, PairConnectivityCalls, EdgesReduced, UnsimpleChains;

