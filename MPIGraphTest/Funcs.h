#pragma once

#include "stdafx.h"
#include "Cclass.h"

bool IsConnected(vector<vector<Edge>> &H);
void Renumerate(vector<vector<Edge>> &H, int s, int t);
vector<int> GetNodePower(vector<vector<Edge>> &H);
void DeleteNode(vector<vector<Edge>> &H, int q);
void ContractEdge(vector<vector<Edge>> &H, int q, int w);
bool ElementInside(vector<int> P, int x);
vector<int> GetChain(vector<vector<Edge>> &H, vector<int> &V);

Edge PairwiseConnectivity(vector<vector<Edge>> &H, Edge F, bool connected, int bufSize, int vectorSize);
void ChainReduction1(vector<vector<Edge>> &H, vector<int> V, bool connected, int bufSize, int vectorSize, int size);
void ChainReduction2(vector<vector<Edge>> &H, vector<int> weight, bool connected, int bufSize, int vectorSize,
                     int size);

void SendControl(Str H, int bufSize, int vectorSize, int size);
char *Pack(Edge e, int bufSize, int vectorSize);
Edge Unpack(char buf[], int bufSize, int vectorSize, int position);
char *PackVector(vector<Edge> F, int bufSize, int vectorSize);
vector<Edge> UnpackVector(char *buf, int bufSize, int vectorSize, int position);
char* PackData(Str H, int strBufSize, int bufSize, int vectorSize);
Str UnpackData(char *buf, int bufSize, int vectorSize);