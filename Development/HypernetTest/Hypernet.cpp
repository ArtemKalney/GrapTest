#include "Hypernet.h"

bool H::IsSNconnected() {
    for(auto &item : _nodes) {
        item.IsVisited = false;
    }
    H::DFS(0, _nodes, GetSN());

    return _nodes[1].IsVisited;
}

void H::RemoveBranch(Branch& branch) {
//    преобразования FN
    _FN.erase(std::remove_if(_FN.begin(), _FN.end(), [branch](Branch &item) ->
            bool { return branch == item; }), _FN.end());
//    преобразования F
    for (auto &edgePtr : branch.GetEdges()) {
        // потому как branch из другой области видимости, где ссылки смотрят на другие вектора
        auto it = std::find_if(_F.begin(), _F.end(), [edgePtr](std::shared_ptr<std::vector<int>> &ptr) ->
                bool { return *edgePtr == *ptr; });
        if (it != _F.end()) {
            // вроде ускоряет
            _F[it - _F.begin()]->clear();
            _F.erase(it);
        }
    }

    RemoveEmptyBranches();
}

void H::RemoveNode(const int& nodeNumber) {
//    преобразования FN
    RemoveNodeFN(nodeNumber);
//    преобразования F
    RemoveNodeSN(nodeNumber);
}

void H::MakeReliableBranch(Branch& branch) {
    auto it = std::find_if(_FN.begin(), _FN.end(), [branch](Branch &item) ->
            bool { return branch == item; });
   _FN[it - _FN.begin()].SetIsReliable(true);
}

bool H::HasReliablePath() {
    auto HwithRemovedBranches = *this;
    for(auto &item : _FN) {
        if (!item.GetIsReliable()) {
            HwithRemovedBranches.RemoveBranch(item);
        }
    }
    auto SN = HwithRemovedBranches.GetSN();
    for(auto &item : _nodes) {
        item.IsVisited = false;
    }
    H::DFS(0, _nodes, SN);

    return _nodes[1].IsVisited;
}

std::vector<bool> H::GetCanDeleteMask(const std::vector<Branch> &SN) {
    std::vector<bool> edgeMask(SN.size(), false);
    std::vector<int> nodePowers = GetNodePowers(SN, _nodes.size());

    // инициализируем edgeMask рёбрами не лежащими в компоненте связности содержащей выдленные вершины, а также
    // рёбра инцидентые висячим вершинам
    for (int i = 0; i < edgeMask.size(); i++) {
        Branch edge = SN[i];
        int firstNode = edge.GetFirstNode(), secondNode = edge.GetSecondNode();
        int penduntNode = nodePowers[firstNode] == 1 ? firstNode : (nodePowers[secondNode] == 1 ? secondNode : -1);
        if (!IsPivotNode(penduntNode) && (!_nodes[firstNode].IsVisited || penduntNode != -1)) {
            edgeMask[i] = true;
        }
    }
    // расширяем маску за счёт висячих деревьев
    bool isChanged;
    do {
        isChanged = false;
        for (int i = 0; i < edgeMask.size(); i++) {
            Branch edge = SN[i];
            int firstNode = edge.GetFirstNode(), secondNode = edge.GetSecondNode();
            // расматриваем толко не выбранные ещё рёбра и в компоненте связности содержащей выделенные вершины
            if (!edgeMask[i] && _nodes[firstNode].IsVisited) {
                int unMaskedEdgesCount = 0, unMaskedEdgeInd = -1;
                if (!IsPivotNode(firstNode)) {
                    for (int j = 0; j < edgeMask.size(); j++) {
                        edge = SN[j];
                        if (!edgeMask[j] && IsIncident(firstNode, edge)) {
                            unMaskedEdgesCount++;
                            unMaskedEdgeInd = j;
                        }
                    }
                }
                if (!IsPivotNode(secondNode) && unMaskedEdgesCount != 1) {
                    unMaskedEdgesCount = 0; unMaskedEdgeInd = -1;
                    for (int j = 0; j < edgeMask.size(); j++) {
                        edge = SN[j];
                        if (!edgeMask[j] && IsIncident(secondNode, edge)) {
                            unMaskedEdgesCount++;
                            unMaskedEdgeInd = j;
                        }
                    }
                }
                if (unMaskedEdgesCount == 1) {
                    isChanged = true;
                    edgeMask[unMaskedEdgeInd] = true;
                }
            }
        }
    } while (isChanged);

    return edgeMask;
}

std::vector<Branch> H::GetHomogeneousChain(std::vector<int>& forbiddenNodes) {
    auto chain = GetChain(forbiddenNodes);
    if (!chain.empty()) {
        bool isUnreliableChain = true;
        for(auto &item : chain) {
            if (item.GetIsReliable()) {
                isUnreliableChain = false;
                break;
            }
        }
        bool isReliableChain = true;
        for(auto &item : chain) {
            if (!item.GetIsReliable()) {
                isReliableChain = false;
                break;
            }
        }


        if (isUnreliableChain || isReliableChain) {
            return chain;
        } else {
            for (auto &item : chain) {
                int firstNode = item.GetFirstNode(), secondNode = item.GetSecondNode();
                if (std::find(forbiddenNodes.begin(), forbiddenNodes.end(), firstNode) == forbiddenNodes.end()) {
                    forbiddenNodes.push_back(firstNode);
                }
                if (std::find(forbiddenNodes.begin(), forbiddenNodes.end(), secondNode) == forbiddenNodes.end()) {
                    forbiddenNodes.push_back(secondNode);
                }
            }

            return GetHomogeneousChain(forbiddenNodes);
        }
    }

    return chain;
}
// Find a chain that does not include forbiddenNodes nodes
std::vector<Branch> H::GetChain(std::vector<int>& forbiddenNodes) {
    auto nodePowers = H::GetNodePowers(_FN, _nodes.size());
    std::vector<Branch> chain;
    // Initializing a branch by an edge where there is a node of degree 2
    auto it = std::find_if(_FN.begin(), _FN.end(), [forbiddenNodes, nodePowers](Branch &item) -> bool {
        int firstNode = item.GetFirstNode(), secondNode = item.GetSecondNode();
        bool isForbiddenNodes =
                std::find(forbiddenNodes.begin(), forbiddenNodes.end(), firstNode) != forbiddenNodes.end() &&
                std::find(forbiddenNodes.begin(), forbiddenNodes.end(), secondNode) != forbiddenNodes.end();
        bool isPivoteNodeInsideChain = IsPivotNode(firstNode) && nodePowers[firstNode] == 2 ||
                IsPivotNode(secondNode) && nodePowers[secondNode] == 2;
        return !isForbiddenNodes && !isPivoteNodeInsideChain &&
               (nodePowers[firstNode] == 2 || nodePowers[secondNode] == 2);
    });
    if (it != _FN.end()) {
        chain.push_back(_FN[it - _FN.begin()]);
    }
    // Chain extensions in both directions
    if (!chain.empty()) {
        int leftNode = chain.front().GetFirstNode(), rightNode = chain.front().GetSecondNode();
        Branch leftBranch = chain.front(), rightBranch = chain.front();
        while (nodePowers[leftNode] == 2) {
            it = std::find_if(_FN.begin(), _FN.end(),
                              [nodePowers, leftNode, leftBranch, rightNode](Branch &item) -> bool {
                                  bool isLeftIncidentBranch =
                                          IsIncident(leftNode, item) && !Branch::EqualNodes(item, leftBranch);
                                  int incidentNode =
                                          item.GetFirstNode() == leftNode ? item.GetSecondNode() : item.GetFirstNode();
                                  return isLeftIncidentBranch && incidentNode != rightNode;
                              });
            if (it != _FN.end()) {
                leftBranch = _FN[it - _FN.begin()];
                int incidentNode =
                        leftBranch.GetFirstNode() == leftNode ? leftBranch.GetSecondNode() : leftBranch.GetFirstNode();
                chain.insert(chain.begin(), leftBranch);
                leftNode = incidentNode;
                if (IsPivotNode(incidentNode) && nodePowers[incidentNode] == 2) {
                    break;
                }
            } else {
                break;
            }
        }
        while (nodePowers[rightNode] == 2) {
            it = std::find_if(_FN.begin(), _FN.end(),
                              [nodePowers, rightNode, rightBranch, leftNode](Branch &item) -> bool {
                                  bool isRightIncidentBranch =
                                          IsIncident(rightNode, item) && !Branch::EqualNodes(item, rightBranch);
                                  int incidentNode =
                                          item.GetFirstNode() == rightNode ? item.GetSecondNode() : item.GetFirstNode();
                                  return isRightIncidentBranch && incidentNode != leftNode;
                              });
            if (it != _FN.end()) {
                rightBranch = _FN[it - _FN.begin()];
                int incidentNode =
                        rightBranch.GetFirstNode() == rightNode ? rightBranch.GetSecondNode() : rightBranch.GetFirstNode();
                chain.push_back(rightBranch);
                rightNode = incidentNode;
                if (IsPivotNode(incidentNode) && nodePowers[incidentNode] == 2) {
                    break;
                }
            } else {
                break;
            }
        }

        return chain;
    }

    return chain;
}
// one model used for Branches and Edges
std::vector<Branch> H::GetSN() {
    std::vector<Branch> EdgeList;
    for (auto &ptr : _F) {
        Branch newEdge = Branch::GetUnity();
        newEdge.SetFirstNode(ptr->front());
        newEdge.SetSecondNode(ptr->back());
        EdgeList.push_back(std::move(newEdge));
    }

    return EdgeList;
}

bool H::EqualEdgeNodes(const std::vector<int>& edge, const int& firstNode, const int& secondNode) {
    bool sameNodes = edge.front() == firstNode && edge.back() == secondNode;
    bool reversedNodes = edge.front() == secondNode && edge.back() == firstNode;
    return sameNodes || reversedNodes;
}

void H::DFS(const int& nodeNumber, std::vector<Node>& nodes, const std::vector<Branch>& graph) {
    std::vector<Node>::iterator it;
    if (nodeNumber != 0) {
        it = std::find_if(nodes.begin(), nodes.end(), [nodeNumber](Node &node) ->
                bool { return nodeNumber == node.NodeNumber; });
        nodes[it - nodes.begin()].IsVisited = true;
    } else {
        nodes.front().IsVisited = true;
    }
    for(auto &item : graph) {
        if (item.GetFirstNode() == nodeNumber || item.GetSecondNode() == nodeNumber) {
            int incidentNode = item.GetFirstNode();
            if (item.GetFirstNode() == nodeNumber) {
                incidentNode = item.GetSecondNode();
            }

            it = std::find_if(nodes.begin(), nodes.end(), [incidentNode](Node &node) ->
                    bool { return incidentNode == node.NodeNumber; });
            bool visitedNode = nodes[it - nodes.begin()].IsVisited;
            if (!visitedNode) {
                DFS(incidentNode, nodes, graph);
            }
        }
    }
}

std::vector<int> H::GetNodePowers(const std::vector<Branch>& graph, const int& size) {
    std::vector<int> nodePowers(size);
    for (int i = 0; i < nodePowers.size(); i++) {
        for (auto &item : graph) {
            if (IsIncident(i, item)) {
                nodePowers[i]++;
            }
        }
    }

    return nodePowers;
}

void H::RemoveEmptyBranches() {
    for (int i=0; i<_FN.size(); i++) {
        Branch branche = _FN[i];
        if (GetBranchSaturation(branche) == 0) {
            //    преобразования FN
            _FN.erase(_FN.begin() + i--);
        }
    }
}

void H::RenumerateNodes(const int& node1, const int& node2) {
    if (node1 == node2) {
        throw "RenumerateNodes: equal nodes";
    }
    // transforms FN with RenumerateNodes
    for (auto &item : _FN) {
        if (item.GetFirstNode() == node1 || item.GetFirstNode() == node2) {
            item.SetFirstNode(item.GetFirstNode() == node1 ? node2 : node1);
        }
        if (item.GetSecondNode() == node1 || item.GetSecondNode() == node2) {
            item.SetSecondNode(item.GetSecondNode() == node1 ? node2 : node1);
        }
    }
    // transforms F with RenumerateNodes
    for (auto &row : _F) {
        for (auto &cell : *row) {
            if (cell == node1) {
                cell = node2;
            } else if (cell == node2) {
                cell = node1;
            }
        }
    }
}

void H::RemoveNodeFN(const int& nodeNumber) {
    for (int i = 0; i < _FN.size(); i++) {
        Branch branche = _FN[i];
        if (IsIncident(nodeNumber, branche)) {
            _FN.erase(_FN.begin() + i--);
        } else {
            int firstNode = branche.GetFirstNode(), secondNode = branche.GetSecondNode();
            if (firstNode > nodeNumber) {
                _FN[i].SetFirstNode(--firstNode);
            }
            if (secondNode > nodeNumber) {
                _FN[i].SetSecondNode(--secondNode);
            }
        }
    }
    // пробразования nodes
    _nodes.erase(std::remove_if(_nodes.begin(), _nodes.end(), [nodeNumber](Node &node) ->
            bool { return nodeNumber == node.NodeNumber; }), _nodes.end());
    for (auto &item : _nodes) {
        if (item.NodeNumber > nodeNumber) {
            item.NodeNumber--;
        }
    }
}

void H::RemoveNodeSN(const int& nodeNumber) {
    for (int i = 0; i < _F.size(); i++) {
        bool deletedRow = false;
        for(auto &item : *_F[i]) {
//             удаляемая вершина оказалась в ребре
            if (item == nodeNumber) {
                _F.erase(_F.begin() + i--);
                deletedRow = true;
                break;
            }
        }
        // decrease nodes after delete
        if (!deletedRow) {
            for (auto &item : *_F[i]) {
                if (item > nodeNumber) {
                    item--;
                }
            }
        }
    }

    RemoveEmptyBranches();
}

bool H::IsSlightlyIncident(const int &node, const std::vector<int> &edge) {
    bool containNode = std::find(edge.begin(), edge.end(), node) != edge.end();
    return edge.front() != node && edge.back() != node && containNode;
}

bool H::IsIncident(const int &node, const std::vector<int> &edge) {
    return edge.front() == node || edge.back() == node;
}

bool H::IsIncident(const int &node, const Branch &branch) {
    return branch.GetFirstNode() == node || branch.GetSecondNode() == node;
}

bool H::IsPivotNode(const int &node) {
    return node == 0 || node == 1;
}

int H::GetBranchSaturation(Branch& branch) {
    int count = 0;
    for(auto &edgePtr : branch.GetEdges()) {
        if (!edgePtr->empty()) {
            count++;
        }
    }

    return count;
}

std::vector<Node> H::GetNodesInChain(const std::vector<Branch>& chain) {
    auto FNnodepowers = GetNodePowers(_FN, _nodes.size());
    std::vector<Node> nodesInChain;
    for (auto &item : chain) {
        for (auto &node : _nodes) {
            int nodeNumber = node.NodeNumber;
            if (H::IsIncident(nodeNumber, item)) {
                auto it = std::find_if(nodesInChain.begin(), nodesInChain.end(), [nodeNumber](Node &item) ->
                        bool { return item.NodeNumber == nodeNumber; });
                if (it == nodesInChain.end()) {
                    if (item == chain.front() || item == chain.back()) {
                        bool isTerminalNode = FNnodepowers[nodeNumber] != 2 || H::IsPivotNode(nodeNumber);
                        Node incidentNode = nodeNumber == item.GetFirstNode() ? _nodes[item.GetSecondNode()]
                                                                              : _nodes[item.GetFirstNode()];
                        if (item == chain.front()) {
                            if (isTerminalNode) {
                                nodesInChain.push_back(node);
                                nodesInChain.push_back(incidentNode);
                            } else {
                                nodesInChain.push_back(incidentNode);
                                nodesInChain.push_back(node);
                            }
                        }
                        if (item == chain.back()) {
                            if (isTerminalNode) {
                                nodesInChain.push_back(node);
                            } else {
                                nodesInChain.push_back(incidentNode);
                            }
                        }
                    } else {
                        nodesInChain.push_back(node);
                    }
                }
            }
        }
    }

    return nodesInChain;
}

void H::PrintHypernet() {
    std::cout << "FN:" << std::endl;
    for(auto &item : _FN) {
        std::cout << "(" << item.GetFirstNode() << "," << item.GetSecondNode() << ")" << std::endl;
        item.PrintBranch();
    }
    std::cout << "F:" << std::endl;
    for (auto &row : _F) {
        for (auto &cell : *row) {
            std::cout << cell << " ";
        }
        std::cout << std::endl;
    }
}
// to debug
void H::RenumerateNodesForGen(const int& node1, const int& node2) {
    RenumerateNodes(node1, node2);
    for(auto &item : _FN) {
        int firstNode = item.GetFirstNode(), secondNode = item.GetSecondNode();
        if (firstNode == node1) {
            item.SetFirstNode(node2);
        } else if (firstNode == node2) {
            item.SetFirstNode(node1);
        }
        if (secondNode == node1) {
            item.SetSecondNode(node2);
        } else if (secondNode == node2) {
            item.SetSecondNode(node1);
        }
    }
}

std::vector<std::vector<int>> H::GetEdgesF(){
    std::vector<std::vector<int>> edges;
    for(auto &edgePtr : _F) {
        if(edgePtr != nullptr) {
            edges.push_back(*edgePtr);
        }
    }
    return edges;
}

std::vector<std::vector<std::vector<int>>> H::GetEdgesFN(){
    std::vector<std::vector<std::vector<int>>> edgesByBranhes;
    for(auto &branch : _FN) {
        std::vector<std::vector<int>> edges;
        for(auto &edgePtr : branch.GetEdges()) {
            if(edgePtr != nullptr) {
                edges.push_back(*edgePtr);
            }
        }
        edgesByBranhes.push_back(edges);
    }
    return edgesByBranhes;
}