#include "stdafx.h"
#include "funcs.h"
#include "globals.h"

// Chain replacement by edge
void ChainReduction(H& H)
{
    auto chain = H.GetChain();

    if (!chain.empty()) {
        chr++;
        Branche newBranch = Branche::GetUnity();
        for (int j = 0; j < chain.size() - 1; j++) {
            newBranch = newBranch * H.GetFN()[chain[j]][chain[j + 1]];
        }
        /*преобразования FN при замене цепи ребром*/
        for (int j = 1; j < chain.size() - 1; j++) {
            H.RemoveNode(chain[j]);
            // After removing the node, the numbering in the adjacency matrix changes
            for (int k = 0; k < chain.size(); k++) {
                if (chain[j] < chain[k]) {
                    chain[k]--;
                }
            }
        }
        int x = chain.front(), y = chain.back();
        // Arise a parallel reduction and complicated edges
        if (H.GetFN()[x][y].IsExisting()) {
            H.GetFN()[x][y] = H.GetFN()[x][y] + newBranch - H.GetFN()[x][y] * newBranch;
            H.GetFN()[y][x] = H.GetFN()[x][y];
        } else {
            H.GetFN()[x][y] = newBranch;
            H.GetFN()[y][x] = newBranch;
        }
        /*преобразования F при замене цепи ребром*/
        std::vector<int> newEdge;
        newEdge.push_back(x);
        newEdge.push_back(y);
        H.GetF().push_back(newEdge);

        return ChainReduction(H);
    }
}

// Removes the connectivity component, where both pivot nodes do not lie and returns true,
//if the pivot nodes in different components, then returns false
bool BridgeReduction(H& H)
{
    auto FN = H.GetFN();
    std::vector<bool> visitedNodes(FN.size(), false);

    H::DFS(0, visitedNodes, FN);

    int nodesInComponent = 0;
    for (auto item : visitedNodes) {
        if (item) {
            nodesInComponent++;
        }
    }

    if (nodesInComponent != FN.size()) {
        bridges++;
        if (visitedNodes.size() != FN.size()) {
            throw "BridgeReduction: sizes not match";
        }

        if (visitedNodes[0] && visitedNodes[1]) {
            for (int i = FN.size() - 1; i >= 0; i--) {
                if (!visitedNodes[i]) {
                    H.RemoveNode(i);
                }
            }
        }

        if (!visitedNodes[0] && !visitedNodes[1]) {
            for (int i = FN.size() - 1; i >= 0; i--) {
                if (visitedNodes[i]) {
                    H.RemoveNode(i);
                }
            }
        }

        return (visitedNodes[0] && !visitedNodes[1]) || (!visitedNodes[0] && visitedNodes[1]);
    } else {
        return false;
    }
}

void PenduntReduction(H& H, Branche& pseudoBranch, std::vector<int>& forbiddenNodes)
{
    auto nodePowers = H.GetNodePowers();
    auto FN = H.GetFN();

    int penduntNode = FN.size();
    for (int i = 0; i < nodePowers.size(); i++) {
        bool isForbidden = std::find(forbiddenNodes.begin(), forbiddenNodes.end(), i) != forbiddenNodes.end();
        if (nodePowers[i] == 1 && i != 0 && i != 1 && !isForbidden) {
            penduntNode = i;
        }
    }

    if (nodePowers[penduntNode] == 1 && FN.size() > 2) {
        pendunt++;
        auto HwithRemovedNode = H;
        HwithRemovedNode.RemoveNode(penduntNode);
        if (HwithRemovedNode.IsSNconnected()) {
            H.RemoveNode(penduntNode);
        } else {
            forbiddenNodes.push_back(penduntNode);
        }

        PenduntReduction(H, pseudoBranch, forbiddenNodes);
    }
}

// Obtain a allowing Branche of minimal degree, from the end of the numbering
Branche GetAllowingBranch(H& H)
{
    Branche allowingBranch;
    auto FN = H.GetFN();
    int maxSaturation = 0;
    for (int i = 0; i < FN.size(); i++) {
        for (int j = 0; j < FN[i].size(); j++) {
            if (i < j && FN[i][j].IsExisting() && !FN[i][j].GetIsReliable()) {
                int saturation = H.GetBranchSaturation(FN[i][j], i, j);
                if (maxSaturation < saturation) {
                    allowingBranch = FN[i][j];
                    allowingBranch.SetFirstNode(i);
                    allowingBranch.SetSecondNode(j);
                    maxSaturation = saturation;
                }
            }
        }
    }

    return allowingBranch;
}

Branche SimpleCase (const std::vector<std::vector<Branche>>& FN, const Branche& pseudoBranch)
{
    if (FN.size() == 2) {
        num2++;
        Branche branch = FN[0][1];
        if (branch.GetIsReliable()) {
            return pseudoBranch;
        } else {
            return pseudoBranch * FN[0][1];
        }
    }
}

Branche PairwiseConnectivity(H& H, Branche& pseudoBranch) {
    factors++;

    if (BridgeReduction(H)) {
        return Branche::GetZero();
    }
    if (H.GetFN().size() < MAX_DIMENSIONAL) {
        return SimpleCase(H.GetFN(), pseudoBranch);
    }

    std::vector<int> forbiddenNodes;
    PenduntReduction(H, pseudoBranch, forbiddenNodes);
    /*ChainReduction(H);*/
    if (H.GetFN().size() < MAX_DIMENSIONAL) {
        return SimpleCase(H.GetFN(), pseudoBranch);
    }

    if (BridgeReduction(H)) {
        return Branche::GetZero();
    }
    if (H.GetFN().size() < MAX_DIMENSIONAL) {
        return SimpleCase(H.GetFN(), pseudoBranch);
    }

    // неявно используются node1 и node2
    Branche allowingBranch = GetAllowingBranch(H);
    if (Branche::IsUnacceptableBranche(allowingBranch)) {
        throw "SimplePairwiseConnectivity: strange allowingBranch";
    }

    Branche pseudoBranch1, pseudoBranch2;
    if (allowingBranch.IsSimpleBranch()) {
        pseudoBranch1 = pseudoBranch;
        pseudoBranch1.SetPower(pseudoBranch1.GetPower() + 1);
        pseudoBranch2 = pseudoBranch;
        pseudoBranch2.SetPower(pseudoBranch2.GetPower() + 1);
        pseudoBranch2.GetC().insert(pseudoBranch2.GetC().begin(), 0);
        pseudoBranch2.GetC().pop_back();
    } else {
        pseudoBranch1 = pseudoBranch * allowingBranch;
        pseudoBranch2 = pseudoBranch * ~allowingBranch;
    }

    auto HwithReliableBranch = H, HwithRemovedBranch = H;
    HwithReliableBranch.MakeReliableBranch(allowingBranch.GetFirstNode(), allowingBranch.GetSecondNode());
    HwithRemovedBranch.RemoveBranch(allowingBranch.GetFirstNode(), allowingBranch.GetSecondNode());

    if (!HwithRemovedBranch.IsSNconnected()) {
        unconnected++;
        if (HwithReliableBranch.hasReliablePath()) {
            reliable++;
            return pseudoBranch1 * Branche::GetUnity();
        } else {
            return PairwiseConnectivity(HwithReliableBranch, pseudoBranch1);
        }
    } else {
        if (HwithReliableBranch.hasReliablePath()) {
            reliable++;
            return pseudoBranch1 * Branche::GetUnity() + PairwiseConnectivity(HwithRemovedBranch, pseudoBranch2);
        } else {
            return PairwiseConnectivity(HwithReliableBranch, pseudoBranch1) +
                    PairwiseConnectivity(HwithRemovedBranch, pseudoBranch2);
        }
    }
}
// debug
void GenCombinations(const H& H, const std::vector<Branche>& branchList, Branche& sum, std::vector<int>& brancheMask,
                     int& curPos){
    if (m == curPos) {
        /*int count = 0;
        for (auto &item : brancheMask) {
            if (item == 1) {
                count++;
            }
        }
        if (count == 1){
            throw "GenCombinations: what we need";
        }*/

        auto hypernet = H;
        for (int i = 0; i < brancheMask.size(); i++) {
            Branche branche = branchList[i];
            if (brancheMask[i] == 0) {
                hypernet.RemoveBranch(branche.GetFirstNode(), branche.GetSecondNode());
            } else if (brancheMask[i] == 1) {
                hypernet.MakeReliableBranch(branche.GetFirstNode(), branche.GetSecondNode());
            }
        }

        if (hypernet.IsSNconnected()) {
            reliable++;
            Branche result = Branche::GetBranch(0);
            for (auto &item : brancheMask) {
                if (item == 1) {
                    result.SetPower(result.GetPower() + 1);
                } else if (item == 0) {
                    result.SetPower(result.GetPower() + 1);
                    result.GetC().insert(result.GetC().begin(), 0);
                    result.GetC().pop_back();
                }
            }

            if (result.GetPower() != m) {
                throw "GenCombinations: strange result power";
            }
            sum = sum + result;
        } else {
            unconnected++;
        }
    }
    else{
        brancheMask[curPos] = 0;
        int increasedPos = curPos + 1;
        GenCombinations(H, branchList, sum, brancheMask, increasedPos);
        brancheMask[curPos] = 1;
        GenCombinations(H, branchList, sum, brancheMask, increasedPos);
    }
}

Branche SimplePairwiseConnectivity(H& H, Branche& pseudoBranch) {
    factors++;
    Branche allowingBranch = GetAllowingBranch(H);
    if (Branche::IsUnacceptableBranche(allowingBranch)) {
        throw "SimplePairwiseConnectivity: strange allowingBranch";
    }

    Branche pseudoBranch1, pseudoBranch2;
    if (allowingBranch.IsSimpleBranch()) {
        pseudoBranch1 = pseudoBranch;
        pseudoBranch1.SetPower(pseudoBranch1.GetPower() + 1);
        pseudoBranch2 = pseudoBranch;
        pseudoBranch2.SetPower(pseudoBranch2.GetPower() + 1);
        pseudoBranch2.GetC().insert(pseudoBranch2.GetC().begin(), 0);
        pseudoBranch2.GetC().pop_back();
    } else {
        pseudoBranch1 = pseudoBranch * allowingBranch;
        pseudoBranch2 = pseudoBranch * ~allowingBranch;
    }

    auto HwithReliableBranch = H, HwithRemovedBranch = H;
    HwithReliableBranch.MakeReliableBranch(allowingBranch.GetFirstNode(), allowingBranch.GetSecondNode());
    HwithRemovedBranch.RemoveBranch(allowingBranch.GetFirstNode(), allowingBranch.GetSecondNode());

    if (!HwithRemovedBranch.IsSNconnected()) {
        unconnected++;
        if (HwithReliableBranch.hasReliablePath()) {
            reliable++;
            return pseudoBranch1*Branche::GetUnity();
        } else {
            return SimplePairwiseConnectivity(HwithReliableBranch, pseudoBranch1);
        }
    } else {
        if (HwithReliableBranch.hasReliablePath()) {
            reliable++;
            return pseudoBranch1*Branche::GetUnity() + SimplePairwiseConnectivity(HwithRemovedBranch, pseudoBranch2);
        } else {
            return SimplePairwiseConnectivity(HwithReliableBranch, pseudoBranch1) +
                    SimplePairwiseConnectivity(HwithRemovedBranch, pseudoBranch2);
        }
    }
}
