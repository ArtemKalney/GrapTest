#include "Stdafx.h"
#include "Funcs.h"
#include "Globals.h"

// Chain replacement by edge
void ChainReduction(H& H) {
    std::vector<int> forbiddenNodes;
    auto chain = H.GetHomogeneousChain(forbiddenNodes);
    if (chain.empty()) {
        return;
    }
    bool isReliableChain = H.GetFN()[chain[0]][chain[1]].GetIsReliable();
    Branche parallelBranch = H.GetFN()[chain.front()][chain.back()];
    bool canReduce = parallelBranch.IsExisting() ? isReliableChain == parallelBranch.GetIsReliable() : true;
    auto SNnodepowers = H::GetNodePowers(H.GetF(), H.GetFN().size());
    bool isSimpleChain = true;
    // возможно использовать SN
    for (int i = 1; i < chain.size() - 1; i++) {
        int node = chain[i];
        if (SNnodepowers[node] != 2 && SNnodepowers[node] != 0) {
            isSimpleChain = false;
            break;
        }
    }

    if (canReduce && isSimpleChain) {
        ChainsReduced++;
        /*преобразования F при замене цепи ребром*/
        auto chainF = chain;
        for (int i = 1; i < chainF.size() - 1; i++) {
            int node = chainF[i];
            for (int j = 0; j < H.GetF().size(); j++) {
                if (H::IsSlightlyIncidental(node, H.GetF()[j])) {
                    H.GetF()[j].erase(std::remove(H.GetF()[j].begin(), H.GetF()[j].end(), node), H.GetF()[j].end());
                    // если длина ребра с тановится 1, то удаляем его
                    if (H.GetF()[i].size() == 1) {
                        H.GetF().erase(H.GetF().begin() + j--);
                    }
                } else if (H::IsIncidental(node, H.GetF()[j])) {
                    auto vector = H.GetF()[j];
                    H.GetF().erase(H.GetF().begin() + i);
                    auto it = std::find_if(H.GetF().begin(), H.GetF().end(), [node](std::vector<int> &row) ->
                            bool { return H::IsIncidental(node, row); });
                    auto vectorToInsert = *it;
                    H.GetF().erase(it);
                    if (vectorToInsert.front() == node && vector.front() == node) {
                        std::reverse(vectorToInsert.begin(), vectorToInsert.end());
                    } else if (vectorToInsert.back() == node && vector.back() == node) {
                        std::reverse(vector.begin(), vector.end());
                    } else if (vectorToInsert.front() == node && vector.back() == node) {
                        std::reverse(vectorToInsert.begin(), vectorToInsert.end());
                        std::reverse(vector.begin(), vector.end());
                    }
                    vectorToInsert.erase(std::remove(vectorToInsert.begin(), vectorToInsert.end(), node));
                    vector.erase(std::remove(vector.begin(), vector.end(), node));
                    vectorToInsert.insert(vectorToInsert.end(), vector.begin(), vector.end());
                    H.GetF().insert(H.GetF().begin(), vectorToInsert);
                }
            }
            // decrease nodes after delete
            for (auto &item : chainF) {
                if (node < item) {
                    item--;
                }
            }
            for (auto &row : H.GetF()) {
                for (auto &cell : row) {
                    if (node < cell) {
                        cell--;
                    }
                }
            }
        }
        /*преобразования FN при замене цепи ребром*/
        auto chainFN = chain;
        Branche newBranch;
        if (isReliableChain) {
            newBranch = Branche::GetSimpleBranch();
            newBranch.SetIsReliable(true);
        } else {
            newBranch = Branche::GetUnity();
            for (int i = 0; i < chainFN.size() - 1; i++) {
                newBranch = newBranch * H.GetFN()[chainFN[i]][chainFN[i + 1]];
            }
        }

        for (int i = 1; i < chainFN.size() - 1; i++) {
            H.RemoveNodeFN(chainFN[i]);
            // After removing the node, the numbering in the adjacency matrix changes
            for (int j = 0; j < chainFN.size(); j++) {
                if (chainFN[i] < chainFN[j]) {
                    chainFN[j]--;
                }
            }
        }
        int x = chainFN.front(), y = chainFN.back();
        // Arise a parallel reduction and complicated edges
        if (H.GetFN()[x][y].IsExisting() && !isReliableChain) {
            H.GetFN()[x][y] = parallelBranch + newBranch - parallelBranch * newBranch;
            H.GetFN()[y][x] = H.GetFN()[x][y];
        } else {
            H.GetFN()[x][y] = newBranch;
            H.GetFN()[y][x] = newBranch;
        }

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
        UnconnectedNodesReduced++;
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

void EdgeReduction(H& H)
{
    auto SN = H.GetSN();
    std::vector<bool> visitedNodes(SN.size(), false);
    H::DFS(0, visitedNodes, SN);
    auto canDeleteMask = H.GetCanDeleteMask(SN, visitedNodes);

    for (int i = 0; i < canDeleteMask.size(); i++) {
        for (int j = i + 1; j < canDeleteMask[i].size(); j++) {
            if (canDeleteMask[i][j]) {
                EdgesReduced++;
                H.GetF().erase(std::remove_if(H.GetF().begin(), H.GetF().end(), [i, j](std::vector<int> &row) ->
                        bool { return H::EqualEdgeNodes(row, i, j); }), H.GetF().end());
            }
        }
    }

    H.RemoveEmptyBranches();
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
        TwoNodesHypernetsCount++;
        Branche branch = FN[0][1];
        if (branch.GetIsReliable()) {
            return pseudoBranch;
        } else {
            return pseudoBranch * FN[0][1];
        }
    }
}

Branche PairConnectivity(H &H, Branche &pseudoBranch) {
    PairConnectivityCalls++;

    if (BridgeReduction(H)) {
        return Branche::GetZero();
    }
    if (H.GetFN().size() < MAX_DIMENSIONAL) {
        return SimpleCase(H.GetFN(), pseudoBranch);
    }

    EdgeReduction(H);
    if (BridgeReduction(H)) {
        return Branche::GetZero();
    }
    if (H.GetFN().size() < MAX_DIMENSIONAL) {
        return SimpleCase(H.GetFN(), pseudoBranch);
    }

   /* ChainReduction(H);
    if (BridgeReduction(H)) {
        return Branche::GetZero();
    }
    if (H.GetFN().size() < MAX_DIMENSIONAL) {
        return SimpleCase(H.GetFN(), pseudoBranch);
    }*/
    // неявно используются node1 и node2
    Branche allowingBranch = GetAllowingBranch(H);
    if (Branche::IsUnacceptableBranche(allowingBranch)) {
        throw "PairConnectivity: strange allowingBranch";
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
        UnconnectedHypernentsCount++;
        if (HwithReliableBranch.HasReliablePath()) {
            ReliableHypernetsCount++;
            return pseudoBranch1 * Branche::GetUnity();
        } else {
            return PairConnectivity(HwithReliableBranch, pseudoBranch1);
        }
    } else {
        if (HwithReliableBranch.HasReliablePath()) {
            ReliableHypernetsCount++;
            return pseudoBranch1 * Branche::GetUnity() + PairConnectivity(HwithRemovedBranch, pseudoBranch2);
        } else {
            return PairConnectivity(HwithReliableBranch, pseudoBranch1) +
                   PairConnectivity(HwithRemovedBranch, pseudoBranch2);
        }
    }
}

Branche SimplePairConnectivity(H &H, Branche &pseudoBranch) {
    PairConnectivityCalls++;
    Branche allowingBranch = GetAllowingBranch(H);
    if (Branche::IsUnacceptableBranche(allowingBranch)) {
        throw "SimplePairConnectivity: strange allowingBranch";
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
        UnconnectedHypernentsCount++;
        if (HwithReliableBranch.HasReliablePath()) {
            ReliableHypernetsCount++;
            return pseudoBranch1*Branche::GetUnity();
        } else {
            return SimplePairConnectivity(HwithReliableBranch, pseudoBranch1);
        }
    } else {
        if (HwithReliableBranch.HasReliablePath()) {
            ReliableHypernetsCount++;
            return pseudoBranch1*Branche::GetUnity() + SimplePairConnectivity(HwithRemovedBranch, pseudoBranch2);
        } else {
            return SimplePairConnectivity(HwithReliableBranch, pseudoBranch1) +
                    SimplePairConnectivity(HwithRemovedBranch, pseudoBranch2);
        }
    }
}