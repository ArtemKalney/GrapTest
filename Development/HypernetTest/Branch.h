#pragma once

#include "Stdafx.h"

class Branch
{
private:
    int _id;
    std::vector<double> _C;
    std::vector<std::shared_ptr<std::vector<int>>>  _edges;
    int _power;
    int _firstNode;
    int _secondNode;
    bool _isReliable;
public:
    Branch() {}

    Branch(const int& id, std::vector<double> C, std::vector<std::shared_ptr<std::vector<int>>>& edges, const int& power,
            const int& firstNode, const int& secondNode, bool isReliable) :
            _id(id),
            _C(std::move(C)),
            _edges(std::move(edges)),
            _power(power),
            _firstNode(firstNode),
            _secondNode(secondNode),
            _isReliable(isReliable)
    {}

    Branch(const Branch& branch) :
            _id(branch._id),
            _C(branch._C),
            _edges(branch._edges),
            _power(branch._power),
            _firstNode(branch._firstNode),
            _secondNode(branch._secondNode),
            _isReliable(branch._isReliable)
    {}

    void SetId(const int& id)
    {
        _id = id;
    }

    int GetId() const
    {
        return _id;
    }

    void SetC(const std::vector<double>& C)
    {
        _C = C;
    }

    std::vector<double>& GetC()
    {
        return _C;
    }

    void SetEdges(const  std::vector<std::shared_ptr<std::vector<int>>>& edges)
    {
        _edges = edges;
    }

    std::vector<std::shared_ptr<std::vector<int>>>& GetEdges()
    {
        return _edges;
    }

    void SetPower(const int& power)
    {
        _power = power;
    }

    int GetPower() const
    {
        return _power;
    }

    void SetFirstNode(const int& firstNode)
    {
        _firstNode = firstNode;
    }

    int GetFirstNode() const
    {
        return _firstNode;
    }

    void SetSecondNode(const int& secondNode)
    {
        _secondNode = secondNode;
    }

    int GetSecondNode() const
    {
        return _secondNode;
    }

    void SetIsReliable(bool isReliable)
    {
        _isReliable = isReliable;
    }

    bool GetIsReliable() const
    {
        return _isReliable;
    }
    //todo уменьшить static функкций
    static Branch GetBranch(const int& vectorSize, const int& power);
    static Branch GetBranch(const int& power);
    static Branch GetSimpleBranch(const int& id, const int& firstNode, const int& secondNode);
    static Branch GetZero();
    static Branch GetUnity();
    static bool EqualNodes(const Branch& firstBranch, const Branch& secondBranch);
    static bool EqualNodes(const Branch& branch, const int& firstNode, const int& secondNode);
    static bool IsUnacceptableBranch(Branch &branch);
    bool IsZero() const;
    bool IsUnity();
    bool IsSimpleBranch();
    void PrintBranch();
    bool operator <(const Branch& branch) const;
};

Branch operator *(Branch firstBranch, Branch secondBranch);
Branch operator +(Branch firstBranch, Branch secondBranch);
Branch operator -(Branch firstBranch, Branch secondBranch);
Branch operator ~(Branch branch);
bool operator ==(Branch firstBranch, Branch secondBranch);
bool operator !=(Branch firstBranch, Branch secondBranch);