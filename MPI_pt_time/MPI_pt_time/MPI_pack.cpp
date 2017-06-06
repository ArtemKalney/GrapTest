#include "stdafx.h"
#include "Cclass.h"
#include "globals.h"

char *Pack(edge e, int buf_size, int vector_size) {
	char *buf = new char[buf_size];
	if (e.C.empty()) e.C.resize(vector_size); // send 0, don't send 1
	int position = 0;

	MPI_Pack(&e.C.front(), vector_size, MPI_DOUBLE, buf, buf_size, &position, MPI_COMM_WORLD);
	MPI_Pack(&e.power, 1, MPI_INT, buf, buf_size, &position, MPI_COMM_WORLD);
	MPI_Pack(&e.simple, 1, MPI_INT, buf, buf_size, &position, MPI_COMM_WORLD);
	MPI_Pack(&e.ex, 1, MPI_BYTE, buf, buf_size, &position, MPI_COMM_WORLD);

	return buf;
}

edge Unpack(char buf[], int buf_size, int vector_size, int position) {
	edge e;
	e.C.resize(vector_size); // need size

	MPI_Unpack(buf, buf_size, &position, &e.C.front(), vector_size, MPI_DOUBLE, MPI_COMM_WORLD);
	MPI_Unpack(buf, buf_size, &position, &e.power, 1, MPI_INT, MPI_COMM_WORLD);
	MPI_Unpack(buf, buf_size, &position, &e.simple, 1, MPI_INT, MPI_COMM_WORLD);
	MPI_Unpack(buf, buf_size, &position, &e.ex, 1, MPI_BYTE, MPI_COMM_WORLD);

	if (!e.ex) {
		edge n; // not fill anything
		n.ex = false; // need this becouse 204 is true
		return n;
	}
	return e;
}

char *Pack_vector(vector<edge> F, int buf_size, int vector_size) {
	int vector_buf_size = F.size()*buf_size + sizeof(int), a = F.size();
	char *buf = new char[vector_buf_size], *buf_edge;
	int position = 0;
	MPI_Pack(&a, 1, MPI_INT, buf, vector_buf_size, &position, MPI_COMM_WORLD);
	for (int i = 0; i < F.size(); i++) {
		buf_edge = Pack(F[i], buf_size, vector_size);
		MPI_Pack(buf_edge, buf_size, MPI_PACKED, buf, vector_buf_size, &position, MPI_COMM_WORLD); //packed?
	}

	return buf;
}

vector<edge> Unpack_vector(char buf[], int buf_size, int vector_size, int position) {
	edge e;
	vector<edge> F;
	int count, shift;
	MPI_Unpack(buf, sizeof(int), &position, &count, 1, MPI_INT, MPI_COMM_WORLD); // size IN buf is not write
	for (int i = 0; i < count; i++) {
		shift = position + i*buf_size;
		e = Unpack(buf, buf_size, vector_size, shift); // buf don't reduce, return another buf_size?
		F.push_back(e);
	}

	return F;
}

char* Pack_data(str H, int str_buf_size, int buf_size, int vector_size)
{
	int position = 0, vector_buf_size;
	char *buf = new char[str_buf_size], *buf_vector;
	for (int i = 0; i < H.G.size(); i++) for (int j = 0; j <= i; j++) H.G[i].erase(H.G[i].begin());

	for (int i = 0; i < H.G.size() - 1; i++) {
		vector_buf_size = H.G[i].size()*buf_size + sizeof(int);
		buf_vector = Pack_vector(H.G[i], buf_size, vector_size);
		MPI_Pack(buf_vector, vector_buf_size, MPI_PACKED, buf, str_buf_size, &position, MPI_COMM_WORLD); //packed?
	}
	vector_buf_size = H.F.size()*buf_size + sizeof(int);
	buf_vector = Pack_vector(H.F, buf_size, vector_size);
	MPI_Pack(buf_vector, vector_buf_size, MPI_PACKED, buf, str_buf_size, &position, MPI_COMM_WORLD); //packed?
	MPI_Pack(&H.ex, 1, MPI_BYTE, buf, str_buf_size, &position, MPI_COMM_WORLD);
	if (!H.ex) {
		bool *V = new bool[H.G.size()];
		for (int i = 0; i < H.G.size(); i++) V[i] = H.V[i];
		MPI_Pack(V, H.G.size(), MPI_BYTE, buf, str_buf_size, &position, MPI_COMM_WORLD);
	}

	return buf;
}


str Unpack_data(char* buf, int buf_size, int vector_size)
{
	str H;
	edge N;
	N.ex = false; // 204 is true
	H.G.push_back(Unpack_vector(buf, buf_size, vector_size, 0)); // vector_buf_size is enough for OUT buf, buf don't reduce
	int position = H.G[0].size()*buf_size + sizeof(int);
	H.G[0].insert(H.G[0].begin(), N); // get matrix size
	H.G.resize(H.G[0].size()); // assume is squre

	for (int i = 1; i < H.G.size() - 1; i++) {
		H.G[i] = Unpack_vector(buf, buf_size, vector_size, position);
		position += H.G[i].size()*buf_size + sizeof(int);
	}
	H.F = Unpack_vector(buf, buf_size, vector_size, position);
	position += H.F.size()*buf_size + sizeof(int);
	MPI_Unpack(buf, sizeof(bool), &position, &H.ex, 1, MPI_BYTE, MPI_COMM_WORLD); // postion is moving
	if (!H.ex) {
		bool *V = new bool[H.G.size()];
		MPI_Unpack(buf, H.G.size() * sizeof(bool), &position, V, H.G.size(), MPI_BYTE, MPI_COMM_WORLD);
		H.V.resize(H.G.size());
		for (int i = 0; i < H.G.size(); i++) H.V[i] = V[i];
	}

	for (int i = 1; i < H.G.size(); i++) for (int j = 0; j <= i; j++) H.G[i].insert(H.G[i].begin(), N);
	for (int i = 1; i < H.G.size(); i++) for (int j = 0; j < i; j++) H.G[i][j] = H.G[j][i];

	return H;
}