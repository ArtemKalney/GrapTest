#include "stdafx.h"
#include "Cclass.h"

char *Pack(Edge e, int bufSize, int vectorSize) {
    auto buf = new char[bufSize];
    if (e.C.empty()) e.C.resize(vectorSize); // send 0, don't send 1
    int position = 0;

    MPI_Pack(&e.C.front(), vectorSize, MPI_DOUBLE, buf, bufSize, &position, MPI_COMM_WORLD);
    MPI_Pack(&e.Power, 1, MPI_INT, buf, bufSize, &position, MPI_COMM_WORLD);
    MPI_Pack(&e.Simple, 1, MPI_INT, buf, bufSize, &position, MPI_COMM_WORLD);
    MPI_Pack(&e.Ex, 1, MPI_BYTE, buf, bufSize, &position, MPI_COMM_WORLD);

    return buf;
}

Edge Unpack(char buf[], int bufSize, int vectorSize, int position) {
    Edge e;
    e.C.resize(vectorSize); // need size

    MPI_Unpack(buf, bufSize, &position, &e.C.front(), vectorSize, MPI_DOUBLE, MPI_COMM_WORLD);
    MPI_Unpack(buf, bufSize, &position, &e.Power, 1, MPI_INT, MPI_COMM_WORLD);
    MPI_Unpack(buf, bufSize, &position, &e.Simple, 1, MPI_INT, MPI_COMM_WORLD);
    MPI_Unpack(buf, bufSize, &position, &e.Ex, 1, MPI_BYTE, MPI_COMM_WORLD);

    if (!e.Ex) {
        Edge n; // not fill anything
        n.Ex = false; // need this becouse 204 is true
        return n;
    }
    return e;
}

char *PackVector(vector<Edge> F, int bufSize, int vectorSize) {
    int vectorBufSize = F.size()*bufSize + sizeof(int), a = F.size();
    char *buf = new char[vectorBufSize], *bufEdge;
    int position = 0;
    MPI_Pack(&a, 1, MPI_INT, buf, vectorBufSize, &position, MPI_COMM_WORLD);
    for (int i = 0; i < F.size(); i++) {
        bufEdge = Pack(F[i], bufSize, vectorSize);
        MPI_Pack(bufEdge, bufSize, MPI_PACKED, buf, vectorBufSize, &position, MPI_COMM_WORLD); //packed?
    }

    return buf;
}

vector<Edge> UnpackVector(char *buf, int bufSize, int vectorSize, int position) {
    Edge e;
    vector<Edge> F;
    int count, shift;
    MPI_Unpack(buf, sizeof(int), &position, &count, 1, MPI_INT, MPI_COMM_WORLD); // size IN buf is not write
    for (int i = 0; i < count; i++) {
        shift = position + i * bufSize;
        e = Unpack(buf, bufSize, vectorSize, shift); // buf don't reduce, return another bufSize?
        F.push_back(e);
    }

    return F;
}

char* PackData(Str H, int strBufSize, int bufSize, int vectorSize) {
    int position = 0, vectorBufSize;
    char *buf = new char[strBufSize], *bufVector;
    for (int i = 0; i < H.Graph.size(); i++) for (int j = 0; j <= i; j++) H.Graph[i].erase(H.Graph[i].begin());

    for (int i = 0; i < H.Graph.size() - 1; i++) {
        vectorBufSize = H.Graph[i].size()*bufSize + sizeof(int);
        bufVector = PackVector(H.Graph[i], bufSize, vectorSize);
        MPI_Pack(bufVector, vectorBufSize, MPI_PACKED, buf, strBufSize, &position, MPI_COMM_WORLD); //packed?
    }
    vectorBufSize = H.F.size()*bufSize + sizeof(int);
    bufVector = PackVector(H.F, bufSize, vectorSize);
    MPI_Pack(bufVector, vectorBufSize, MPI_PACKED, buf, strBufSize, &position, MPI_COMM_WORLD); //packed?
    MPI_Pack(&H.Ex, 1, MPI_BYTE, buf, strBufSize, &position, MPI_COMM_WORLD);
    if (!H.Ex) {
        auto visited = new bool[H.Graph.size()];
        for (int i = 0; i < H.Graph.size(); i++) visited[i] = H.Visited[i];
        MPI_Pack(visited, H.Graph.size(), MPI_BYTE, buf, strBufSize, &position, MPI_COMM_WORLD);
    }

    return buf;
}

Str UnpackData(char *buf, int bufSize, int vectorSize) {
    Str H;
    Edge N;
    N.Ex = false; // 204 is true
    H.Graph.push_back(UnpackVector(buf, bufSize, vectorSize, 0)); // vector_buf_size is enough for OUT buf, buf don't reduce
    int position = H.Graph[0].size()*bufSize + sizeof(int);
    H.Graph[0].insert(H.Graph[0].begin(), N); // get matrix size
    H.Graph.resize(H.Graph[0].size()); // assume is squre

    for (int i = 1; i < H.Graph.size() - 1; i++) {
        H.Graph[i] = UnpackVector(buf, bufSize, vectorSize, position);
        position += H.Graph[i].size()*bufSize + sizeof(int);
    }
    H.F = UnpackVector(buf, bufSize, vectorSize, position);
    position += H.F.size()*bufSize + sizeof(int);
    MPI_Unpack(buf, sizeof(bool), &position, &H.Ex, 1, MPI_BYTE, MPI_COMM_WORLD); // postion is moving
    if (!H.Ex) {
        bool *V = new bool[H.Graph.size()];
        MPI_Unpack(buf, H.Graph.size() * sizeof(bool), &position, V, H.Graph.size(), MPI_BYTE, MPI_COMM_WORLD);
        H.Visited.resize(H.Graph.size());
        for (int i = 0; i < H.Graph.size(); i++) H.Visited[i] = V[i];
    }

    for (int i = 1; i < H.Graph.size(); i++) for (int j = 0; j <= i; j++) H.Graph[i].insert(H.Graph[i].begin(), N);
    for (int i = 1; i < H.Graph.size(); i++) for (int j = 0; j < i; j++) H.Graph[i][j] = H.Graph[j][i];

    return H;
}