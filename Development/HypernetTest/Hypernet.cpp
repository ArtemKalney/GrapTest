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

void H::RemoveNode(const int& node) {
//    преобразования FN
    RemoveNodeFN(node);
//    преобразования F
    RemoveNodeSN(node);
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

std::vector<Branch> H::GetHomogeneousChain() {
    auto nodePowers = H::GetNodePowers(_FN, _nodes.size());
    // Initializing a branch by an edge where there is a node of degree 2
    // todo учесть всё
    auto it = std::find_if(_FN.begin(), _FN.end(), [this, nodePowers](Branch &branch) -> bool {
        int firstNode = branch.GetFirstNode(), secondNode = branch.GetSecondNode();
        bool isPivotNodeInsideChain = IsPivotNode(firstNode) && nodePowers[firstNode] == 2 ||
                                      IsPivotNode(secondNode) && nodePowers[secondNode] == 2;
        if (!isPivotNodeInsideChain && (nodePowers[firstNode] == 2 || nodePowers[secondNode] == 2)) {
            bool isParallelBranch = std::find_if(_FN.begin(), _FN.end(), [branch](Branch &item) -> bool {
                return Branch::EqualNodes(item, branch) && item != branch;
            }) != _FN.end();
            if (!isParallelBranch) {
                bool isHomogeneousChain = false;
                if (nodePowers[firstNode] == 2) {
                    auto leftBranchIt = std::find_if(_FN.begin(), _FN.end(), [firstNode, branch](Branch &item) -> bool {
                        return IsIncident(firstNode, item) && item != branch;
                    });
                    isHomogeneousChain = _FN[leftBranchIt - _FN.begin()].GetIsReliable() == branch.GetIsReliable();
                }
                if (nodePowers[secondNode] == 2 && !isHomogeneousChain) {
                    auto rightBranchIt = std::find_if(_FN.begin(), _FN.end(),
                                                      [secondNode, branch](Branch &item) -> bool {
                                                          return IsIncident(secondNode, item) && item != branch;
                                                      });
                    isHomogeneousChain = _FN[rightBranchIt - _FN.begin()].GetIsReliable() == branch.GetIsReliable();
                }
                return isHomogeneousChain;
            }
        }

        return false;
    });
    std::vector<Branch> chain;
    bool isReliableChain;
    if (it != _FN.end()) {
        Branch branch = _FN[it - _FN.begin()];
        chain.push_back(branch);
        isReliableChain = branch.GetIsReliable();
    }
    // Chain extensions in both directions
    if (!chain.empty()) {
        int leftNode = chain.front().GetFirstNode(), rightNode = chain.front().GetSecondNode();
        Branch leftBranch = chain.front(), rightBranch = chain.front();
        while (nodePowers[leftNode] == 2) {
            it = std::find_if(_FN.begin(), _FN.end(),
                              [nodePowers, leftNode, leftBranch, rightNode, isReliableChain](Branch &item) -> bool {
                                  bool isHomogeneousChain = isReliableChain == item.GetIsReliable();
                                  bool isLeftIncidentBranch =
                                          IsIncident(leftNode, item) && item != leftBranch;
                                  int incidentNode =
                                          item.GetFirstNode() == leftNode ? item.GetSecondNode() : item.GetFirstNode();
                                  return isLeftIncidentBranch && incidentNode != rightNode && isHomogeneousChain;
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
                              [nodePowers, rightNode, rightBranch, leftNode, isReliableChain](Branch &item) -> bool {
                                  bool isHomogeneousChain = isReliableChain == item.GetIsReliable();
                                  bool isRightIncidentBranch =
                                          IsIncident(rightNode, item) && item != rightBranch;
                                  int incidentNode =
                                          item.GetFirstNode() == rightNode ? item.GetSecondNode() : item.GetFirstNode();
                                  return isRightIncidentBranch && incidentNode != leftNode && isHomogeneousChain;
                              });
            if (it != _FN.end()) {
                rightBranch = _FN[it - _FN.begin()];
                int incidentNode =
                        rightBranch.GetFirstNode() == rightNode ? rightBranch.GetSecondNode()
                                                                : rightBranch.GetFirstNode();
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
    std::vector<Branch> graph;
    for (auto &ptr : _F) {
        Branch newEdge = Branch::GetUnity();
        newEdge.SetFirstNode(ptr->front());
        newEdge.SetSecondNode(ptr->back());
        graph.push_back(std::move(newEdge));
    }

    return graph;
}

bool H::EqualEdgeNodes(const std::vector<int>& firstEdge, const Branch& secondEdge) {
    bool sameNodes = firstEdge.front() == secondEdge.GetFirstNode() && firstEdge.back() == secondEdge.GetSecondNode();
    bool reversedNodes =
            firstEdge.front() == secondEdge.GetSecondNode() && firstEdge.back() == secondEdge.GetFirstNode();
    return sameNodes || reversedNodes;
}

void H::DFS(const int& node, std::vector<Node>& nodes, const std::vector<Branch>& graph) {
    std::vector<Node>::iterator it;
    if (node != 0) {
        it = std::find_if(nodes.begin(), nodes.end(), [node](Node &item) ->
                bool { return item.NodeNumber == node; });
        nodes[it - nodes.begin()].IsVisited = true;
    } else {
        nodes.front().IsVisited = true;
    }
    for(auto &item : graph) {
        if (item.GetFirstNode() == node || item.GetSecondNode() == node) {
            int incidentNode = item.GetFirstNode();
            if (item.GetFirstNode() == node) {
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

void H::RenumerateNodes(const int& firstNode, const int& secondNode) {
    if (firstNode == secondNode) {
        throw "RenumerateNodes: equal nodes";
    }
    // transforms FN with RenumerateNodes
    for (auto &item : _FN) {
        if (item.GetFirstNode() == firstNode || item.GetFirstNode() == secondNode) {
            item.SetFirstNode(item.GetFirstNode() == firstNode ? secondNode : firstNode);
        }
        if (item.GetSecondNode() == firstNode || item.GetSecondNode() == secondNode) {
            item.SetSecondNode(item.GetSecondNode() == firstNode ? secondNode : firstNode);
        }
    }
    // transforms F with RenumerateNodes
    for (auto &row : _F) {
        for (auto &cell : *row) {
            if (cell == firstNode) {
                cell = secondNode;
            } else if (cell == secondNode) {
                cell = firstNode;
            }
        }
    }
}

void H::RemoveNodeFN(const int& node) {
    for (int i = 0; i < _FN.size(); i++) {
        Branch branche = _FN[i];
        if (IsIncident(node, branche)) {
            _FN.erase(_FN.begin() + i--);
        } else {
            int firstNode = branche.GetFirstNode(), secondNode = branche.GetSecondNode();
            if (firstNode > node) {
                _FN[i].SetFirstNode(--firstNode);
            }
            if (secondNode > node) {
                _FN[i].SetSecondNode(--secondNode);
            }
        }
    }
    // пробразования nodes
    _nodes.erase(std::remove_if(_nodes.begin(), _nodes.end(), [node](Node &item) ->
            bool { return item.NodeNumber == node; }), _nodes.end());
    for (auto &item : _nodes) {
        if (item.NodeNumber > node) {
            item.NodeNumber--;
        }
    }
}

void H::RemoveNodeSN(const int& node) {
    for (int i = 0; i < _F.size(); i++) {
        bool deletedRow = false;
        for(auto &item : *_F[i]) {
//             удаляемая вершина оказалась в ребре
            if (item == node) {
                _F.erase(_F.begin() + i--);
                deletedRow = true;
                break;
            }
        }
        // decrease nodes after delete
        if (!deletedRow) {
            for (auto &item : *_F[i]) {
                if (item > node) {
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
void H::RenumerateNodesForGen(const int& firstNode, const int& secondNode) {
    RenumerateNodes(firstNode, secondNode);
    for(auto &item : _FN) {
        int node1 = item.GetFirstNode(), node2 = item.GetSecondNode();
        if (node1 == firstNode) {
            item.SetFirstNode(secondNode);
        } else if (node1 == secondNode) {
            item.SetFirstNode(firstNode);
        }
        if (node2 == firstNode) {
            item.SetSecondNode(secondNode);
        } else if (node2 == secondNode) {
            item.SetSecondNode(firstNode);
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