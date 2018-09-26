#pragma once

#include "Stdafx.h"
#include "Branch.h"

struct Node {
    int NodeNumber;
    bool IsVisited;

    Node(const int& nodeNumber, bool isVisited) :
            NodeNumber(nodeNumber),
            IsVisited(isVisited)
    {}
};

class H {
private:
    std::vector<Branch> _FN;
    std::vector<Node> _nodes;
    std::vector<std::shared_ptr<std::vector<int>>> _F;
public:
    H(std::vector<Branch> FN,  std::vector<Node> nodes, std::vector<std::shared_ptr<std::vector<int>>> F) :
            _FN(std::move(FN)),
            _nodes(std::move(nodes)),
            _F(std::move(F))
    {}
    //copy
    H(const H &H) :
            _FN(H._FN),
            _nodes(H._nodes),
            _F(H._F)
    {
        // создаём новые вектора
        for(auto &edgePtr : _F) {
            auto ptr = std::make_shared<std::vector<int>>(*edgePtr);
            edgePtr = ptr;
            for(auto &branch : _FN) {
                for(auto &item : branch.GetEdges()) {
                    if (*edgePtr == *item) {
                        item = ptr;
                    }
                }
            }
        }
    }

    void SetFN(const  std::vector<Branch> &FN)
    {
        _FN = FN;
    }

    std::vector<Branch>& GetFN()
    {
        return _FN;
    }

    void SetNodes(const  std::vector<Node> &nodes)
    {
        _nodes = nodes;
    }

    std::vector<Node>& GetNodes()
    {
        return _nodes;
    }

    void SetF(const std::vector<std::shared_ptr<std::vector<int>>> &F)
    {
        _F = F;
    }

    std::vector<std::shared_ptr<std::vector<int>>>& GetF()
    {
        return _F;
    }
    //todo уменьшить static функкций
    static bool EqualEdgeNodes(const std::vector<int>& firstEdge, const Branch& secondEdge);
    static void DFS(const int& node, std::vector<Node>& nodes, const std::vector<Branch>& graph);
    static std::vector<int> GetNodePowers(const std::vector<Branch>& graph, const int& size);
    static bool IsPivotNode(const int &node);
    static int GetBranchSaturation(Branch& branch);
    static bool IsSlightlyIncident(const int &node, const std::vector<int> &edge);
    static bool IsIncident(const int &node, const std::vector<int> &edge);
    static bool IsIncident(const int &node, const Branch &branch);
    bool IsSNconnected();
    bool HasReliablePath();
    std::vector<Branch> GetHomogeneousChain(std::vector<int>& forbiddenNodes);
    void RemoveBranch(Branch& branch);
    void RemoveNode(const int& node);
    void RemoveNodeFN(const int& node);
    void RemoveNodeSN(const int& node);
    void MakeReliableBranch(Branch& branch);
    void RemoveEmptyBranches();
    void RenumerateNodes(const int& firstNode, const int& secondNode);
    std::vector<Branch> GetSN();
    std::vector<bool> GetCanDeleteMask(const std::vector<Branch> &SN);
    std::vector<int> GetNodesInChain(const std::vector<Branch>& chain);
    void PrintHypernet();
    // to debug
    void RenumerateNodesForGen(const int& firstNode, const int& secondNode);
    std::vector<std::vector<int>> GetEdgesF();
    std::vector<std::vector<std::vector<int>>> GetEdgesFN();
};
