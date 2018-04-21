#include "stdafx.h"
#include "Branch.h"
#include "globals.h"

Branche Branche::GetBranch (const int& firstNode, const int& secondNode, const int& power) {
    std::vector<double> vec(m);
    vec.front() = 1;
    auto newBranche = Branche(vec, power, firstNode, secondNode, 0, false);
    return newBranche;
}

Branche Branche::GetBranch (const int& power) {
    std::vector<double> vec(m);
    vec.front() = 1;
    auto newBranche = Branche(vec, power, 0, 0, 0, false);
    return newBranche;
}

Branche Branche::GetBranch (const int& vectorSize, const int& power) {
    std::vector<double> vec(vectorSize);
    vec.front() = 1;
    auto newBranche = Branche(vec, power, 0, 0, 0, false);
    return newBranche;
}

Branche Branche::GetSimpleBranch (const int& firstNode, const int& secondNode) {
    std::vector<double> vec(m);
    vec.front() = 1;
    auto newBranche = Branche(vec, 1, firstNode, secondNode, 0, false);
    return newBranche;
}

Branche Branche::GetSimpleBranch() {
    std::vector<double> vec(m);
    vec.front() = 1;
    auto newBranche = Branche(vec, 1, 0, 0, 0, false);
    return newBranche;
}

Branche Branche::GetZero () {
    std::vector<double> vec;
    auto newBranche = Branche(vec, 0, 0, 0, 0, false);
    return newBranche;
}

Branche Branche::GetUnity () {
    std::vector<double> vec;
    vec.push_back(1);
    auto newBranche = Branche(vec , 0, 0, 0, 0, false);
    return newBranche;
}

bool Branche::EqualNodes(const Branche& firstBranche, const Branche& secondBranche) {
    auto sameNodes = firstBranche.GetFirstNode() == secondBranche.GetFirstNode() &&
                     firstBranche.GetSecondNode() == secondBranche.GetSecondNode();
    auto reversedNodes = firstBranche.GetFirstNode() == secondBranche.GetSecondNode() &&
                         firstBranche.GetSecondNode() == secondBranche.GetFirstNode();
    return sameNodes || reversedNodes;
}

bool Branche::EqualNodes(const Branche& branche, const int& firstNode, const int& secondNode){
    auto sameNodes = branche.GetFirstNode() == firstNode &&
            branche.GetSecondNode() == secondNode;
    auto reversedNodes = branche.GetFirstNode() == secondNode &&
            branche.GetSecondNode() == firstNode;
    return sameNodes || reversedNodes;
}

bool Branche::EqualNodes(const int& firstBranchNode, const int& secondBranchNode, const int& firstNode,
                         const int& secondNode){
    auto sameNodes = firstBranchNode == firstNode &&
            secondBranchNode == secondNode;
    auto reversedNodes = firstBranchNode == secondNode &&
            secondBranchNode == firstNode;
    return sameNodes || reversedNodes;
}

void Branche::ParallelReduction(Branche& branche) {
    int newPower = branche.GetPower() + branche.GetSimple();
    branche = Bin[newPower];
    branche.GetC()[newPower] = 0;
}

bool Branche::IsUnacceptableBranche(Branche& branche) {
    int firstNode = branche.GetFirstNode(), secondNode = branche.GetSecondNode();

    bool isFirstNodeUnacceptable = firstNode < 0 || firstNode > n,
            isSecondNodeUnacceptable = secondNode < 0 || secondNode > n;
    return isFirstNodeUnacceptable || isSecondNodeUnacceptable;
}

bool Branche::IsExisting() const {
    return !_C.empty();
}

bool Branche::IsUnity() {
    return _C.size() == 1 && _C.front() == 1;
}

void Branche::PrintBranche() {
    if (this->IsExisting()) {
        std::cout << "Branche:" << std::endl;
        for (auto &item : this->_C) {
            std::cout << item << " ";
        }
        std::cout << std::endl << "power=" << this->_power << ",simple=" << this->_simple << std::endl;
    }
    else {
        std::cout << "empty edge" << std::endl;
    }
}

bool Branche::IsSimpleBranch() {
    return this->_power == 1 && this->_simple == 0;
}
/* * The following rules for calculations:
 * 0 <=> edge.C.empty() = true
 * 1 <=> edge.C.size() = 1*/
Branche operator *(Branche firstBranch, Branche secondBranch)
{
    Branche result;
    if (firstBranch.IsExisting() && secondBranch.IsExisting()) {
        if (firstBranch.IsUnity()) {
            return secondBranch;
        }
        if (secondBranch.IsUnity()) {
            return firstBranch;
        }

        if (firstBranch.GetSimple() > 0 && firstBranch.GetPower() != 1) {
            throw "Branche operator *: strange branch";
        }
        if (secondBranch.GetSimple() > 0 && secondBranch.GetPower() != 1) {
            throw "Branche operator *: strange branch";
        }

        result.GetC().resize(firstBranch.GetC().size());
        // Parallel reduction for both edges x, y
        if (firstBranch.GetSimple() > 0) {
            Branche::ParallelReduction(firstBranch);
        }
        if (secondBranch.GetSimple() > 0) {
            Branche::ParallelReduction(secondBranch);
        }

        result.SetPower(firstBranch.GetPower() + secondBranch.GetPower());
        for (int i = 0; i < firstBranch.GetC().size(); i++) {
            for (int j = 0; j < secondBranch.GetC().size(); j++) {
                if (firstBranch.GetC()[i] != 0 && secondBranch.GetC()[j] != 0) {
                    if (i + j >= result.GetC().size()) {
                        throw "Branche operator *: vector is out of size";
                    }
                    result.GetC()[i + j] += firstBranch.GetC()[i] * secondBranch.GetC()[j];
                }
            }
        }
    }

    return result;
}

Branche operator +(Branche firstBranch, Branche secondBranch)
{
    Branche result;
    if (firstBranch.IsExisting() && secondBranch.IsExisting()) {
        if (firstBranch.IsUnity() && secondBranch.IsUnity()) {
            throw "Branche operator +: unity + unity";
        }
        if (firstBranch.GetSimple() > 0 && firstBranch.GetPower() != 1) {
            throw "Branche operator +: strange branch";
        }
        if (secondBranch.GetSimple() > 0 && secondBranch.GetPower() != 1) {
            throw "Branche operator +: strange branch";
        }

        result.GetC().resize(firstBranch.GetC().size());
        // Parallel reduction for both edges x, y
        if (firstBranch.GetSimple() > 0) {
            Branche::ParallelReduction(firstBranch);
        }
        if (secondBranch.GetSimple() > 0) {
            Branche::ParallelReduction(secondBranch);
        }
        // Multiply by a unit of the required degree, so that the degrees x and y coincide
        if (firstBranch.GetPower() != secondBranch.GetPower()) {
            Branche I = Bin[abs(firstBranch.GetPower() - secondBranch.GetPower())];
            if (firstBranch.GetPower() < secondBranch.GetPower()) {
                firstBranch = firstBranch * I;
            } else {
                secondBranch = secondBranch * I;
            }
        }

        result.SetPower(firstBranch.GetPower());
        for (int i = 0; i<result.GetC().size(); i++) {
            result.GetC()[i] = firstBranch.GetC()[i] + secondBranch.GetC()[i];
        }
    } else if (firstBranch.IsExisting() && !secondBranch.IsExisting()) {
        return firstBranch;
    } else if (!firstBranch.IsExisting() && secondBranch.IsExisting()) {
        return secondBranch;
    }

    return result;
}

Branche operator -(Branche firstBranch, Branche secondBranch)
{
    Branche result;
    if (firstBranch.IsExisting() && secondBranch.IsExisting()) {
        if (firstBranch.IsUnity() && secondBranch.IsUnity()) {
            throw "Branche operator -: unity - unity";
        }
        if (firstBranch.GetSimple() > 0 && firstBranch.GetPower() != 1) {
            throw "Branche operator -: strange branch";
        }
        if (secondBranch.GetSimple() > 0 && secondBranch.GetPower() != 1) {
            throw "Branche operator -: strange branch";
        }

        result.GetC().resize(firstBranch.GetC().size());
        // Parallel reduction for both edges x, y
        if (firstBranch.GetSimple() > 0) {
            Branche::ParallelReduction(firstBranch);
        }
        if (secondBranch.GetSimple() > 0) {
            Branche::ParallelReduction(secondBranch);
        }
        // Multiply by a unit of the required degree, so that the degrees x and y coincide
        if (firstBranch.GetPower() != secondBranch.GetPower()) {
            Branche I = Bin[abs(firstBranch.GetPower() - secondBranch.GetPower())];
            if (firstBranch.GetPower() < secondBranch.GetPower()) {
                firstBranch = firstBranch * I;
            }
            if (secondBranch.GetPower() < firstBranch.GetPower()) {
                secondBranch = secondBranch * I;
            }
        }

        result.SetPower(firstBranch.GetPower());
        for (int i = 0; i<result.GetC().size(); i++) {
            result.GetC()[i] = firstBranch.GetC()[i] - secondBranch.GetC()[i];
        }
    } else if (firstBranch.IsExisting() && !secondBranch.IsExisting()) {
        return firstBranch;
    } else if (!firstBranch.IsExisting() && secondBranch.IsExisting()) {
        return secondBranch;
    }

    return result;
}

Branche operator ~(Branche branche)
{
    Branche result;
    if (branche.IsExisting()) {
        if (branche.IsUnity()) {
            return result;
        }
        result.GetC().resize(branche.GetC().size());

        if (branche.GetSimple() > 0 && branche.GetPower() != 1)
            throw "Branche operator ~: strange branch";
        // Parallel reduction for the edge x
        if (branche.GetSimple() > 0) {
            Branche::ParallelReduction(branche);
        }
        // Subtract from unity the same degree as x
        result.SetPower(branche.GetPower());
        Branche I = Bin[result.GetPower()];
        for (int i = 0; i<result.GetC().size(); i++) {
            result.GetC()[i] = I.GetC()[i] - branche.GetC()[i];
        }
    } else {
        auto unity = Branche::GetUnity();
        return unity;
    }

    return result;
}