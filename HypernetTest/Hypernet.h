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
    H(H &H) {
        _FN = H._FN;
        _F = H._F;
    }

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

    void PrintHyperNet();
    static bool EqualEdgeNodes (const std::vector<int>& F, const int& firstNode, const int& secondNode);
    bool IsSNconnected();
    bool hasReliablePath();
    int GetBranchSaturation (const Branche& branche) const;
    void RemoveBranch(const Branche& branche);
    void RemoveNode(const int& node);
    void MakeReliableBranch(const Branche& branche);
    void RemoveEmptyBranches();
};
