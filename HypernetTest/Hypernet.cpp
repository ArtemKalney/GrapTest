#include "Hypernet.h"
#include "globals.h"

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

/*возможно нужно придумать метод для получения списка путей соответсвующих данному ребру*/
bool H::EqualEdgeNodes(const std::vector<int>& F, const int& firstNode, const int& secondNode) {
    bool sameNodes = F.front() == firstNode && F.back() == secondNode;
    bool reversedNodes = F.front() == secondNode && F.back() == firstNode;
    return sameNodes || reversedNodes;
}

bool H::IsSNconnected() {
    for (auto &row : this->_F) {
        if (H::EqualEdgeNodes(row, 0, 1)) {
            return true;
        }
    }

    return false;
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
                    this->GetFN()[i][j].GetC().clear();
                }
            }
        }
    }
}

void H::RemoveBranch(const Branche& branche) {
    /*преобразования FN при удалении ветви*/
    int firstNode = branche.GetFirstNode(), secondNode = branche.GetSecondNode();
    /*~FN[firstNode][secondNode];
    ~FN[secondNode][firstNode];*/
    this->GetFN()[firstNode][secondNode].GetC().clear();
    this->GetFN()[secondNode][firstNode].GetC().clear();

    /*преобразования F при удалении ветви*/
    for (int i = 0; i < _F.size(); i++) {
        auto prevItem = _F[i].front();
        for (auto &item : _F[i]) {
            if (Branche::EqualNodes(branche, item, prevItem)) {
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
            if (this->GetF()[i][j] == node) {
                // if node is inside edge we delete edge, otherwise cut edge by node
                if (this->GetF()[i].front() == node || this->GetF()[i].back() == node) {
                    this->GetF()[i].erase(this->GetF()[i].begin() + j);
                } else {
                    // postfix increment operator usage
                    this->GetF().erase(this->GetF().begin() + i--);
                    deletedRow = true;
                    break;
                }
            }
            if (this->GetF()[i].size() == 1) {
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

void H::MakeReliableBranch(const Branche& branche) {
    int firstNode = branche.GetFirstNode(), secondNode = branche.GetSecondNode();
    this->GetFN()[firstNode][secondNode].SetIsReliable(true);
    this->GetFN()[secondNode][firstNode].SetIsReliable(true);
}

bool H::hasReliablePath() {
    for (auto &row : this->_F) {
        if (H::EqualEdgeNodes(row, 0, 1)) {
            auto isReliablePath = true;
            auto prevCell = row.front();
            for (auto &cell : row) {
                /*запрещает наличие петель */
                if (!this->GetFN()[cell][prevCell].GetIsReliable() && prevCell != cell) {
                    isReliablePath = false;
                    break;
                }
                prevCell = cell;
            }
            if (isReliablePath) {
                return true;
            }
        }
    }

    return false;
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

void H::RenumerateNodes(const int& s, const int& t) {
    if (s != t) {
        std::vector<Branche> firstVector = this->GetFN()[s], secondVector = this->GetFN()[t];
        for (int i = 0; i<this->GetFN().size(); i++) {
            if (i != s && i != t) {
                std::swap(firstVector[i], secondVector[i]);
            }
        }

        /*if (!visitedNodes.empty() && std::max(s, t) < visitedNodes.size()) {
            bool r = visitedNodes[s];
            visitedNodes[s] = visitedNodes[t];
            visitedNodes[t] = r;
        }*/

        for (int i = 0; i < this->GetFN().size(); i++) {
            if (this->GetFN()[t][i].IsExisting()) {
                this->GetFN()[t][i].GetC().clear();
                this->GetFN()[i][t].GetC().clear();
            }
            if (this->GetFN()[s][i].IsExisting()) {
                this->GetFN()[s][i].GetC().clear();
                this->GetFN()[i][s].GetC().clear();
            }
        }

        for (int i = 0; i < this->GetFN().size(); i++) {
            if (firstVector[i].IsExisting() && i != s) {
                this->GetFN()[s][i] = firstVector[i];
                if (i != t) {
                    this->GetFN()[i][s] = firstVector[i];
                }
            }
            if (secondVector[i].IsExisting() && i != t) {
                this->GetFN()[t][i] = secondVector[i];
                if (i != s) {
                    this->GetFN()[i][t] = secondVector[i];
                }
            }
        }
    }
}

bool H::IsFNconnected()
{
    int nodesInComponent = 0;
    this->_visitedNodes.resize(this->_FN.size());

    for (auto item : this->_visitedNodes) {
        item = false;
    }

    this->DFS(0);
    for (auto item : this->_visitedNodes) {
        if (item) {
            nodesInComponent++;
        }
    }

    return nodesInComponent == this->_FN.size();
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
                    auto expandedCahin = chain;
                    expandedCahin.push_back(j);
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
                    auto expandedCahin = chain;
                    expandedCahin.push_back(j);
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