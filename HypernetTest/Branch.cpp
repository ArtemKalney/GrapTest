#include "stdafx.h"
#include "Branch.h"
#include "globals.h"

bool Branche::IsExisting() {
    return !_C.empty();
}

void Branche::PrintBranche() {
    if (this->IsExisting()) {
        std::cout << "Branche" << this->_firstNode << "," << this->_secondNode << ":" << std::endl;
        for (auto &item : this->_C) {
            std::cout << item << " ";
        }
        std::cout << std::endl << "power=" << this->_power << ",simple=" << this->_simple << std::endl;
    }
    else {
        std::cout << "empty edge" << std::endl;
    }
}

Branche Branche::GetBranch (const int& n, const int& firstNode, const int& secondNode, const int& power) {
    std::vector<double> SimpleEdge(n);
    SimpleEdge.front() = 1;
    auto newBranche = Branche(SimpleEdge, power, firstNode, secondNode, 0, false);
    return newBranche;
}

Branche Branche::GetBranch (const int& n, const int& power) {
    std::vector<double> SimpleEdge(n);
    SimpleEdge.front() = 1;
    auto newBranche = Branche(SimpleEdge, power, 0, 0, 0, false);
    return newBranche;
}

Branche Branche::GetSimpleBranch (const int& n, const int& firstNode, const int& secondNode) {
    std::vector<double> SimpleEdge(n);
    SimpleEdge.front() = 1;
    auto newBranche = Branche(SimpleEdge, 1, firstNode, secondNode, 0, false);
    return newBranche;
}

Branche Branche::GetSimpleBranch(const int& n) {
    std::vector<double> SimpleEdge(n);
    SimpleEdge.front() = 1;
    auto newBranche = Branche(SimpleEdge, 1, 0, 0, 0, false);
    return newBranche;
}

Branche Branche::GetZero () {
    std::vector<double> Zero;
    auto newBranche = Branche(Zero, 0, 0, 0, 0, false);
    return newBranche;
}

Branche Branche::GetUnity () {
    std::vector<double> Unity;
    Unity.push_back(1);
    auto newBranche = Branche(Unity , 0, 0, 0, 0, false);
    return newBranche;
}

bool Branche::IsSimpleBranch() {
    return this->_power == 1 && this->_simple == 0;
}

bool Branche::EqualNodes(const Branche& firstBranche, const Branche& SecondBranche) {
    auto sameNodes = firstBranche.GetFirstNode() == SecondBranche.GetFirstNode() &&
                     firstBranche.GetSecondNode() == SecondBranche.GetSecondNode();
    auto reversedNodes = firstBranche.GetFirstNode() == SecondBranche.GetSecondNode() &&
                         firstBranche.GetSecondNode() == SecondBranche.GetFirstNode();
    return sameNodes || reversedNodes;
}

bool Branche::EqualNodes(const Branche& Branche, const int& firstNode, const int& secondNode){
    auto sameNodes = Branche.GetFirstNode() == firstNode &&
                     Branche.GetSecondNode() == secondNode;
    auto reversedNodes = Branche.GetFirstNode() == secondNode &&
                         Branche.GetSecondNode() == firstNode;
    return sameNodes || reversedNodes;
}

/* * The following rules for calculations:
 * 0 <=> edge.C.empty() = true
 * 1 <=> edge.C.size() = 1*/

/*edge operator *(edge x, edge y)
{
    edge F;
    if (!x.C.empty() && !y.C.empty()) {
        if (x.C.size() == 1) return y;
        if (y.C.size() == 1) return x;
        F.C.resize(x.C.size());
        F.power = 0;
        F.simple = 0;
        F.ex = true;

        if (x.simple > 0 && x.power != 1)
            cout << "Eror in Operator *" << endl;
        if (y.simple > 0 && y.power != 1)
            cout << "Eror in Operator *" << endl;

        // Parallel reduction for both edges x, y
        if (x.simple > 0) {
            x.power += x.simple;
            x = Bin[x.power];
            x.C[x.power] = 0;
        }

        if (y.simple > 0) {
            y.power += y.simple;
            y = Bin[y.power];
            y.C[y.power] = 0;
        }

        try {
            F.power = x.power + y.power;
            for (int i = 0; i<x.C.size(); i++)
                for (int j = 0; j<y.C.size(); j++) {
                    if (x.C[i] != 0 && y.C[j] != 0) {
                        if (i + j >= F.C.size())
                            throw "Eror - vector is out of size";
                        F.C[i + j] += x.C[i] * y.C[j];
                    }
                }
        }
        catch (char *str) {
            cout << str << endl;
            for (int i = 0; i<x.C.size(); i++)
                cout << x.C[i] << " ";
            cout << endl << "*" << endl;;
            for (int i = 0; i<y.C.size(); i++)
                cout << y.C[i] << " ";
            cout << endl;
        }
    }
    return F;
}

edge operator +(edge x, edge y)
{
    edge F;
    if (!x.C.empty() && !y.C.empty()) {
        if (x.C.size() == 1 && y.C.size() != 1) {
            x.C.resize(y.C.size());
            x.power = 0;
            x.simple = 0;
        }

        if (x.C.size() != 1 && y.C.size() == 1) {
            y.C.resize(x.C.size());
            y.power = 0;
            y.simple = 0;
        }

        if (x.C.size() == 1 && y.C.size() == 1)
            cout << "Eror in Operator +" << endl;

        F.C.resize(x.C.size());
        F.simple = 0;
        F.ex = true;

        if (x.simple > 0 && x.power != 1)
            cout << "Eror in Operator +" << endl;
        if (y.simple > 0 && y.power != 1)
            cout << "Eror in Operator +" << endl;

        // Parallel reduction for both edges x, y
        if (x.simple > 0) {
            x.power += x.simple;
            x = Bin[x.power];
            x.C[x.power] = 0;
        }

        if (y.simple > 0) {
            y.power += y.simple;
            y = Bin[y.power];
            y.C[y.power] = 0;
        }

        // Multiply by a unit of the required degree, so that the degrees x and y coincide
        if (x.power != y.power) {
            edge I = Bin[abs(x.power - y.power)];
            if (x.power < y.power) x = x*I;
            if (y.power < x.power) y = y*I;
        }

        F.power = x.power;
        for (int i = 0; i<F.C.size(); i++)
            F.C[i] = x.C[i] + y.C[i];
    }

    if (!x.C.empty() && y.C.empty()) F = x;
    if (x.C.empty() && !y.C.empty()) F = y;

    return F;
}

edge operator -(edge x, edge y)
{
    edge F;
    if (!x.C.empty() && !y.C.empty()) {
        if (x.C.size() == 1 && y.C.size() != 1) {
            x.C.resize(y.C.size());
            x.power = 0;
            x.simple = 0;
        }

        if (x.C.size() != 1 && y.C.size() == 1) {
            y.C.resize(x.C.size());
            y.power = 0;
            y.simple = 0;
        }

        if (x.C.size() == 1 && y.C.size() == 1)
            cout << "Eror in Operator -" << endl;

        F.C.resize(x.C.size());
        F.simple = 0;
        F.ex = true;

        if (x.simple > 0 && x.power != 1)
            cout << "Eror in Operator -" << endl;
        if (y.simple > 0 && y.power != 1)
            cout << "Eror in Operator -" << endl;

        // Parallel reduction for both edges x, y
        if (x.simple > 0) {
            x.power += x.simple;
            x = Bin[x.power];
            x.C[x.power] = 0;
        }

        if (y.simple > 0) {
            y.power += y.simple;
            y = Bin[y.power];
            y.C[y.power] = 0;
        }

        // Multiply by a unit of the required degree, so that the degrees x and y coincide
        if (x.power != y.power) {
            edge I = Bin[abs(x.power - y.power)];
            if (x.power < y.power) x = x*I;
            if (y.power < x.power) y = y*I;
        }

        F.power = x.power;
        for (int i = 0; i<F.C.size(); i++)
            F.C[i] = x.C[i] - y.C[i];
    }

    if (!x.C.empty() && y.C.empty()) F = x;
    if (x.C.empty() && !y.C.empty()) {
        F = y;
        for (int i = 0; i<F.C.size(); i++) F.C[i] = -F.C[i];
    }

    return F;
}

edge operator ~(edge x)
{
    edge F;
    if (!x.C.empty()) {
        if (x.C.size() == 1) return F;
        F.C.resize(x.C.size());
        F.simple = 0;
        F.ex = true;

        if (x.simple > 0 && x.power != 1)
            cout << "Eror in Operator ~" << endl;

        // Parallel reduction for the edge x
        if (x.simple > 0) {
            x.power += x.simple;
            x = Bin[x.power];
            x.C[x.power] = 0;
        }

        // Subtract from unity the same degree as x
        F.power = x.power;
        edge I = Bin[F.power];
        for (int i = 0; i<F.C.size(); i++)
            F.C[i] = I.C[i] - x.C[i];
    }
    else F.C.push_back(1);

    return F;
}

edge operator *(int x, edge y)
{
    edge F;
    if (!y.C.empty()) {
        F = y;
        for (int i = 0; i<F.C.size(); i++)
            F.C[i] = x*F.C[i];
    }

    return F;
}

edge operator *(edge x, int y)
{
    edge F;
    if (!x.C.empty()) {
        F = x;
        for (int i = 0; i<F.C.size(); i++)
            F.C[i] = y*F.C[i];
    }

    return F;
}*/
