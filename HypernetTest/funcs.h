#pragma once

#include "Branch.h"
#include "Hypernet.h"

Branche PairwiseConnectivity(H &H, Branche &pseudoEdge, bool connected);
// debug
Branche SimplePairwiseConnectivity(H &H, Branche &pseudoEdge);
