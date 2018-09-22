#include "Stdafx.h"
#include "Funcs.h"
#include "Globals.h"

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
        if (visitedNodes.size() != FN.size()) {
            throw "BridgeReduction: sizes not match";
        }

        if (visitedNodes[0] && visitedNodes[1]) {
            for (int i = FN.size() - 1; i >= 0; i--) {
                if (!visitedNodes[i]) {
                    UnconnectedNodesReduced++;
                    H.RemoveNode(i);
                }
            }
        }

        if (!visitedNodes[0] && !visitedNodes[1]) {
            for (int i = FN.size() - 1; i >= 0; i--) {
                if (visitedNodes[i]) {
                    UnconnectedNodesReduced++;
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