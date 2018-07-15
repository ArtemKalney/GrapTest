#pragma once

#include "stdafx.h"
#include "Branch.h"

class H {
private:
    std::vector<std::vector<Branche>> _FN;
    std::vector<std::vector<int>> _F;
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

    static bool EqualEdgeNodes (const std::vector<int>& edge, const int& firstNode, const int& secondNode);
    static void DFS(const int& node, std::vector<bool>& visitedNodes, const std::vector<std::vector<Branche>>& graph);
    static std::vector<std::vector<Branche>> GetAdjacencyMatrix(std::vector<Branche>& BranchList, const int& size);
    static std::vector<int> GetNodePowers(const std::vector<std::vector<Branche>>& graph);

    void PrintHypernet();
    bool IsSNconnected();
    bool HasReliablePath();
    int GetBranchSaturation (const Branche& branche, const int& firstNode, const int& secondNode);
    std::vector<int> GetChain();
    void RemoveBranch(const int& node1, const int& node2);
    void RemoveNode(const int& node);
    void MakeReliableBranch(const int& node1, const int& node2);
    void RemoveEmptyBranches();
    void RenumerateNodes(const int& node1, const int& node2);
    void RenumerateNodesForGen(const int& node1, const int& node2);
    std::vector<std::vector<Branche>> GetSN();
    std::vector<Branche> GetBranchList();
    bool CanDeletePenduntNode(const int& node);
    std::vector<std::vector<bool>> GetCanDeleteMask(const std::vector<std::vector<Branche>> &SN,
                                                    const std::vector<bool> &visitedNodes);
};
