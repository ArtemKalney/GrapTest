#include "stdafx.h"
#include "Cclass.h"
#include "globals.h"

void Edge::PrintEdge() {
    if (Ex == 1) { // can be 204
        for (int i = 0; i < C.size(); i++) {
            cout << C[i] << " ";
        }
        cout << endl << "Power = " << Power << ", Simple = " << Simple << endl;
    }
    else cout << "empty Edge" << endl;
}

void Str::PrintStr() {
    for (int i = 0; i < Graph.size(); i++) {
        cout << "H.Graph[" << i << "] : " << endl;
        for (int j = 0; j < Graph[i].size(); j++) Graph[i][j].PrintEdge();
    }
    cout << "H.F : " << endl;
    for (int i = 0; i < F.size(); i++) F[i].PrintEdge();
    cout << "H.Ex = " << Ex << endl;
    cout << "H.Visited : ";
    for (int i = 0; i < Visited.size(); i++) cout << Visited[i] << " ";
    cout << endl;
}

Edge operator *(Edge x, Edge y) {
    Edge F;
    if (!x.C.empty() && !y.C.empty()) { // for edges not zero attendance
        if (x.C.size() == 1) return y;  // * on 1;
        if (y.C.size() == 1) return x;
        F.C.resize(x.C.size());
        F.Power = 0;
        F.Simple = 0;
        F.Ex = true; // need for ChainReduction

        if (x.Simple > 0 && x.Power != 1) {
            throw "Eror in Operator *";
        } // coudn't be becouse we contract them
        if (y.Simple > 0 && y.Power != 1) {
            throw "Eror in Operator *";
        }

        if (x.Simple > 0) { // paralel reduction
            x.Power += x.Simple;
            x = Bin[x.Power];
            x.C[x.Power] = 0;
        }
        if (y.Simple > 0) {
            y.Power += y.Simple;
            y = Bin[y.Power];
            y.C[y.Power] = 0;
        }

        F.Power = x.Power + y.Power;
        for (int i = 0; i < x.C.size(); i++)
            for (int j = 0; j < y.C.size(); j++) {
                if (x.C[i] != 0 && y.C[j] != 0) {
                    if (i + j >= F.C.size()) throw "Eror - Visited is out of size";
                    F.C[i + j] += x.C[i] * y.C[j];
                }
            }
    }
    return F;
}

Edge operator +(Edge x, Edge y) {
    Edge F;
    if (!x.C.empty() && !y.C.empty()) { // for edges not zero attendance
        if (x.C.size() == 1 && y.C.size() != 1) { // need for ChainReduction
            x.C.resize(y.C.size());
            x.Power = 0;
            x.Simple = 0;
        }
        if (x.C.size() != 1 && y.C.size() == 1) {
            y.C.resize(x.C.size());
            y.Power = 0;
            y.Simple = 0;
        }
        if (x.C.size() == 1 && y.C.size() == 1) {
            throw "Eror in Operator +";
        }
        F.C.resize(x.C.size());
        F.Simple = 0;
        F.Ex = true; // need for ChainReduction

        if (x.Simple > 0 && x.Power != 1) {
            throw "Eror in Operator +";
        }
        if (y.Simple > 0 && y.Power != 1) {
            throw "Eror in Operator +";
        }

        if (x.Simple > 0) { // paralel reduction
            x.Power += x.Simple;
            x = Bin[x.Power];
            x.C[x.Power] = 0;
        }
        if (y.Simple > 0) {
            y.Power += y.Simple;
            y = Bin[y.Power];
            y.C[y.Power] = 0;
        }

        if (x.Power != y.Power) {
            Edge I = Bin[abs(x.Power - y.Power)];
            if (x.Power < y.Power) x = x * I;
            if (y.Power < x.Power) y = y * I;
        }

        F.Power = x.Power;
        for (int i = 0; i<F.C.size(); i++) F.C[i] = x.C[i] + y.C[i];
    }
    if (!x.C.empty() && y.C.empty()) F = x;
    if (x.C.empty() && !y.C.empty()) F = y;
    return F;
}

Edge operator -(Edge x, Edge y) {
    Edge F;
    if (!x.C.empty() && !y.C.empty()) { // for edges not zero attendance
        if (x.C.size() == 1 && y.C.size() != 1) { // need for ChainReduction
            x.C.resize(y.C.size());
            x.Power = 0;
            x.Simple = 0;
        }
        if (x.C.size() != 1 && y.C.size() == 1) {
            y.C.resize(x.C.size());
            y.Power = 0;
            y.Simple = 0;
        }
        if (x.C.size() == 1 && y.C.size() == 1) {
            throw "Eror in Operator -";
        }
        F.C.resize(x.C.size());
        F.Simple = 0;
        F.Ex = true;

        if (x.Simple > 0 && x.Power != 1) {
            throw "Eror in Operator -";
        }
        if (y.Simple > 0 && y.Power != 1) {
            throw "Eror in Operator -";
        }

        if (x.Simple > 0) { // paralel reduction
            x.Power += x.Simple;
            x = Bin[x.Power];
            x.C[x.Power] = 0;
        }
        if (y.Simple > 0) {
            y.Power += y.Simple;
            y = Bin[y.Power];
            y.C[y.Power] = 0;
        }

        if (x.Power != y.Power) {
            Edge I = Bin[abs(x.Power - y.Power)];
            if (x.Power < y.Power) x = x * I;
            if (y.Power < x.Power) y = y * I;
        }

        F.Power = x.Power;
        for (int i = 0; i<F.C.size(); i++) F.C[i] = x.C[i] - y.C[i];
    }
    if (!x.C.empty() && y.C.empty()) F = x;
    if (x.C.empty() && !y.C.empty()) {
        F = y;
        for (int i = 0; i<F.C.size(); i++) F.C[i] = -F.C[i]; // ?
    }
    return F;
}

Edge operator ~(Edge x) {
    Edge F;
    if (!x.C.empty()) { // for edges not zero attendance
        if (x.C.size() == 1) return F;
        F.C.resize(x.C.size());
        F.Simple = 0;
        F.Ex = true;

        if (x.Simple > 0 && x.Power != 1) {
            throw "Eror in Operator ~";
        }

        if (x.Simple > 0) { // paralel reduction
            x.Power += x.Simple;
            x = Bin[x.Power];
            x.C[x.Power] = 0;
        }

        F.Power = x.Power;
        Edge I = Bin[F.Power];
        for (int i = 0; i<F.C.size(); i++) F.C[i] = I.C[i] - x.C[i];
    }
    else F.C.push_back(1);
    return F;
}

Edge operator *(int x, Edge y) {
    Edge F;
    if (!y.C.empty()) {
        F = y;
        for (int i = 0; i<F.C.size(); i++) F.C[i] = x * F.C[i];
    }
    return F;
}

Edge operator *(Edge x, int y) {
    Edge F;
    if (!x.C.empty()) { // for edges not zero attendance
        F = x;
        for (int i = 0; i<F.C.size(); i++) F.C[i] = y * F.C[i];
    }
    return F;
}