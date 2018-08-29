#include "Hypernet.h"
#include "Globals.h"

bool H::CanDeletePenduntNode(const int& node) {
    auto SN = GetSN();
    std::vector<bool> visitedNodes(SN.size(), false);
    H::DFS(0, visitedNodes, SN);
    auto canDeleteMask = GetCanDeleteMask(SN, visitedNodes);
    // для соизмерения матриц смежности
    auto HwithRemovedBranches = *this;
    for (int i=0; i < HwithRemovedBranches.GetFN()[node].size(); i++) {
        if (HwithRemovedBranches.GetFN()[node][i].IsExisting()) {
            HwithRemovedBranches.RemoveBranch(node, i);
            HwithRemovedBranches.RemoveBranch(i, node);
        }
    }
    auto SNwithRemovedNode = HwithRemovedBranches.GetSN();
    for (int i = 0; i < SNwithRemovedNode.size(); i++) {
        for (int j = i + 1; j < SNwithRemovedNode[i].size(); j++) {
            Branche edge = SNwithRemovedNode[i][j];
            bool isDeletedEdge = !edge.IsExisting() && SN[i][j].IsExisting();
            if (isDeletedEdge && !canDeleteMask[i][j]) {
                return false;
            }
        }
    }

    return true;
}

bool H::IsSNconnected() {
    auto SN = GetSN();
    std::vector<bool> visitedNodes(SN.size(), false);

    H::DFS(0, visitedNodes, SN);

    return visitedNodes[0] && visitedNodes[1];
}

void H::RemoveBranch(const int& node1, const int& node2) {
    /*преобразования FN при удалении ветви*/
    _FN[node1][node2].GetC().clear();
    _FN[node2][node1].GetC().clear();
    /*преобразования F при удалении ветви*/
    for (int i = 0; i < _F.size(); i++) {
        // тут была хитрая ошибка с previtem
        for (int j = 0; j < _F[i].size() - 1; j++) {
            if (Branche::EqualNodes(node1, node2, _F[i][j + 1], _F[i][j])) {
                // postfix increment operator usage
                _F.erase(_F.begin() + i--);
                break;
            }
        }
    }
    /*возможно неудачная мысль*/
    RemoveEmptyBranches();
}

void H::RemoveNode(const int& node) {
    /*преобразования FN при удалении вершины*/
    RemoveNodeFN(node);
    /*преобразования F при удалении вершины*/
    RemoveNodeSN(node);
}

void H::MakeReliableBranch(const int& node1, const int& node2) {
    _FN[node1][node2].SetIsReliable(true);
    _FN[node1][node2].SetIsReliable(true);
}

bool H::HasReliablePath() {
    auto HwithRemovedBranches = *this;
    for (int i = 0; i < _FN.size(); i++) {
        for (int j = i + 1; j < _FN[i].size(); j++) {
            Branche branche = _FN[i][j];
            if (branche.IsExisting() && !branche.GetIsReliable()) {
                HwithRemovedBranches.RemoveBranch(i, j);
            }
        }
    }

    auto SN = HwithRemovedBranches.GetSN();
    std::vector<bool> visitedNodes(SN.size(), false);

    H::DFS(0, visitedNodes, SN);
    return visitedNodes[0] && visitedNodes[1];
}

std::vector<int> H::GetHomogeneousChain(std::vector<int>& forbiddenNodes) {
    auto chain = GetChain(forbiddenNodes);
    // проверяем что цепь онадёженная, если да то возвращаем её,
    // иначе добавляем её внутренние вершины к списку запрещённых вершин и снова ищем цепь
    if (!chain.empty()) {
        bool isUnreliableChain = true;
        for(int i=0; i<chain.size() - 1; i++){
            if (_FN[i][i + 1].GetIsReliable()) {
                isUnreliableChain = false;
                break;
            }
        }
        bool isReliableChain = true;
        for(int i=0; i<chain.size() - 1; i++){
            if (!_FN[i][i + 1].GetIsReliable()) {
                isReliableChain = false;
                break;
            }
        }

        if (isUnreliableChain || isReliableChain) {
            return chain;
        } else {
            for (int i = 1; i < chain.size() - 1; i++) {
                if (std::find(forbiddenNodes.begin(), forbiddenNodes.end(), chain[i]) != forbiddenNodes.end()) {
                    forbiddenNodes.push_back(chain[i]);
                }
            }

            GetHomogeneousChain(forbiddenNodes);
        }
    }

    return chain;
}

std::vector<std::vector<bool>> H::GetCanDeleteMask(const std::vector<std::vector<Branche>> &SN,
                                                   const std::vector<bool> &visitedNodes) {
    auto nodePowers = H::GetNodePowers(SN);
    std::vector<std::vector<bool>> edgeMask(SN.size());
    for (int i = 0; i < SN.size(); i++) {
        edgeMask[i].resize(SN[i].size(), false);
    }
    // инициализируем edgeMask рёбрами не лежащими в компоненте связности содержащей выдленные вершины, а также
    // рёбра инцидентые висячим вершинам
    for (int i = 0; i < SN.size(); i++) {
        if (i != 0 && i != 1) {
            for (int j = 0; j < SN[i].size(); j++) {
                Branche edge = SN[i][j];
                if (edge.IsExisting() && (nodePowers[i] == 1 || !visitedNodes[i])) {
                    edgeMask[i][j] = true;
                    edgeMask[j][i] = true;
                    break;
                }
            }
        }
    }
    // расширяем мааску за счёт висячих деревьев
    bool isChanged;
    do {
        isChanged = false;
        for (int i = 0; i < SN.size(); i++) {
            // интересуют только рёбра в компоненте связности содержащей выделенные вершины
            if (i != 0 && i != 1 && visitedNodes[i]) {
                int unMaskedEdgesCount = 0, unMaskedEdgeInd = -1;
                for (int j = 0; j < SN[i].size(); j++) {
                    auto edge = SN[i][j];
                    if (!edgeMask[i][j] && edge.IsExisting()) {
                        unMaskedEdgesCount++;
                        unMaskedEdgeInd = j;
                    }
                }
                if (unMaskedEdgesCount == 1 && unMaskedEdgeInd > -1) {
                    isChanged = true;
                    edgeMask[i][unMaskedEdgeInd] = true;
                    edgeMask[unMaskedEdgeInd][i] = true;
                }
            }
        }
    } while (isChanged);

    return edgeMask;
}
// Find a chain that does not include forbiddenNodes nodes
std::vector<int> H::GetChain(std::vector<int>& forbiddenNodes) {
    auto nodePowers = H::GetNodePowers(_FN);
    std::vector<int> chain;
    // Initializing a chain by an edge where there is a node of degree 2
    bool foundChainBase = false;
    int i = 0;
    while (!foundChainBase && i < _FN.size()) {
        for (int j = 0; j < _FN[i].size(); j++) {
            bool isForbiddenNodes =
                    std::find(forbiddenNodes.begin(), forbiddenNodes.end(), i) != forbiddenNodes.end() &&
                    std::find(forbiddenNodes.begin(), forbiddenNodes.end(), j) != forbiddenNodes.end();
            bool isPivoteNodeInChain = IsPivoteNode(i) && nodePowers[i] == 2 || IsPivoteNode(j) && nodePowers[j] == 2;
            if (_FN[i][j].IsExisting() && (nodePowers[j] == 2 || nodePowers[i] == 2)
                && !isPivoteNodeInChain && !isForbiddenNodes) {
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
            for (int j = 0; j < _FN[i].size(); j++) {
                if (_FN[i][j].IsExisting()) {
                    if (IsPivoteNode(j) && nodePowers[j] == 2) {
                        canExpandLeft = false;
                        chain.insert(chain.begin(), j);
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
            for (int j = 0; j < _FN[i].size(); j++) {
                if (_FN[i][j].IsExisting()) {
                    if (IsPivoteNode(j) && nodePowers[j] == 2) {
                        canExpandRight = false;
                        chain.push_back(j);
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
// one model used for Branches and Edges
std::vector<std::vector<Branche>> H::GetSN() {
    std::vector<Branche> EdgeList;
    for (auto &row : _F) {
        Branche newEdge = Branche::GetUnity();
        newEdge.SetFirstNode(row.front());
        newEdge.SetSecondNode(row.back());

        bool isSimpleEdge = false;
        for (auto &item : EdgeList) {
            if (Branche::EqualNodes(item, newEdge)) {
                item.SetSimple(item.GetSimple() + 1);
                isSimpleEdge = true;
            }
        }
        if (!isSimpleEdge) {
            EdgeList.push_back(std::move(newEdge));
        }
    }

    return H::GetAdjacencyMatrix(EdgeList, _FN.size());
}

std::vector<std::vector<Branche>> H::GetAdjacencyMatrix(std::vector<Branche>& BranchList, const int& size) {
    std::vector<std::vector<Branche>> FN(size);
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
//возможно нужно придумать метод для получения списка путей соответсвующих данному ребру
bool H::EqualEdgeNodes(const std::vector<int>& edge, const int& firstNode, const int& secondNode) {
    bool sameNodes = edge.front() == firstNode && edge.back() == secondNode;
    bool reversedNodes = edge.front() == secondNode && edge.back() == firstNode;
    return sameNodes || reversedNodes;
}

void H::DFS(const int& node, std::vector<bool>& visitedNodes, const std::vector<std::vector<Branche>>& graph) {
    visitedNodes[node] = true;
    for (int i = 0; i < graph[node].size(); i++) {
        if (graph[node][i].IsExisting() && !visitedNodes[i]) {
            DFS(i, visitedNodes, graph);
        }
    }
}

std::vector<int> H::GetNodePowers(const std::vector<std::vector<Branche>>& graph) {
    std::vector<int> nodePowers(graph.size());
    for (int i = 0; i < graph.size(); i++) {
        for (auto &item : graph[i]) {
            if (item.IsExisting()) {
                nodePowers[i]++;
            }
        }
    }

    return nodePowers;
}

std::vector<int> H::GetNodePowers(const std::vector<std::vector<int>>& F, const int& size) {
    std::vector<int> nodePowers(size);
    for (int i = 0; i < size; i++) {
        for (auto &row : F) {
            if (row.front() == i || row.back() == i) {
                nodePowers[i]++;
            }
        }
    }

    return nodePowers;
}

// need nodes from adjancy matrix case we don't update branch nodes
int H::GetBranchSaturation(const Branche& branche, const int& firstNode, const int& secondNode) {
    int count = 0;
    for (auto &row : _F) {
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
    for (int i = 0; i < _FN.size(); i++) {
        for (int j = i + 1; j < _FN[i].size(); j++) {
            if (_FN[i][j].IsExisting()) {
                int saturation = GetBranchSaturation(_FN[i][j], i, j);
                if (saturation == 0) {
                    _FN[i][j].GetC().clear();
                    _FN[j][i].GetC().clear();
                }
            }
        }
    }
}

void H::RenumerateNodes(const int& node1, const int& node2) {
    if (node1 == node2) {
        throw "RenumerateNodes: equal nodes";
    }
    auto firstVector = _FN[node1], secondVector = _FN[node2];
    // transforms FN with RenumerateNodes
    for (int i = 0; i < _FN.size(); i++) {
        if (i != node1 && i != node2) {
            std::swap(firstVector[i], secondVector[i]);
        }
    }
    for (int i = 0; i < _FN.size(); i++) {
        if (_FN[node2][i].IsExisting()) {
            _FN[node2][i].GetC().clear();
            _FN[i][node2].GetC().clear();
        }
        if (_FN[node1][i].IsExisting()) {
            _FN[node1][i].GetC().clear();
            _FN[i][node1].GetC().clear();
        }
    }
    for (int i = 0; i < _FN.size(); i++) {
        if (firstVector[i].IsExisting() && i != node1) {
            _FN[node1][i] = firstVector[i];
            if (i != node2) {
                _FN[i][node1] = firstVector[i];
            }
        }
        if (secondVector[i].IsExisting() && i != node2) {
            _FN[node2][i] = secondVector[i];
            if (i != node1) {
                _FN[i][node2] = secondVector[i];
            }
        }
    }
    // transforms F with RenumerateNodes
    for (auto &row : _F) {
        for (auto &cell : row) {
            if (cell == node1) {
                cell = node2;
            } else if (cell == node2) {
                cell = node1;
            }
        }
    }
}
// для полного перебора
void H::RenumerateNodesForGen(const int& node1, const int& node2) {
    if (node1 == node2) {
        throw "RenumerateNodes: equal nodes";
    }
    RenumerateNodes(node1, node2);
    for (int i = 0; i < _FN.size(); i++) {
        for (int j = 0; j < _FN[i].size(); j++) {
            Branche branche = _FN[i][j];
            if(i < j && branche.IsExisting()) {
                int firstNode = branche.GetFirstNode(), secondNode = branche.GetSecondNode();
                if (firstNode == node1) {
                    _FN[i][j].SetFirstNode(node2);
                    _FN[j][i].SetFirstNode(node2);
                } else if (firstNode == node2) {
                    _FN[i][j].SetFirstNode(node1);
                    _FN[j][i].SetFirstNode(node1);
                }
                if (secondNode == node1) {
                    _FN[i][j].SetSecondNode(node2);
                    _FN[j][i].SetSecondNode(node2);
                } else if (secondNode == node2) {
                    _FN[i][j].SetSecondNode(node1);
                    _FN[j][i].SetSecondNode(node1);
                }
            }
        }
    }
}
// для полного перебора
std::vector<Branche> H::GetBranchList() {
    std::vector<Branche> BranchList;
    auto FN = _FN;
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

void H::RemoveNodeFN(const int& node) {
    _FN.erase(_FN.begin() + node);
    for (auto &row : _FN) {
        row.erase(row.begin() + node);
    }
}

void H::RemoveNodeSN(const int& node) {
    for (int i = 0; i < _F.size(); i++) {
        bool deletedRow = false;
        for (int j = 0; j < _F[i].size(); j++) {
            // удаляемая вершина оказалась в ребре
            if (_F[i][j] == node) {
                // postfix increment operator usage
                _F.erase(_F.begin() + i--);
                deletedRow = true;
                break;
            }
        }
        // decrease nodes after delete
        if (!deletedRow) {
            for (auto &item : _F[i]) {
                if (item > node) {
                    item--;
                }
            }
        }
    }
    /*возможно неудачная мысль*/
    RemoveEmptyBranches();
}

void H::ContractEdge(const int& deleteNode, const int& node) {
    for (auto &row : _F) {
        bool deletedRow = false;
        // если ребро инцедентно deleteNode, то делаем его инцидентым вершине node
        if (row.front() == deleteNode || row.back() == deleteNode) {
            int incidentNode = row.front() == deleteNode ? row.back() : row.front();
            if (incidentNode != node) {
                auto newEdge = row;
                if (row.front() == deleteNode) {
                    row.front() = node;
                } else if (row.back() == deleteNode) {
                    row.back() = node;
                }

                _F.push_back(newEdge);
            } else {
                deletedRow = true;
                _F.erase(std::remove(_F.begin(), _F.end(), row), _F.end());
            }
        }
        // удаляем deleteNode
        if (!deletedRow) {
            for (auto &item : row) {
                if (item > deleteNode) {
                    item--;
                } else if (item == deleteNode) {
                    row.erase(std::remove(row.begin(), row.end(), item), row.end());
                }
            }
        }
    }
    RemoveEmptyBranches();
}

bool H::IsSlightlyIncidental(const int& node, const std::vector<int>& edge) {
    bool containNode = std::find(edge.begin(), edge.end(), node) != edge.end();
    return edge.front() != node && edge.back() != node && containNode;
}

bool H::IsIncidental(const int& node, const std::vector<int>& edge) {
    return edge.front() == node || edge.back() == node;
}

int H::IsPivoteNode(const int& node) {
    return node == 0 || node == 1;
}

void H::PrintHypernet() {
    std::cout << "FN:" << std::endl;
    for (int i = 0; i < _FN.size(); i++) {
        for (int j = 0; j < _FN[i].size(); j++) {
            if (i < j && _FN[i][j].IsExisting()) {
                std::cout << "(" << i << "," << j << ")" << std::endl;
                _FN[i][j].PrintBranche();
            }
        }
    }
    std::cout << "F:" << std::endl;
    for (auto &row : _F) {
        for (auto &cell : row) {
            std::cout << cell << " ";
        }
        std::cout << std::endl;
    }
}