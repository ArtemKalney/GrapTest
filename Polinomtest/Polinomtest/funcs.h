#pragma once

#include "stdafx.h"
#include "Cclass.h"

int Bin(int k,int l);
bool gconnected(vector<vector<edge>> &H);
void renumerate(vector<vector<edge>> &H, int s, int t);
edge procedure(vector<vector<edge>> &H, edge F, bool connected); 