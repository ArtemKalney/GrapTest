#pragma once

#include "stdafx.h"
#include "Cclass.h"

bool gconnected(vector<vector<edge>> &H);
void renumerate(vector<vector<edge>> &H, int s, int t);
vector<int> gnodepower(vector<vector<edge>> &H);
void delnode(vector<vector<edge>> &H, int q);
void contractedge(vector<vector<edge>> &H, int q, int w);
bool exis(vector<int> P, int x);
vector<int> fchain(vector<vector<edge>> &H, vector<int> &V);

edge procedure(vector<vector<edge>> &H, edge F, bool connected, int buf_size, int vector_size);
void chainreduction1(vector<vector<edge>> &H, vector<int> V, bool connected, int buf_size, int vector_size, int size);
void chainreduction2(vector<vector<edge>> &H, vector<int> weight, bool connected, int buf_size, int vector_size, int size);

void send_control(str H, int buf_size, int vector_size, int size);
char *Pack(edge e, int buf_size, int vector_size);
edge Unpack(char buf[], int buf_size, int vector_size, int position);
char *Pack_vector(vector<edge> F, int buf_size, int vector_size);
vector<edge> Unpack_vector(char buf[], int buf_size, int vector_size, int position);
char* Pack_data(str H, int str_buf_size, int buf_size, int vector_size);
str Unpack_data(char* buf, int buf_size, int vector_size);