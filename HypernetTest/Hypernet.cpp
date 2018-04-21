#include "Hypernet.h"
#include "globals.h"

std::vector<std::vector<Branche>> H::GetAdjacencyMatrix(std::vector<Branche>& BranchList) {
    std::vector<std::vector<Branche>> FN(n);
    for (auto &row : FN) {
        row.resize(FN.size(), Branche::GetZero());
    }
    for (int i = 0; i < FN.size(); i++) {
        for (int j = 0; j < FN[i].size(); j++) {
            for (auto &item : BranchList) {
                if (Branche::EqualNodes(item, i, j)) {
                    FN[i][j] = item;
                }
            }
        }
    }

    return FN;
}

/*возможно нужно придумать метод для получения списка путей соответсвующих данному ребру*/
bool H::EqualEdgeNodes(const std::vector<int>& edge, const int& firstNode, const int& secondNode) {
    bool sameNodes = edge.front() == firstNode && edge.back() == secondNode;
    bool reversedNodes = edge.front() == secondNode && edge.back() == firstNode;
    return sameNodes || reversedNodes;
}

void H::DFS(const int& node, std::vector<bool>& visitedNodes, const std::vector<std::vector<Branche>>& FN) {
    visitedNodes[node] = true;
    for (int i = 0; i < FN[node].size(); i++) {
        if (FN[node][i].IsExisting() && !visitedNodes[i]) {
            DFS(i, visitedNodes, FN);
        }
    }
}

void H::PrintHypernet() {
    std::cout << "FN:" << std::endl;
    for (int i = 0; i < this->GetFN().size(); i++) {
        for (int j = 0; j < this->GetFN()[i].size(); j++) {
            if (i < j && this->GetFN()[i][j].IsExisting()) {
                std::cout << "(" << i << "," << j << ")" << std::endl;
                this->GetFN()[i][j].PrintBranche();
            }
        }
    }
    std::cout << "F:" << std::endl;
    for (auto &row : this->_F) {
        for (auto &cell : row) {
            std::cout << cell << " ";
        }
        std::cout << std::endl;
    }
}
// one model used for Branches and Edges
std::vector<std::vector<Branche>> H::GetSN() {
    std::vector<Branche> EdgeList;
    for (auto &row : this->_F) {
        Branche newEdge = Branche::GetUnity();
        newEdge.SetFirstNode(row.front());
        newEdge.SetSecondNode(row.back());
        bool isReliableEdge = true;
        int prevCell = row.front();
        for (auto &cell : row) {
            //запрещает наличие петель
            if (!this->GetFN()[cell][prevCell].GetIsReliable() && prevCell != cell) {
                isReliableEdge = false;
                break;
            }
            prevCell = cell;
        }
        newEdge.SetIsReliable(isReliableEdge);

        bool isSimpleBranch = false;
        for (auto &item : EdgeList) {
            if (Branche::EqualNodes(item, newEdge)) {
                item.SetSimple(item.GetSimple() + 1);
                isSimpleBranch = true;
            }
        }
        if (!isSimpleBranch) {
            EdgeList.push_back(std::move(newEdge));
        }
    }

    return H::GetAdjacencyMatrix(EdgeList);
}

std::vector<Branche> H::GetBranchList() {
    std::vector<Branche> BranchList;
    auto FN = this->GetFN();
    for (int i = 0; i < FN.size(); i++) {
        for (int j = 0; j < FN[i].size(); j++) {
            Branche branche = FN[i][j];
            if (i < j && branche.IsExisting()) {
                BranchList.push_back(branche);
            }
        }
    }

    return BranchList;
}

bool H::IsSNconnected() {
    auto SN = this->GetSN();
    std::vector<bool> visitedNodes(SN.size(), false);

    H::DFS(0, visitedNodes, SN);

    return visitedNodes[0] && visitedNodes[1];
}
// need nodes from adjancy matrix case we don't update branch nodes
int H::GetBranchSaturation(const Branche& branche, const int& firstNode, const int& secondNode) {
    int count = 0;
    for (auto &row : this->_F) {
        auto prevCell = row.front();
        for (auto &cell : row) {
            if (Branche::EqualNodes(firstNode, secondNode, cell, prevCell)) {
                count++;
            }
            prevCell = cell;
        }
    }

    return count;
}

void H::RemoveEmptyBranches() {
    for (int i = 0; i < this->GetFN().size(); i++) {
        for (int j = 0; j < this->GetFN()[i].size(); j++) {
            if (i < j && this->GetFN()[i][j].IsExisting()) {
                int saturation = this->GetBranchSaturation(this->GetFN()[i][j], i, j);
                if (saturation == 0 && !this->GetFN()[i][j].GetIsReliable()) {
                    this->GetFN()[i][j].GetC().clear();
                    this->GetFN()[j][i].GetC().clear();
                }
            }
        }
    }
}

void H::RemoveBranch(const int& node1, const int& node2) {
    /*преобразования FN при удалении ветви*/
    this->GetFN()[node1][node2].GetC().clear();
    this->GetFN()[node2][node1].GetC().clear();
    /*преобразования F при удалении ветви*/
    for (int i = 0; i < _F.size(); i++) {
        auto prevItem = _F[i].front();
        for (auto &item : _F[i]) {
            if (Branche::EqualNodes(node1, node2, item, prevItem)) {
                _F.erase(_F.begin() + i);
                i--;
            }
            prevItem = item;
        }
    }
    /*возможно неудачная мысль*/
    this->RemoveEmptyBranches();
}

void H::RemoveNode(const int& node) {
    /*преобразования FN при удалении вершины*/
    this->GetFN().erase(this->GetFN().begin() + node);
    for (auto &row : this->GetFN()) {
        row.erase(row.begin() + node);
    }
    /*преобразования F при удалении вершины*/
    for (int i = 0; i < this->GetF().size(); i++) {
        bool deletedRow = false;
        for (int j = 0; j < this->GetF()[i].size(); j++) {
            // удаляемая вершина оказалась в ребре
            if (this->GetF()[i][j] == node) {
                // postfix increment operator usage
                this->GetF().erase(this->GetF().begin() + i--);
                deletedRow = true;
                break;
            }
        }
        // decrease nodes after delete
        if (!deletedRow) {
            for (auto &item : this->GetF()[i]) {
                if (item > node) {
                    item--;
                }
            }
        }
    }

    /*возможно неудачная мысль*/
    this->RemoveEmptyBranches();
}

void H::MakeReliableBranch(const int& node1, const int& node2) {
    this->GetFN()[node1][node2].SetIsReliable(true);
    this->GetFN()[node1][node2].SetIsReliable(true);
}

bool H::hasReliablePath() {
    auto HwithRemovedBranches = *this;
    auto FN = this->GetFN();
    for (int i = 0; i < FN.size(); i++) {
        for (int j = 0; j < FN[i].size(); j++) {
            Branche branche = FN[i][j];
            if (i < j && branche.IsExisting() && !branche.GetIsReliable()) {
                HwithRemovedBranches.RemoveBranch(i, j);
            }
        }
    }

    auto SN = HwithRemovedBranches.GetSN();
    std::vector<bool> visitedNodes(SN.size(), false);

    H::DFS(0, visitedNodes, SN);

    return visitedNodes[0] && visitedNodes[1];
}

std::vector<int> H::GetNodePowers() {
    std::vector<int> nodePowers(this->_FN.size());
    for (int i = 0; i < this->_FN.size(); i++) {
        for (auto &item : this->_FN[i]) {
            if (item.IsExisting()) {
                nodePowers[i]++;
            }
        }
    }

    return nodePowers;
}

void H::RenumerateNodes(const int& node1, const int& node2) {
    if (node1 == node2) {
        throw "RenumerateNodes: equal nodes";
    }
    auto firstVector = this->GetFN()[node1], secondVector = this->GetFN()[node2];
    // transforms FN with RenumerateNodes
    for (int i = 0; i < this->GetFN().size(); i++) {
        if (i != node1 && i != node2) {
            std::swap(firstVector[i], secondVector[i]);
        }
    }
    for (int i = 0; i < this->GetFN().size(); i++) {
        if (this->GetFN()[node2][i].IsExisting()) {
            this->GetFN()[node2][i].GetC().clear();
            this->GetFN()[i][node2].GetC().clear();
        }
        if (this->GetFN()[node1][i].IsExisting()) {
            this->GetFN()[node1][i].GetC().clear();
            this->GetFN()[i][node1].GetC().clear();
        }
    }
    for (int i = 0; i < this->GetFN().size(); i++) {
        if (firstVector[i].IsExisting() && i != node1) {
            this->GetFN()[node1][i] = firstVector[i];
            if (i != node2) {
                this->GetFN()[i][node1] = firstVector[i];
            }
        }
        if (secondVector[i].IsExisting() && i != node2) {
            this->GetFN()[node2][i] = secondVector[i];
            if (i != node1) {
                this->GetFN()[i][node2] = secondVector[i];
            }
        }
    }
    // transforms F with RenumerateNodes
    for (auto &row : this->_F) {
        for (auto &cell : row) {
            if (cell == node1) {
                cell = node2;
            } else if (cell == node2) {
                cell = node1;
            }
        }
    }
}

void H::RenumerateNodesForGen(const int& node1, const int& node2) {
    if (node1 == node2) {
        throw "RenumerateNodes: equal nodes";
    }
    this->RenumerateNodes(node1, node2);
    for (int i = 0; i < this->GetFN().size(); i++) {
        for (int j = 0; j < this->GetFN()[i].size(); j++) {
            Branche branche = this->GetFN()[i][j];
            if(i < j && branche.IsExisting()) {
                int firstNode = branche.GetFirstNode(), secondNode = branche.GetSecondNode();
                if (firstNode == node1) {
                    this->GetFN()[i][j].SetFirstNode(node2);
                } else if (firstNode == node2) {
                    this->GetFN()[i][j].SetFirstNode(node1);
                }
                if (secondNode == node1) {
                    this->GetFN()[i][j].SetSecondNode(node2);
                } else if (secondNode == node2) {
                    this->GetFN()[i][j].SetSecondNode(node1);
                }
            }
        }
    }
}

// Find a chain that does not include checked nodes
std::vector<int> H::GetChain()
{
    auto nodePowers = this->GetNodePowers();
    std::vector<int> chain;

    // Initializing a chain by an edge where there is a node of degree 2
    bool foundChainBase = false;
    int i = 0;
    while (!foundChainBase && i < this->GetFN().size()) {
        for (int j = 0; j < this->GetFN()[i].size(); j++) {
            bool pivoteNodesInChain = Branche::EqualNodes(i, j, 0, 1);
            if (this->GetFN()[i][j].IsExisting() && (nodePowers[j] == 2 || nodePowers[i] == 2) && !pivoteNodesInChain) {
                chain.push_back(i);
                chain.push_back(j);
                foundChainBase = true;
                break;
            }
        }
        i++;
    }

    // Chain extensions in both directions
    if (!chain.empty()) {
        i = chain.front();
        bool canExpandLeft = true;
        while (nodePowers[i] == 2 && i != chain.back() && canExpandLeft) {
            for (int j = 0; j < this->GetFN()[i].size(); j++) {
                if (this->GetFN()[i][j].IsExisting()) {
                    bool pivoteNodesInChain = Branche::EqualNodes(i, j, 0, 1);
                    if (pivoteNodesInChain) {
                        canExpandLeft = false;
                        break;
                    } else if (j != chain[1]) {
                        if (j != chain.back()) {
                            chain.insert(chain.begin(), j);
                        }
                        i = j;
                        break;
                    }
                }
            }
        }

        i = chain.back();
        bool canExpandRight = true;
        while (nodePowers[i] == 2 && i != chain.front() && canExpandRight) {
            for (int j = 0; j < this->GetFN()[i].size(); j++) {
                if (this->GetFN()[i][j].IsExisting()) {
                    bool pivoteNodesInChain = Branche::EqualNodes(i, j, 0, 1);
                    if (pivoteNodesInChain) {
                        canExpandRight = false;
                        break;
                    } else if (j != chain[chain.size() - 2]) {
                        if (j != chain.front()) {
                            chain.push_back(j);
                        }
                        i = j;
                        break;
                    }
                }
            }
        }

        return chain;
    }

    return chain;
}