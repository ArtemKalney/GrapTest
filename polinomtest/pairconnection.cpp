#include "stdafx.h"
#include "funcs.h"
#include "globals.h"

// Compute the vector of degrees of nodes. The node number in the adjacency matrix is the same
// as the number in the vector
vector<int> GetNodePowers(vector<vector<edge>> &H)
{
    vector<int> nodePowers(H.size());
    int i = 0;
    while (i < H.size()) {
        for (int j = 0; j<H[i].size(); j++)
            if (H[i][j].ex) nodePowers[i]++;
        i++;
    }

    return nodePowers;
}

// Search in depth, starts from the node q
void Dfs(vector<vector<edge>> &H, int q)
{
    visitedNodes[q] = true;
    for (int i = 0; i<H[q].size(); i++)
        if (H[q][i].ex && !visitedNodes[i]) Dfs(H, i);
}

bool ConnectedGraph(vector<vector<edge>> &H)
{
    int nodesInComponent = 0;
    visitedNodes.resize(H.size());

    for (int i = 0; i<visitedNodes.size(); i++) visitedNodes[i] = false;

    Dfs(H, 0);
    for (int i = 0; i<visitedNodes.size(); i++)
        if (visitedNodes[i]) nodesInComponent++;

    return nodesInComponent == H.size();
}

void RemoveNode(vector<vector<edge>> &H, int q)
{
    H.erase(H.begin() + q);
    for (int i = 0; i<H.size(); i++)
        H[i].erase(H[i].begin() + q);
}

void RemoveEdge(vector<vector<edge>> &H, int q, int w)
{
    edge F;
    F.ex = false;
    H[q][w] = F;
    H[w][q] = F;
}

void ContractEdge(vector<vector<edge>> &H, int q, int w)
{
    for (int i = 0; i<H[q].size(); i++)
        if (H[q][i].ex && i != w) {
            // Simple edge <=> edge.power = 1
            if (H[w][i].ex && H[q][i].power == 1 && H[w][i].power == 1) {
                H[w][i].simple += H[q][i].simple + 1;
                H[i][w].simple += H[q][i].simple + 1;
            }

            if (H[w][i].ex && (H[q][i].power != 1 || H[w][i].power != 1)) {
                H[w][i] = H[q][i] + H[w][i] - H[q][i] * H[w][i];
                H[i][w] = H[w][i];
            }

            if (!H[w][i].ex) {
                H[w][i] = H[q][i];
                H[i][w] = H[q][i];
            }
        }

    RemoveNode(H, q);
}

// Renumbering of two nodes in an adjacency matrix
void Renumerate(vector<vector<edge>> &H, int s, int t)
{
    if (s != t) {
        vector<edge> F, G;
        F = H[s];
        G = H[t];
        for (int i = 0; i<H.size(); i++)
            if (i != s && i != t) swap(F[i], G[i]);

        if (!visitedNodes.empty() && max(s, t) < visitedNodes.size()) {
            bool r = visitedNodes[s];
            visitedNodes[s] = visitedNodes[t];
            visitedNodes[t] = r;
        }

        for (int i = 0; i<H.size(); i++) {
            if (H[t][i].ex) RemoveEdge(H, t, i);
            if (H[s][i].ex) RemoveEdge(H, s, i);
        }

        for (int i = 0; i<H.size(); i++) {
            if (F[i].ex && i != s) {
                H[s][i] = F[i];
                if (i != t) H[i][s] = F[i];
            }
            if (G[i].ex && i != t) {
                H[t][i] = G[i];
                if (i != s) H[i][t] = G[i];
            }
        }
    }
}

void PenduntReduction(vector<vector<edge>> &H, edge &F, int pivotNode, bool invalidCase)
{
    pendunt++;
    vector<int> nodePowers = GetNodePowers(H);
    // prohibit the reduction of the pivot node
    if (invalidCase)
        nodePowers.erase(nodePowers.begin() + pivotNode);

    vector <int>::iterator it, iteratorOfPenduntNode = nodePowers.end() - 1;
    for (it = nodePowers.begin(); it<nodePowers.end(); ++it)
        if (*it == 1) iteratorOfPenduntNode = it;
    // We get the number of the pendant node in the adjacency matrix
    int numberOfPenduntNode = iteratorOfPenduntNode - nodePowers.begin();

    if (*iteratorOfPenduntNode == 1 && H.size() > 2) {
        // We find an incident edge of a pendant node
        for (int i = H[numberOfPenduntNode].size() - 1; i >= 0; i--)
            if (H[numberOfPenduntNode][i].ex) {
                // Invalid case
                if ((numberOfPenduntNode == 1 && i == 0) || (numberOfPenduntNode == 0 && i == 1)) {
                    pivotNode = numberOfPenduntNode;
                    invalidCase = true;
                }

                if (numberOfPenduntNode != 0 && numberOfPenduntNode != 1) RemoveNode(H, numberOfPenduntNode);

                if ((numberOfPenduntNode == 1 && i != 0) || (numberOfPenduntNode == 0 && i != 1)) {
                    F = F*H[numberOfPenduntNode][i];
                    RemoveNode(H, numberOfPenduntNode);
                    // After removing the node, the numbering in the adjacency matrix changes
                    i--;
                    // The incident node is assigned with number 1
                    if (i != 1) Renumerate(H, i, 1);
                }
                break;
            }

        PenduntReduction(H, F, pivotNode, invalidCase);
    }
}

// Removes the connectivity component, where both pivot nodes do not lie and returns true,
//if the pivot nodes in different components, then returns false
bool BridgeReduction(vector<vector<edge>> &H)
{
    bridges++;
    if (visitedNodes.size() != H.size())
        cout << "Eror BridgeReduction!" << endl;

    if (visitedNodes[0] && visitedNodes[1]) {
        for (int i = H.size() - 1; i >= 0; i--)
            if (!visitedNodes[i]) RemoveNode(H, i);
    }

    if (!visitedNodes[0] && !visitedNodes[1]) {
        for (int i = H.size() - 1; i >= 0; i--)
            if (visitedNodes[i]) RemoveNode(H, i);
    }

    if ((visitedNodes[0] && !visitedNodes[1]) || (!visitedNodes[0] && visitedNodes[1]))
        return false;

    return true;
}

// Obtain a allowing edge of minimal degree, from the end of the numbering
edge GetAllowingEdge(vector<vector<edge>> &H)
{
    edge F;
    vector<int> nodePowers = GetNodePowers(H);
    vector<int>::iterator it, iteratorNodeWithSmallestPower = nodePowers.end() - 1;

    for (it = nodePowers.begin(); it<nodePowers.end(); ++it)
        if (*it <= *iteratorNodeWithSmallestPower && (it - nodePowers.begin() != 0) && (it - nodePowers.begin() != 1))
            iteratorNodeWithSmallestPower = it;
    int numberNodeWithSmallestPower = iteratorNodeWithSmallestPower - nodePowers.begin();

    for (int i = H[numberNodeWithSmallestPower].size() - 1; i >= 0; i--)
        // Find the incident edge of the node with the minimal degree
        if (H[numberNodeWithSmallestPower][i].ex) {
            F = H[numberNodeWithSmallestPower][i];
            F.node1 = numberNodeWithSmallestPower;
            F.node2 = i;
            break;
        }

    return F;
}

// If the element x lies in the vector P returns true, then false
bool ElementInside(vector<int> P, int x)
{
    if (P.empty()) return false;
    else
        for (int i = 0; i<P.size(); i++)
            if (x == P[i]) return true;

    return false;
}

// Find a chain that does not include checked nodes
vector<int> GetChain(vector<vector<edge>> &H, vector<int> &checkedNodes)
{
    vector<int> nodePowers = GetNodePowers(H);
    vector<int> chain;

    // Initializing a chain by an edge where there is a node of degree 2
    bool find = false;
    int i = 0;
    while (!find && i<H.size()) {
        for (int j = 0; j<H[i].size(); j++) {
            if (H[i][j].ex && !(ElementInside(checkedNodes, i) && ElementInside(checkedNodes, j)) &&
                    (nodePowers[j] == 2 || nodePowers[i] == 2)) {
                chain.push_back(i);
                chain.push_back(j);
                find = true;
                break;
            }
        }
        i++;
    }

    // Chain extensions in both directions
    if (!chain.empty()) {
        i = chain.front();
        while (nodePowers[i] == 2 && i != chain.back())
            for (int j = 0; j<H[i].size(); j++)
                if (H[i][j].ex && j != chain[1]) {
                    if (j != chain.back()) chain.insert(chain.begin(), j);
                    i = j;
                    break;
                }

        i = chain.back();
        while (nodePowers[i] == 2 && i != chain.front())
            for (int j = 0; j<H[i].size(); j++)
                if (H[i][j].ex && j != chain[chain.size() - 2]) {
                    if (j != chain.front()) chain.push_back(j);
                    i = j;
                    break;
                }

        return chain;
    }

    return chain;
}

// Reduces chains and returns special chain if it finds,
// the number of pivot nodes in a special chain is placed at the end of the vector
vector<int> ChainReduction(vector<vector<edge>> &H, vector<int> &specialChain, vector<int> &checkedNodes,
                           bool chainWithTwoSpecialNodes)
{
    vector<int> chain = GetChain(H, checkedNodes);
    int countPivotNodes = 0;

    if (!chain.empty()) {
        for (int i = 0; i<chain.size(); i++)
            if (chain[i] == 0 || chain[i] == 1) countPivotNodes++;

        // If the pivot node with the edge of the chain reduce it
        vector<int>::iterator it, iteratorOfPivotNode = chain.begin();
        if (countPivotNodes == 1) {
            for (it = chain.begin(); it<chain.end(); ++it)
                if (*it == 0 || *it == 1) iteratorOfPivotNode = it;

            if ((iteratorOfPivotNode == chain.begin() || iteratorOfPivotNode == chain.end() - 1) &&
                    !specialChain.empty())
                countPivotNodes = 0;
        }

        if ((countPivotNodes == 1 || countPivotNodes == 2) && !chainWithTwoSpecialNodes) {
            // Return only this chain, the rest is reduced
            if (countPivotNodes == 2) {
                chainWithTwoSpecialNodes = true;
                checkedNodes = chain;
            }

            if (countPivotNodes == 1) {
                // If we find a new chain with 1 pivot node, then we reduce the old chain,
                // where the pivot node with the edge
                if (!specialChain.empty()) {
                    for (it = specialChain.begin(); it<specialChain.end() - 1; ++it)
                        if (*it == 0 || *it == 1) iteratorOfPivotNode = it;

                    if (iteratorOfPivotNode == specialChain.begin() || iteratorOfPivotNode == specialChain.end() - 2)
                        checkedNodes.clear();
                }

                for (int j = 0; j<chain.size(); j++)
                    if (!ElementInside(checkedNodes, chain[j])) checkedNodes.push_back(chain[j]);
            }

            chain.push_back(countPivotNodes);
            specialChain = chain;

            return ChainReduction(H, specialChain, checkedNodes, chainWithTwoSpecialNodes);
        }
        else
            countPivotNodes = 0;

        // Chain replacement by edge
        if (countPivotNodes == 0) {
            chr++;
            edge newEdge;
            newEdge.C.push_back(1);

            for (int j = 0; j<chain.size() - 1; j++)
                newEdge = newEdge*H[chain[j]][chain[j + 1]];

            for (int j = 1; j<chain.size() - 1; j++) {
                RemoveNode(H, chain[j]);
                // After removing the node, the numbering in the adjacency matrix changes
                for (int k = 0; k<chain.size(); k++)
                    if (chain[j] < chain[k]) chain[k]--;
                for (int k = 0; k<specialChain.size(); k++)
                    if (chain[j] < specialChain[k]) specialChain[k]--;
                for (int k = 0; k<checkedNodes.size(); k++)
                    if (chain[j] < checkedNodes[k]) checkedNodes[k]--;
            }

            int x = chain.front(), y = chain.back();

            // Arise a parallel reduction and complicated edges
            if (H[x][y].ex) {
                H[x][y] = H[x][y] + newEdge - H[x][y] * newEdge;
                H[y][x] = H[x][y];
            }
            else {
                H[x][y] = newEdge;
                H[y][x] = newEdge;
            }

            // After reduction, it is possible to expand the special chain
            if (!specialChain.empty()) {
                specialChain.pop_back();
                vector<int> nodePowers = GetNodePowers(H);

                int i = specialChain.front();
                while (nodePowers[i] == 2 && i != specialChain.back())
                    for (int j = 0; j<H[i].size(); j++)
                        if (H[i][j].ex && j != specialChain[1]) {
                            if (j != specialChain.back()) {
                                specialChain.insert(specialChain.begin(), j);
                                if (!ElementInside(checkedNodes, j)) checkedNodes.push_back(j);
                            }
                            i = j;
                            break;
                        }

                i = specialChain.back();
                while (nodePowers[i] == 2 && i != specialChain.front())
                    for (int j = 0; j<H[i].size(); j++)
                        if (H[i][j].ex && j != specialChain[specialChain.size() - 2]) {
                            if (j != specialChain.front()) {
                                specialChain.push_back(j);
                                if (!ElementInside(checkedNodes, j)) checkedNodes.push_back(j);
                            }
                            i = j;
                            break;
                        }

                countPivotNodes = 0;
                for (int i = 0; i<specialChain.size(); i++)
                    if (specialChain[i] == 0 || specialChain[i] == 1) countPivotNodes++;

                specialChain.push_back(countPivotNodes);
            }

            return ChainReduction(H, specialChain, checkedNodes, chainWithTwoSpecialNodes);
        }
    }

    return specialChain;
}

edge SimpleCase (const vector<vector<edge>> &H, const edge &F)
{
    if (H.size() == 2) {
        num2++;
        return F*H[0][1];
    }

    if (H.size() == 3) {
        num3++;
        return F*(H[0][1] + H[1][2] * H[0][2] - H[0][1] * H[1][2] * H[0][2]);
    }

    if (H.size() == 4) {
        num4++;
        return F*(H[0][1] + ~H[0][1] * (H[1][2] * H[0][2] + H[0][3] * H[1][3] - H[1][2] * H[0][2] * H[0][3] * H[1][3] +
                H[1][2] * H[2][3] * H[0][3] * ~H[1][3] * ~H[0][2] + ~H[1][2] * H[2][3] * ~H[0][3] * H[1][3] * H[0][2]));
    }

    if (H.size() == 5) {
        num5++;
        return F*(~(~H[0][1] * ~H[0][2] * (H[1][2] * ~H[1][4] * (H[0][4] * ~H[0][3] * ~H[2][4] * (~H[3][4] +
                ~H[1][3] * ~H[2][3] * H[3][4]) + H[0][3] * ~H[1][3] * ~H[2][3] * (~H[2][4] + ~H[0][4] * ~H[3][4] * H[2][4])) +
                ~H[0][3] * ~H[0][4] * ~(~H[1][2] * ~H[1][3] * ~H[1][4])) +
                ~H[0][1] * ~H[1][3] * (H[1][4] * ~H[0][4] * (H[0][3] * ~H[0][2] * ~H[3][4] * (~H[2][3] +
                ~H[1][2] * ~H[2][4] * H[2][3]) + H[0][2] * ~H[1][2] * ~H[2][4] * (~H[3][4] + ~H[0][3] * ~H[2][3] * H[3][4])) +
                ~H[1][2] * ~H[1][4]) + H[1][3] * ~H[0][1] * ~H[0][3] * ~H[1][2] * (H[0][2] * ~H[0][4] * ~H[2][3] * (~H[2][4] +
                ~H[1][4] * ~H[3][4] * H[2][4]) + H[0][4] * ~H[1][4] * ~H[3][4] * (~H[2][3] + ~H[0][2] * ~H[2][4] * H[2][3]))));
    }
}

edge PairwiseConnectivity(vector<vector<edge>> &H, edge F, bool connected)
{
    factors++;
    if (!connected) {
        edge emptyEdge;
        if (!BridgeReduction(H)) {
            num0++;
            return emptyEdge;
        }

        connected = true;
    }

    if (H.size() < MAX_DIMENSIONAL)
        return SimpleCase(H, F);

    PenduntReduction(H, F, 0, false);

    int typeOfSpecialChain = 0;
    vector<int> Chain, checkedNodes;
    Chain = ChainReduction(H, Chain, checkedNodes, false);
    if (!Chain.empty()) {
        typeOfSpecialChain = Chain.back();
        Chain.pop_back();
    }
    //typeOfSpecialChain = 0;

    if (H.size() < MAX_DIMENSIONAL)
        return SimpleCase(H, F);

    if (typeOfSpecialChain == 1) {
        ch1++;
        int s, t;
        bool terminalCase = false;
        vector<int> Ch(Chain);
        vector<int>::iterator it, iteratorOfPivotNode;

        for (it = Ch.begin(); it < Ch.end(); ++it)
            if (*it == 0 || *it == 1) iteratorOfPivotNode = it;

        int placeOfPivotNodeT = iteratorOfPivotNode - Ch.begin();
        if (iteratorOfPivotNode == Ch.begin() || iteratorOfPivotNode == Ch.end() - 1)
            terminalCase = true;
        t = *iteratorOfPivotNode;
        t == 0 ? s = 1 : s = 0;

        edge P, L;
        P.C.push_back(1);
        L.C.push_back(1);
        vector<vector<edge>> graph(H);
        for (int i = 0; i<placeOfPivotNodeT; i++)
            L = L*graph[Ch[i]][Ch[i + 1]];
        for (int i = placeOfPivotNodeT; i<Ch.size() - 1; i++)
            P = P*graph[Ch[i]][Ch[i + 1]];
        edge T = P*L;

        // Remove the chain
        for (int i = 1; i<Ch.size() - 1; i++) {
            RemoveNode(graph, Ch[i]);
            // After removing the node, the numbering in the adjacency matrix changes
            for (int j = 0; j<Ch.size(); j++)
                if (Ch[i] < Ch[j]) Ch[j]--;
        }
        int x = Ch.front(), y = Ch.back();

        vector<vector<edge>> graphWithDeletedChain(graph);
        bool connectedGraph = ConnectedGraph(graph);
        vector<bool> visitedNodesBeforeRenumerate(visitedNodes);

        edge k;
        // Do not consider the term multiplied by 0
        if (placeOfPivotNodeT != 0) {
            (terminalCase) ? Renumerate(graph, y, t) : Renumerate(graph, y, 1);
            k = PairwiseConnectivity(graph, F * (P - T), connectedGraph); // Rsy
        }

        edge w;
        // Do not consider the term multiplied by 0
        if (placeOfPivotNodeT != Ch.size() - 1) {
            visitedNodes = visitedNodesBeforeRenumerate;
            graph = graphWithDeletedChain;
            (terminalCase) ? Renumerate(graph, x, t) : Renumerate(graph, x, 1);
            w = PairwiseConnectivity(graph, F * (L - T), connectedGraph); // Rsx
        }

        graph = graphWithDeletedChain;
        if (x == 0 || x == 1 || y == 0 || y == 1)
            (x == 0 || x == 1) ? ContractEdge(graph, y, x) : ContractEdge(graph, x, y);
        else {
            int xy = x;
            // when contracting, remove the node and renumber the numbering
            if (y < xy) xy--;
            ContractEdge(graph, y, x);
            Renumerate(graph, xy, 1);
        }

        edge m;
        if (!connectedGraph) {
            visitedNodes = visitedNodesBeforeRenumerate;
            int firstComponentSize = 0;
            for (int i = 0; i < visitedNodes.size(); i++)
                if (visitedNodes[i]) firstComponentSize++;
            int secondComponentSize = visitedNodes.size() - firstComponentSize;

            // Decomposition, do not consider cases where 1 node
            if (firstComponentSize != 1 && secondComponentSize != 1) {
                decomp2++;
                if ((visitedNodes[x] && visitedNodes[y]) || (!visitedNodes[x] && !visitedNodes[y]))
                    cout << "Eror in decomposition chr" << endl;

                graph = graphWithDeletedChain;
                if (!terminalCase) s = 0;
                if ((visitedNodes[s] && visitedNodes[x]) || (!visitedNodes[s] && !visitedNodes[x]))
                    (terminalCase) ? Renumerate(graph, x, t) : Renumerate(graph, x, 1);
                else
                    (terminalCase) ? Renumerate(graph, y, t) : Renumerate(graph, y, 1);

                m = PairwiseConnectivity(graph, F * T, connectedGraph);
            }
            else m = PairwiseConnectivity(graph, F * T, connected);
        }
        else m = PairwiseConnectivity(graph, F * T, connected);

        return k + w + m;
    }

    if (typeOfSpecialChain == 2) {
        ch2++;
        vector<int> Ch(Chain);
        vector<int>::iterator it, iteratorOfPivotNode;

        for (it = Ch.begin(); it<Ch.end(); ++it)
            if (*it == 0) iteratorOfPivotNode = it;
        int placeOfPivotNodeS = iteratorOfPivotNode - Ch.begin();

        for (it = Ch.begin(); it<Ch.end(); ++it)
            if (*it == 1) iteratorOfPivotNode = it;
        int placeOfPivotNodeT = iteratorOfPivotNode - Ch.begin();

        if (placeOfPivotNodeS > placeOfPivotNodeT) {
            int u = placeOfPivotNodeS;
            placeOfPivotNodeS = placeOfPivotNodeT;
            placeOfPivotNodeT = u;
        }

        edge P, L;
        P.C.push_back(1);
        L.C.push_back(1);
        vector<vector<edge>> graph(H);

        for (int i = 0; i<placeOfPivotNodeS; i++)
            L = L*graph[Ch[i]][Ch[i + 1]];
        for (int i = placeOfPivotNodeS; i<placeOfPivotNodeT; i++)
            P = P*graph[Ch[i]][Ch[i + 1]];
        for (int i = placeOfPivotNodeT; i<Ch.size() - 1; i++)
            L = L*graph[Ch[i]][Ch[i + 1]];

        for (int i = 1; i<Ch.size() - 1; i++) {
            RemoveNode(graph, Ch[i]);
            for (int j = 0; j<Ch.size(); j++)
                if (Ch[i] < Ch[j]) Ch[j]--;
        }
        int x = Ch.front(), y = Ch.back();

        bool connectedGraph = ConnectedGraph(graph);

        if (x != 0 && x != 1 && y != 0 && y != 1) {
            Renumerate(graph, x, 0);
            Renumerate(graph, y, 1);
        }
        if (x == 0 && y != 1) Renumerate(graph, y, 1);
        if (x == 1 && y != 0) Renumerate(graph, y, 0);
        if (y == 0 && x != 1) Renumerate(graph, x, 1);
        if (y == 1 && x != 0) Renumerate(graph, x, 0);
        edge n = PairwiseConnectivity(graph, F * (L - P * L), connectedGraph); // Rxy

        return F*P + n;
    }

    // Assume allowingEdge.node1 can not be the selected node
    edge allowingEdge = GetAllowingEdge(H);
    if (allowingEdge.node1 == 1 || allowingEdge.node1 == 0 )
        cout << "Eror in Allowing edge" << endl;

    vector<vector<edge>> graphWithContractedEdge(H);
    vector<vector<edge>> graphWithDeletedEdge(H);

    edge F1, F2;
    if (allowingEdge.simple == 0 && allowingEdge.power == 1) {
        F1 = F;
        F1.power++;
        F2 = F;
        F2.power++;
        F2.C.insert(F2.C.begin(), 0);
        F2.C.pop_back();
    }
    else {
        F1 = F*allowingEdge;
        F2 = F*~allowingEdge;
    }

    ContractEdge(graphWithContractedEdge, allowingEdge.node1, allowingEdge.node2);
    RemoveEdge(graphWithDeletedEdge, allowingEdge.node1, allowingEdge.node2);

    bool connectedGraph = ConnectedGraph(graphWithDeletedEdge);
    vector<bool> visitedNodesBeforeRenumerate(visitedNodes);

    if (!connectedGraph) {
        int firstComponentSize = 0;

        for (int i = 0; i < visitedNodes.size(); i++)
            if (visitedNodes[i]) firstComponentSize++;
        int secondComponentSize = visitedNodes.size() - firstComponentSize;

        // Decomposition, do not consider cases where 1 node
        if (firstComponentSize != 1 && secondComponentSize != 1) {
            decomp3++;
            vector<vector<edge>> G1(graphWithDeletedEdge);
            vector<vector<edge>> G2(graphWithDeletedEdge);
            edge F3 = Bin.front();
            edge m = PairwiseConnectivity(graphWithDeletedEdge, F2, connectedGraph);

            if (allowingEdge.node2 != 0 && allowingEdge.node2 != 1 && ((visitedNodes[0] && !visitedNodes[1]) ||
                    (!visitedNodes[0] && visitedNodes[1]))) {
                visitedNodes = visitedNodesBeforeRenumerate;
                if ((visitedNodes[allowingEdge.node1] && visitedNodes[0]) || (!visitedNodes[allowingEdge.node1] &&
                        !visitedNodes[0]))
                    Renumerate(G1, 1, allowingEdge.node1);
                else
                    Renumerate(G1, 1, allowingEdge.node2);

                edge k = PairwiseConnectivity(G1, F3, connectedGraph);

                visitedNodes = visitedNodesBeforeRenumerate;
                if ((visitedNodes[allowingEdge.node1] && visitedNodes[1]) || (!visitedNodes[allowingEdge.node1] &&
                        !visitedNodes[1]))
                    Renumerate(G2, 0, allowingEdge.node1);
                else
                    Renumerate(G2, 0, allowingEdge.node2);

                edge w = PairwiseConnectivity(G2, F3, connectedGraph);

                return F1*k*w + m;
            }

            if (allowingEdge.node2 != 0 && allowingEdge.node2 != 1 && ((visitedNodes[0] && visitedNodes[1]) ||
                    (!visitedNodes[0] && !visitedNodes[1]))) {
                visitedNodes = visitedNodesBeforeRenumerate;

                edge k = PairwiseConnectivity(G1, F1, connectedGraph);

                return k + m;
            }

            if (allowingEdge.node2 == 0 || allowingEdge.node2 == 1) {
                visitedNodes = visitedNodesBeforeRenumerate;
                if (allowingEdge.node2 == 0 && ((visitedNodes[0] && !visitedNodes[1]) || (!visitedNodes[0] &&
                        visitedNodes[1])))
                    Renumerate(G1, 0, allowingEdge.node1);
                if (allowingEdge.node2 == 1 && ((visitedNodes[0] && !visitedNodes[1]) || (!visitedNodes[0] &&
                        visitedNodes[1])))
                    Renumerate(G1, 1, allowingEdge.node1);

                edge k = PairwiseConnectivity(G1, F1, connectedGraph);

                return k + m;
            }
        }
        else return PairwiseConnectivity(graphWithContractedEdge, F1, connected) +
                    PairwiseConnectivity(graphWithDeletedEdge, F2, connectedGraph);
    }
    else return PairwiseConnectivity(graphWithContractedEdge, F1, connected) +
                PairwiseConnectivity(graphWithDeletedEdge, F2, connectedGraph);
}