#pragma once

#include "stdafx.h"
#include "Branch.h"

class H {
private:
    std::vector<std::vector<Branche>> _FN;
    std::vector<std::vector<int>> _F;
    std::vector<bool> _visitedNodes;
    // Search in depth, starts from the node
    void DFS(const int& node) {
        this->_visitedNodes[node] = true;
        for (int i = 0; i < this->_FN[node].size(); i++) {
            if (this->_FN[node][i].IsExisting() && !this->_visitedNodes[i]) {
                DFS(i);
            }
        }
    }
public:
    H(std::vector<std::vector<Branche>> FN, std::vector<std::vector<int>> F) :
            _FN(std::move(FN)),
            _F(std::move(F))
    {}
    //copy
    H(const H &H) :
            _FN(H._FN),
            _F(H._F)
    {}

    void SetFN(const std::vector<std::vector<Branche>> &FN)
    {
        _FN = FN;
    }

    std::vector<std::vector<Branche>>& GetFN()
    {
        return _FN;
    }

    void SetF(const std::vector<std::vector<int>> &F)
    {
        _F = F;
    }

    std::vector<std::vector<int>>& GetF()
    {
        return _F;
    }

    std::vector<bool> GetVisitedNodes()
    {
        return _visitedNodes;
    }

    static bool EqualEdgeNodes (const std::vector<int>& F, const int& firstNode, const int& secondNode);

    void PrintHypernet();
    bool IsSNconnected();
    bool IsFNconnected();
    bool hasReliablePath();
    int GetBranchSaturation (const Branche& branche, const int& firstNode, const int& secondNode);
    std::vector<int> GetChain();
    void RemoveBranch(const Branche& branche);
    void RemoveNode(const int& node);
    void MakeReliableBranch(const Branche& branche);
    void RemoveEmptyBranches();
    std::vector<int> GetNodePowers();
    void RenumerateNodes(const int& s, const int& t);
};
