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

    bool isReliableChain = chain.front().GetIsReliable();
    auto nodesInChain = H.GetNodesInChain(chain);
    auto SNnodepowers = H::GetNodePowers(H.GetSN(), H.GetNodes().size());
    bool isSimpleChain = true;
    for (int i = 1; i < nodesInChain.size() - 1; i++) {
        int nodeNumber = nodesInChain[i];
        if (SNnodepowers[nodeNumber] != 2 && SNnodepowers[nodeNumber] != 0) {
            isSimpleChain = false;
            break;
        }
    }

    if (isSimpleChain) {
        ChainsReduced++;
//        преобразования F при замене цепи ребром
        auto nodesF = nodesInChain;
        for (int i = 1; i < nodesF.size() - 1; i++) {
            int nodeNumber = nodesF[i];
            for (int j = 0; j < H.GetF().size(); j++) {
                auto edgePtr = H.GetF()[j];
                if (H::IsSlightlyIncident(nodeNumber, *edgePtr)) {
                    edgePtr->erase(std::remove(edgePtr->begin(), edgePtr->end(), nodeNumber), edgePtr->end());
                    // если длина ребра с тановится 1, то удаляем его
                    if (edgePtr->size() == 1) {
                        edgePtr->clear();
                        H.GetF().erase(H.GetF().begin() + j--);
                    }
                } else if (H::IsIncident(nodeNumber, *edgePtr)) {
                    H.GetF().erase(H.GetF().begin() + j--);
                    auto it = std::find_if(H.GetF().begin(), H.GetF().end(),
                                           [nodeNumber](std::shared_ptr<std::vector<int>> &ptr) ->
                                                   bool { return H::IsIncident(nodeNumber, *ptr); });
                    auto ptrToInsert = *it;
                    if (ptrToInsert->front() == nodeNumber && edgePtr->front() == nodeNumber) {
                        std::reverse(ptrToInsert->begin(), ptrToInsert->end());
                    } else if (ptrToInsert->back() == nodeNumber && edgePtr->back() == nodeNumber) {
                        std::reverse(edgePtr->begin(), edgePtr->end());
                    } else if (ptrToInsert->front() == nodeNumber && edgePtr->back() == nodeNumber) {
                        std::reverse(ptrToInsert->begin(), ptrToInsert->end());
                        std::reverse(edgePtr->begin(), edgePtr->end());
                    }
                    // удаляем вершину из обоих рёбер
                    ptrToInsert->erase(std::remove(ptrToInsert->begin(), ptrToInsert->end(), nodeNumber));
                    edgePtr->erase(std::remove(edgePtr->begin(), edgePtr->end(), nodeNumber));
                    // объединяем рёбра
                    ptrToInsert->insert(ptrToInsert->end(), edgePtr->begin(), edgePtr->end());
                    // заменяем в FN ссылки соответсвующие edgePtr на новое ребро
                    //todo сделать проще
                    for (auto &item : H.GetFN()) {
                        for (auto &ptr : item.GetEdges()) {
                            if (ptr == edgePtr) {
                                ptr = *it;
                            }
                        }
                    }
                    // нужно для newBranch
                    for(auto &ptr : chain.front().GetEdges()) {
                        if (ptr == edgePtr) {
                            ptr = *it;
                        }
                    }
                }
            }
            // decrease indexes after delete
            for (auto &item : nodesF) {
                if (nodeNumber < item) {
                    item--;
                }
            }
            for (auto &ptr : H.GetF()) {
                for (auto &item : *ptr) {
                    if (nodeNumber < item) {
                        item--;
                    }
                }
            }
        }
//        преобразования FN при замене цепи ребром
        auto nodesFN = nodesInChain;
        Branch newBranch;
        if (isReliableChain) {
            newBranch = Branch::GetUnity();
            newBranch.SetIsReliable(true);
        } else {
            newBranch = Branch::GetUnity();
            for (auto &item : chain) {
                newBranch = newBranch * item;
            }
        }
        newBranch.SetEdges(chain.front().GetEdges());
        newBranch.SetId(FreeId++);
        for (int i = 1; i < nodesFN.size() - 1; i++) {
            int nodeNumber = nodesFN[i];
            H.RemoveNodeFN(nodeNumber);
            for (auto &item : nodesFN) {
                if (nodeNumber < item) {
                    item--;
                }
            }
        }
        newBranch.SetFirstNode(nodesFN.front());
        newBranch.SetSecondNode(nodesFN.back());
        H.GetFN().push_back(newBranch);

        return ChainReduction(H);
    } else {
        UnsimpleChains++;
        for (auto &item : nodesInChain) {
            if (std::find(forbiddenNodes.begin(), forbiddenNodes.end(), item) == forbiddenNodes.end()) {
                forbiddenNodes.push_back(item);
            }
        }

        return ChainReduction(H);
    }
}
// Removes the connectivity component, where both pivot nodes do not lie and returns true,
//if the pivot nodes in different components, then returns false
bool BridgeReduction(H& H)
{
    for(auto &item : H.GetNodes()) {
        item.IsVisited = false;
    }
    H::DFS(0, H.GetNodes(), H.GetFN());
    int visitedNodes = 0;
    for(auto &item : H.GetNodes()) {
       if (item.IsVisited) {
           visitedNodes++;
       }
    }

    if (visitedNodes != H.GetNodes().size()) {
        for (int i = 0; i < H.GetNodes().size(); i++) {
            Node node = H.GetNodes()[i];
            if (!node.IsVisited) {
                UnconnectedNodesReduced++;
                H.RemoveNode(i--);
            }
        }

        return !H.GetNodes()[1].IsVisited;
    } else {
        return false;
    }
}

void EdgeReduction(H& H) {
    auto SN = H.GetSN();
    H::DFS(0, H.GetNodes(), SN);
    auto canDeleteMask = H.GetCanDeleteMask(SN);

    for (int i = 0; i < canDeleteMask.size(); i++) {
        if (canDeleteMask[i]) {
            EdgesReduced++;
            Branch edge = SN[i];
            H.GetF().erase(
                    std::remove_if(H.GetF().begin(), H.GetF().end(),
                                   [edge](std::shared_ptr<std::vector<int>> &ptr) -> bool {
                                       bool isRemoved = H::EqualEdgeNodes(*ptr, edge);
                                       if (isRemoved) {
                                           ptr->clear();
                                       }
                                       return isRemoved;
                                   }), H.GetF().end());
        }
    }

    H.RemoveEmptyBranches();
}
// Obtain a allowing Branch of max saturation
Branch GetAllowingBranch(H& H)
{
    auto FN = H.GetFN();
//    std::sort(FN.begin(), FN.end());
    Branch allowingBranch;
    int maxSaturation = 0;
    for(auto &branch : FN) {
        if (!branch.GetIsReliable()) {
            int saturation = H::GetBranchSaturation(branch);
            if (maxSaturation < saturation) {
                allowingBranch = branch;
                maxSaturation = saturation;
            }
        }
    }

    return allowingBranch;
}

Branch SimpleCase (const std::vector<Branch>& FN, const Branch& pseudoBranch) {
    TwoNodesHypernetsCount++;
    if (FN.size() == 1) {
        return FN.front().GetIsReliable() ? pseudoBranch : pseudoBranch * FN.front();
    } else {
        Branch result = Branch::GetZero();
        for (int i = 0; i < FN.size() - 1; i++) {
            result = result + (FN[i] + FN[i + 1] - FN[i] * FN[i + 1]);
        }
        return result;
    }
}

Branch PairConnectivity(H &H, Branch &pseudoBranch) {
    PairConnectivityCalls++;
    if (BridgeReduction(H)) {
        return Branch::GetZero();
    }
    if (H.GetNodes().size() < MAX_DIMENSIONAL) {
        return SimpleCase(H.GetFN(), pseudoBranch);
    }

    EdgeReduction(H);
    if (BridgeReduction(H)) {
        return Branch::GetZero();
    }
    if (H.GetNodes().size() < MAX_DIMENSIONAL) {
        return SimpleCase(H.GetFN(), pseudoBranch);
    }

    ChainReduction(H);
    if (BridgeReduction(H)) {
        return Branch::GetZero();
    }
    if (H.GetNodes().size() < MAX_DIMENSIONAL) {
        return SimpleCase(H.GetFN(), pseudoBranch);
    }

    Branch allowingBranch = GetAllowingBranch(H);
    if (Branch::IsUnacceptableBranch(allowingBranch)) {
        throw "PairConnectivity: strange allowingBranch";
    }

    Branch pseudoBranch1, pseudoBranch2;
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

    if (!HwithRemovedBranch.IsSNconnected()) {
        UnconnectedHypernetsCount++;
        if (HwithReliableBranch.HasReliablePath()) {
            ReliableHypernetsCount++;
            return pseudoBranch1 * Branch::GetUnity();
        } else {
            return PairConnectivity(HwithReliableBranch, pseudoBranch1);
        }
    } else {
        if (HwithReliableBranch.HasReliablePath()) {
            ReliableHypernetsCount++;
            return pseudoBranch1 * Branch::GetUnity() + PairConnectivity(HwithRemovedBranch, pseudoBranch2);
        } else {
            return PairConnectivity(HwithReliableBranch, pseudoBranch1) +
                   PairConnectivity(HwithRemovedBranch, pseudoBranch2);
        }
    }
}

Branch SimplePairConnectivity(H &H, Branch &pseudoBranch) {
    PairConnectivityCalls++;
    Branch allowingBranch = GetAllowingBranch(H);
    if (Branch::IsUnacceptableBranch(allowingBranch)) {
        throw "SimplePairConnectivity: strange allowingBranch";
    }

    Branch pseudoBranch1, pseudoBranch2;
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

    if (!HwithRemovedBranch.IsSNconnected()) {
        UnconnectedHypernetsCount++;
        if (HwithReliableBranch.HasReliablePath()) {
            ReliableHypernetsCount++;
            return pseudoBranch1*Branch::GetUnity();
        } else {
            return SimplePairConnectivity(HwithReliableBranch, pseudoBranch1);
        }
    } else {
        if (HwithReliableBranch.HasReliablePath()) {
            ReliableHypernetsCount++;
            return pseudoBranch1*Branch::GetUnity() + SimplePairConnectivity(HwithRemovedBranch, pseudoBranch2);
        } else {
            return SimplePairConnectivity(HwithReliableBranch, pseudoBranch1) +
                    SimplePairConnectivity(HwithRemovedBranch, pseudoBranch2);
        }
    }
}