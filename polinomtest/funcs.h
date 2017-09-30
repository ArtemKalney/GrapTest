#pragma once

#include "Cclass.h"

bool ConnectedGraph(vector<vector<edge>> &H);
void Renumerate(vector<vector<edge>> &H, int s, int t);
edge PairwiseConnectivity(vector<vector<edge>> &H, edge F, bool connected);
vector<int> GetNodePowers(vector<vector<edge>> &H);
void DeleteNode(vector<vector<edge>> &H, int q);
void ContractEdge(vector<vector<edge>> &H, int q, int w);
bool ElementInside(vector<int> P, int x);
vector<int> GetChain(vector<vector<edge>> &H, vector<int> &V);
edge ChainReduction1(vector<vector<edge>> &H, edge sum, vector<int> V, bool connected);
edge ChainReduction2(vector<vector<edge>> &H, edge sum, vector<int> weight, bool connected);
