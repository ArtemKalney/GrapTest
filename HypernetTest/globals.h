#pragma once

#include "Branch.h"
#include "Hypernet.h"

#define MAX_DIMENSIONAL 3

extern std::vector<Branche> Bin;
extern int n, m, k;
extern int ReliableHypernetsCount, UnconnectedHypernentsCount, TwoNodesHypernetsCount, ChainsReduced,
        UnconnectedNodesReduced, PairConnectivityCalls, EdgesReduced;

