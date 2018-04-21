#pragma once

#include "Branch.h"
#include "Hypernet.h"

Branche PairwiseConnectivity(H &H, Branche &pseudoEdge);
// debug
void GenCombinations(const H& H, const std::vector<Branche>& branchList, Branche& sum, std::vector<int>& brancheMask,
                     int& curPos);
Branche SimplePairwiseConnectivity(H &H, Branche &pseudoEdge);
