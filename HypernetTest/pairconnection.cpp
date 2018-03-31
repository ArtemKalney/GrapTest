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
    bridges++;
    auto visitedNodes = H.GetVisitedNodes();
    if (visitedNodes.size() != H.GetFN().size()) {
        throw "BridgeReduction: sizes not match";
    }

    if (visitedNodes[0] && visitedNodes[1]) {
        for (int i = H.GetFN().size() - 1; i >= 0; i--) {
            if (!visitedNodes[i]) {
                H.RemoveNode(i);
            }
        }
    }

    if (!visitedNodes[0] && !visitedNodes[1]) {
        for (int i = H.GetFN().size() - 1; i >= 0; i--) {
            if (visitedNodes[i]) {
                H.RemoveNode(i);
            }
        }
    }

    if ((visitedNodes[0] && !visitedNodes[1]) || (!visitedNodes[0] && visitedNodes[1])) {
        return false;
    } else {
        return true;
    }
}

void PenduntReduction(H &H, Branche &pseudoBranch, int pivotNode, bool invalidCase)
{
    std::vector<int> nodePowers = H.GetNodePowers();
    // prohibit the reduction of the pivot node
    if (invalidCase) {
        nodePowers.erase(nodePowers.begin() + pivotNode);
    }

    std::vector<int>::const_iterator iteratorOfPenduntNode = std::min_element(nodePowers.begin(), nodePowers.end());
    // We get the number of the pendant node in the adjacency matrix
    int numberOfPenduntNode = iteratorOfPenduntNode - nodePowers.begin();

    if (*iteratorOfPenduntNode == 1 && H.GetFN().size() > 2) {
        pendunt++;
        // We find an incident edge of a pendant node
        for (int i = H.GetFN()[numberOfPenduntNode].size() - 1; i >= 0; i--)
            if (H.GetFN()[numberOfPenduntNode][i].IsExisting()) {
                // Invalid case
                if ((numberOfPenduntNode == 1 && i == 0) || (numberOfPenduntNode == 0 && i == 1)) {
                    pivotNode = numberOfPenduntNode;
                    invalidCase = true;
                } else {
                    if (numberOfPenduntNode != 0 && numberOfPenduntNode != 1) {
                        H.RemoveNode(numberOfPenduntNode);
                    } else {
                        if (!H.GetFN()[numberOfPenduntNode][i].GetIsReliable()) {
                            pseudoBranch = pseudoBranch * H.GetFN()[numberOfPenduntNode][i];
                        }
                        H.RemoveNode(numberOfPenduntNode);
                        // After removing the node, the numbering in the adjacency matrix changes
                        i--;
                        // The incident node is assigned with number 1
                        if (i != 1) {
                            H.RenumerateNodes(i, 1);
                        }
                    }
                    break;
                }
            }

        PenduntReduction(H, pseudoBranch, pivotNode, invalidCase);
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
                auto saturation = H.GetBranchSaturation(FN[i][j], i, j);
                if (maxSaturation < saturation) {
                    allowingBranch = FN[i][j];
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
        return pseudoBranch*FN[0][1];
    }
}

Branche PairwiseConnectivity(H& H, Branche& pseudoBranch, bool connected) {
    factors++;
    if (!connected) {
        if (!BridgeReduction(H)) {
            unconnected++;
            return Branche::GetZero();
        }
        connected = true;
    }

    if (H.GetFN().size() < MAX_DIMENSIONAL) {
        return SimpleCase(H.GetFN(), pseudoBranch);
    }

    PenduntReduction(H, pseudoBranch, 0, false);
    /*ChainReduction(H);*/

    if (H.GetFN().size() < MAX_DIMENSIONAL) {
        return SimpleCase(H.GetFN(), pseudoBranch);
    }

    Branche allowingBranch = GetAllowingBranch(H);

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
    HwithReliableBranch.MakeReliableBranch(allowingBranch);
    HwithRemovedBranch.RemoveBranch(allowingBranch);
    // case don't delete nodes and renumerate nodes then, no need to change visitedNodes
    bool isHremovedBranchFNconnected = HwithRemovedBranch.IsFNconnected();

    if (!HwithRemovedBranch.IsSNconnected()) {
        unconnected++;
        if (HwithReliableBranch.hasReliablePath()) {
            reliable++;
            return pseudoBranch1*Branche::GetUnity();
        } else {
            return PairwiseConnectivity(HwithReliableBranch, pseudoBranch1, true);
        }
    } else {
        if (HwithReliableBranch.hasReliablePath()) {
            reliable++;
            return pseudoBranch1*Branche::GetUnity() + PairwiseConnectivity(HwithRemovedBranch, pseudoBranch2,
                                                              isHremovedBranchFNconnected);
        } else {
            return PairwiseConnectivity(HwithReliableBranch, pseudoBranch1, true) +
                    PairwiseConnectivity(HwithRemovedBranch, pseudoBranch2, isHremovedBranchFNconnected);
        }
    }

    //vector<bool> visitedNodesBeforeRenumerate(visitedNodes);
    /*if (!connectedGraph) {
        int firstComponentSize = 0;

        for (int i = 0; i < visitedNodes.size(); i++)
            if (visitedNodes[i]) firstComponentSize++;
        int secondComponentSize = visitedNodes.size() - firstComponentSize;

        // Decomposition, do not consider cases where 1 node
        if (firstComponentSize != 1 && secondComponentSize != 1) {
            decomp3++;
            vector<vector<edge>> G1(graphWithDeletedEdge);
            vector<vector<edge>> G2(graphWithDeletedEdge);
            edge F3 = Bin.front();
            edge m = PairwiseConnectivity(graphWithDeletedEdge, F2, connectedGraph);

            if (allowingEdge.node2 != 0 && allowingEdge.node2 != 1 && ((visitedNodes[0] && !visitedNodes[1]) ||
                                                                       (!visitedNodes[0] && visitedNodes[1]))) {
                visitedNodes = visitedNodesBeforeRenumerate;
                if ((visitedNodes[allowingEdge.node1] && visitedNodes[0]) || (!visitedNodes[allowingEdge.node1] &&
                                                                              !visitedNodes[0]))
                    Renumerate(G1, 1, allowingEdge.node1);
                else
                    Renumerate(G1, 1, allowingEdge.node2);

                edge k = PairwiseConnectivity(G1, F3, connectedGraph);

                visitedNodes = visitedNodesBeforeRenumerate;
                if ((visitedNodes[allowingEdge.node1] && visitedNodes[1]) || (!visitedNodes[allowingEdge.node1] &&
                                                                              !visitedNodes[1]))
                    Renumerate(G2, 0, allowingEdge.node1);
                else
                    Renumerate(G2, 0, allowingEdge.node2);

                edge w = PairwiseConnectivity(G2, F3, connectedGraph);

                return F1*k*w + m;
            }

            if (allowingEdge.node2 != 0 && allowingEdge.node2 != 1 && ((visitedNodes[0] && visitedNodes[1]) ||
                                                                       (!visitedNodes[0] && !visitedNodes[1]))) {
                visitedNodes = visitedNodesBeforeRenumerate;

                edge k = PairwiseConnectivity(G1, F1, connectedGraph);

                return k + m;
            }

            if (allowingEdge.node2 == 0 || allowingEdge.node2 == 1) {
                visitedNodes = visitedNodesBeforeRenumerate;
                if (allowingEdge.node2 == 0 && ((visitedNodes[0] && !visitedNodes[1]) || (!visitedNodes[0] &&
                                                                                          visitedNodes[1])))
                    Renumerate(G1, 0, allowingEdge.node1);
                if (allowingEdge.node2 == 1 && ((visitedNodes[0] && !visitedNodes[1]) || (!visitedNodes[0] &&
                                                                                          visitedNodes[1])))
                    Renumerate(G1, 1, allowingEdge.node1);

                edge k = PairwiseConnectivity(G1, F1, connectedGraph);

                return k + m;
            }
        }
        else return PairwiseConnectivity(graphWithContractedEdge, F1, connected) +
                    PairwiseConnectivity(graphWithDeletedEdge, F2, connectedGraph);
    }*/
}