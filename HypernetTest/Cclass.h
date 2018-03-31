#pragma once

#include "stdafx.h"
//example
class Foo
{
public:
    //constructor
    Foo(int foo):
            m_foo(foo)
    {
    }
    //set
    void set_foo(int foo)
    {
        m_foo = foo;
    }
    //get
    int get_foo() const
    {
        return m_foo;
    }
private:
    int m_foo;
};

/*int main()
{
    Foo bar(10);

    std::cout << bar.get_foo() << std::endl;

    bar.set_foo(20);

    std::cout << bar.get_foo() << std::endl;

    return 0;
}*/

class Edge
{
private:
    vector<double> _C;
    int _power;
    int _node1;
    int _node2;
    int _simple;
public:
    //constructor
    Edge(std::vector<double> C, const int& power, const int& node1, const int& node2, const int& simple) :
            _C(std::move(C)),
            _power(power),
            _node1(node1),
            _node2(node2),
            _simple(simple)
    {}
    static vector<double> GetUnity ( int n) {
        vector<double> Unity (n);
        Unity.front() = 1;
        return Unity;
    }
    void SetC(const vector<double>& C)
    {
        _C = C;
    }
    vector<double>& GetC()
    {
        return _C;
    }
    void SetPower(int power)
    {
        _power = power;
    }
    int GetPower() const
    {
        return _power;
    }
    void SetFirstNode(int node1)
    {
        _node1 = node1;
    }
    int GetFirstNode() const
    {
        return _node1;
    }
    void SetSecondNode(int node2)
    {
        _power = node2;
    }
    int GetSecondNode() const
    {
        return _node2;
    }
    void SetSimple(int simple)
    {
        _simple = simple;
    }
    int GetSimple() const
    {
        return _simple;
    }
    void IsExisting();
    void PrintEdge();
};

class H {
private:
    vector<vector<Edge>> _G;
    vector<vector<int>> _F;
public:
    void PrintHyperNet();
};

Edge operator *(Edge x, Edge y);
Edge operator +(Edge x, Edge y);
Edge operator -(Edge x, Edge y);
Edge operator ~(Edge x);
Edge operator *(int x, Edge y);
Edge operator *(Edge x, int y);