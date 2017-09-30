#include "stdafx.h"
#include "funcs.h"
#include "globals.h"

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

void DeleteNode(vector<vector<edge>> &H, int q)
{
    H.erase(H.begin() + q);
    for (int i = 0; i<H.size(); i++)
        H[i].erase(H[i].begin() + q);
}

void DeleteEdge(vector<vector<edge>> &H, int q, int w)
{
    edge F;
    F.ex = false;
    H[q][w] = F;
    H[w][q] = F;
}

void ContractEdge(vector<vector<edge>> &H, int q, int w)
{
    for (int i = 0; i<H[q].size(); i++)
        if (H[q][i].ex && i != w) { // do not consider H[q][w]
            if (H[w][i].ex && H[q][i].power == 1 && H[w][i].power == 1) { // power==1 is falg of simple edge
                H[w][i].simple += H[q][i].simple + 1;
                H[i][w].simple += H[q][i].simple + 1;
            }

            if (H[w][i].ex && (H[q][i].power != 1 || H[w][i].power != 1)) { // arise in ChainReduction
                H[w][i] = H[q][i] + H[w][i] - H[q][i] * H[w][i];
                H[i][w] = H[w][i]; // for symmetry and less computing
            }

            if (!H[w][i].ex) {
                H[w][i] = H[q][i]; // so easy, no need to compute
                H[i][w] = H[q][i];
            }
        }

    DeleteNode(H, q);
}

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

        for (int i = 0; i<H.size(); i++) { //clear strings and columns for nodes s,t
            if (H[t][i].ex) DeleteEdge(H, t, i); //take into account the symmetry
            if (H[s][i].ex) DeleteEdge(H, s, i);
        }

        for (int i = 0; i<H.size(); i++) { // filling
            if (F[i].ex && i != s) {
                H[s][i] = F[i];
                if (i != t) H[i][s] = F[i]; // if do not fill another string
            }
            if (G[i].ex && i != t) {
                H[t][i] = G[i];
                if (i != s) H[i][t] = G[i];
            }
        }
    }
}

void PenduntReduction(vector<vector<edge>> &H, edge &F, int q, bool find)
{
    pendunt++;
    vector<int> nodePowers = GetNodePowers(H);
    if (find) nodePowers.erase(nodePowers.begin() + q); // in PenduntReduction we not consider pendunt node 0 || 1

    vector <int>::iterator it, iteratorOfPenduntNode = nodePowers.end() - 1;
    for (it = nodePowers.begin(); it<nodePowers.end(); ++it)
        if (*it == 1) iteratorOfPenduntNode = it;
    int numberOfPenduntNode = iteratorOfPenduntNode - nodePowers.begin(); // find pendunt node, can be 0,1

    if (*iteratorOfPenduntNode == 1 && H.size() > 2) { //otherwise all will be deleted
        for (int i = H[numberOfPenduntNode].size() - 1; i >= 0; i--)
            if (H[numberOfPenduntNode][i].ex) {
                //find incident edge to pendunt node, can't be 0,1 || 1,0
                if ((numberOfPenduntNode == 1 && i == 0) || (numberOfPenduntNode == 0 && i == 1)) { // that do not have loop
                    q = numberOfPenduntNode;
                    find = true;
                }

                if (numberOfPenduntNode != 0 && numberOfPenduntNode != 1) DeleteNode(H, numberOfPenduntNode);

                if ((numberOfPenduntNode == 1 && i != 0) || (numberOfPenduntNode == 0 && i != 1)) {
                    F = F*H[numberOfPenduntNode][i];
                    DeleteNode(H, numberOfPenduntNode);
                    i--; // after delenode not forget about i
                    if (i != 1) Renumerate(H, i, 1); // always must be 1
                }
                break; // arise the eror if abort this
            }

        PenduntReduction(H, F, q, find);
    }
}

bool BridgeReduction(vector<vector<edge>> &H)
{
    bridges++;
    if (visitedNodes.size() != H.size())
        cout << "Eror BridgeReduction!" << endl;

    if (visitedNodes[0] && visitedNodes[1]) { // only one of this cases exist
        for (int i = H.size() - 1; i >= 0; i--)
            if (!visitedNodes[i]) DeleteNode(H, i);  // when end is 0,0 make mistake if start from 0
    }

    if (!visitedNodes[0] && !visitedNodes[1]) {
        for (int i = H.size() - 1; i >= 0; i--)
            if (visitedNodes[i]) DeleteNode(H, i);
    }

    if ((visitedNodes[0] && !visitedNodes[1]) || (!visitedNodes[0] && visitedNodes[1]))
        return false;

    return true;
}

edge GetAllowingEdge(vector<vector<edge>> &H)
{
    edge F;
    vector<int> nodepower = GetNodePowers(H);
    vector<int>::iterator it, smallest = nodepower.end() - 1;

    for (it = nodepower.begin(); it<nodepower.end(); ++it)
        if (*it <= *smallest && (it - nodepower.begin() != 0) && (it - nodepower.begin() != 1))
            smallest = it;
    //becouse after DeleteEdge we can get unconnected graph in case nodepower=1
    int r = smallest - nodepower.begin(); //can't be 0,1

    for (int i = H[r].size() - 1; i >= 0; i--)
        if (H[r][i].ex) {  //find incident edge selected node, can be 0,1
            F = H[r][i];
            F.node1 = r; // Becouse we use only matrix C
            F.node2 = i;
            break;
        }

    return F;
}

bool ElementInside(vector<int> P, int x)
{
    if (P.empty()) return false;
    else
        for (int i = 0; i<P.size(); i++)
            if (x == P[i]) return true;

    return false;
}

vector<int> GetChain(vector<vector<edge>> &H, vector<int> &checkedNodes)
{
    vector<int> nodePowers = GetNodePowers(H);
    vector<int> P; // chain nodes vector

    bool find = false;
    int i = 0;
    while (!find && i<H.size()) {
        for (int j = 0; j<H[i].size(); j++) {
            if (H[i][j].ex && !(ElementInside(checkedNodes, i) && ElementInside(checkedNodes, j)) && (nodePowers[j] == 2 || nodePowers[i] == 2)) {
                // find node power = 2 and add nodes of edge where is selected node
                P.push_back(i);
                P.push_back(j);
                find = true;
                break;
            }
        }
        i++;
    }

    if (!P.empty()) { // expand vector in both sides if it is not empty
        i = P.front();
        while (nodePowers[i] == 2 && i != P.back())
            // find node incident to left node of our chain and add it
            for (int j = 0; j<H[i].size(); j++)
                if (H[i][j].ex && j != P[1]) {
                    if (j != P.back()) P.insert(P.begin(), j);
                    i = j;
                    break;
                }

        i = P.back();
        while (nodePowers[i] == 2 && i != P.front())
            for (int j = 0; j<H[i].size(); j++)
                if (H[i][j].ex && j != P[P.size() - 2]) {
                    if (j != P.front()) P.push_back(j);
                    i = j;
                    break;
                }

        return P;
    }

    return P;
}

vector<int> ChainReduction(vector<vector<edge>> &H, vector<int> &Ch, vector<int> &checkedNodes,
                           bool chainWithTwoSpecialNodes)
{
    vector<int> P = GetChain(H, checkedNodes);
    int countSelectedNodes = 0;

    if (!P.empty()) {
        for (int i = 0; i<P.size(); i++)
            if (P[i] == 0 || P[i] == 1) countSelectedNodes++; // countSelectedNodes selected nodes in chain

        vector<int>::iterator it, r = P.begin();
        if (countSelectedNodes == 1) {
            for (it = P.begin(); it<P.end(); ++it)
                if (*it == 0 || *it == 1) r = it;
            if ((r == P.begin() || r == P.end() - 1) && !Ch.empty())
                countSelectedNodes = 0; // reduct special chains
        }

        if ((countSelectedNodes == 1 || countSelectedNodes == 2) && !chainWithTwoSpecialNodes) {
            if (countSelectedNodes == 2) { // have only chain with countSelectedNodes=2
                chainWithTwoSpecialNodes = true;
                checkedNodes = P;
            }

            if (countSelectedNodes == 1) {
                if (!Ch.empty()) {
                    for (it = Ch.begin(); it<Ch.end() - 1; ++it)
                        if (*it == 0 || *it == 1) r = it;  // Ch.back()=countSelectedNodes
                    if (r == Ch.begin() || r == Ch.end() - 2)
                        checkedNodes.clear(); // next recursive call reduct Ch
                }
                for (int j = 0; j<P.size(); j++)
                    if (!ElementInside(checkedNodes, P[j])) checkedNodes.push_back(P[j]);
            }

            P.push_back(countSelectedNodes);
            Ch = P;

            return ChainReduction(H, Ch, checkedNodes, chainWithTwoSpecialNodes);
        }
        else
            countSelectedNodes = 0; // to reduct chain(s) when we chainWithTwoSpecialNodes countSelectedNodes=2

        if (countSelectedNodes == 0) { // Replace chain by edge
            chr++;
            edge newEdge;
            newEdge.C.push_back(1);

            for (int j = 0; j<P.size() - 1; j++)
                newEdge = newEdge*H[P[j]][P[j + 1]];

            for (int j = 1; j<P.size() - 1; j++) {
                DeleteNode(H, P[j]); // delete only inner nodes of chain
                for (int k = 0; k<P.size(); k++)
                    if (P[j] < P[k]) P[k]--; // not forget about P
                for (int k = 0; k<Ch.size(); k++)
                    if (P[j] < Ch[k]) Ch[k]--; // not forget about Ch
                for (int k = 0; k<checkedNodes.size(); k++)
                    if (P[j] < checkedNodes[k]) checkedNodes[k]--; // not forget about checkedNodes
            }

            int x = P.front(), y = P.back();

            if (H[x][y].ex) { // Becouse newEdge.power != 1
                H[x][y] = H[x][y] + newEdge - H[x][y] * newEdge;
                H[y][x] = H[x][y];
            }
            else {
                H[x][y] = newEdge;
                H[y][x] = newEdge;
            }

            if (!Ch.empty()) { // expand vector in both sides if it is not empty
                Ch.pop_back();
                vector<int> nodePowers = GetNodePowers(H);

                int i = Ch.front();
                while (nodePowers[i] == 2 && i != Ch.back())
                    // chainWithTwoSpecialNodes node incident to left node of our chain and add it
                    for (int j = 0; j<H[i].size(); j++)
                        if (H[i][j].ex && j != Ch[1]) {
                            if (j != Ch.back()) {
                                Ch.insert(Ch.begin(), j);
                                if (!ElementInside(checkedNodes, j)) checkedNodes.push_back(j);
                            }
                            i = j;
                            break;
                        }

                i = Ch.back();
                while (nodePowers[i] == 2 && i != Ch.front())
                    for (int j = 0; j<H[i].size(); j++)
                        if (H[i][j].ex && j != Ch[Ch.size() - 2]) {
                            if (j != Ch.front()) {
                                Ch.push_back(j);
                                if (!ElementInside(checkedNodes, j)) checkedNodes.push_back(j);
                            }
                            i = j;
                            break;
                        }

                countSelectedNodes = 0;
                for (int i = 0; i<Ch.size(); i++)
                    if (Ch[i] == 0 || Ch[i] == 1) countSelectedNodes++; // countSelectedNodes selected nodes in chain

                Ch.push_back(countSelectedNodes);
            }

            return ChainReduction(H, Ch, checkedNodes, chainWithTwoSpecialNodes);
        }
    }

    return Ch;
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
        return SimpleCase(H,F);

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
        return SimpleCase(H,F);

    if (typeOfSpecialChain == 1) {
        ch1++;
        int s, t;
        bool terminal = false;
        vector<int> Ch(Chain);
        vector<int>::iterator it, iteratorOfPivoteNode;

        for (it = Ch.begin(); it < Ch.end(); ++it)
            if (*it == 0 || *it == 1) iteratorOfPivoteNode = it;

        int placeOfPivoteNodeT = iteratorOfPivoteNode - Ch.begin(); // place of pivote node t=(0 or 1) into vector Ch
        if (iteratorOfPivoteNode == Ch.begin() || iteratorOfPivoteNode == Ch.end() - 1)
            terminal = true;
        t = *iteratorOfPivoteNode;
        t == 0 ? s = 1 : s = 0; // need for case terminal, s<=>t doesn't matter

        edge P, L;
        P.C.push_back(1);
        L.C.push_back(1);
        vector<vector<edge>> graph(H);
        for (int i = 0; i<placeOfPivoteNodeT; i++)
            L = L*graph[Ch[i]][Ch[i + 1]];
        for (int i = placeOfPivoteNodeT; i<Ch.size() - 1; i++)
            P = P*graph[Ch[i]][Ch[i + 1]];
        edge T = P*L;

        for (int i = 1; i<Ch.size() - 1; i++) { // after this we get 2 nodes from chain, one of them can be pivote
            DeleteNode(graph, Ch[i]);
            for (int j = 0; j<Ch.size(); j++)
                if (Ch[i] < Ch[j]) Ch[j]--; // not forget about Ch
        }
        int x = Ch.front(), y = Ch.back(); // matter x<=>y

        vector<vector<edge>> graphWithDeletedChain(graph);
        bool connectedGraph = ConnectedGraph(graph);
        vector<bool> visitedNodesBeforeRenumerate(visitedNodes);

        edge k;
        if (placeOfPivoteNodeT != 0) {
            (terminal) ? Renumerate(graph, y, t) : Renumerate(graph, y, 1);
            // s=0||1, becouse we don't delete pivote node
            // after delete: s - node out of chain always 0, so t=1
            k = PairwiseConnectivity(graph, F * (P - T), connectedGraph); // Rsy
        }

        edge w;
        if (placeOfPivoteNodeT != Ch.size() - 1) {
            visitedNodes = visitedNodesBeforeRenumerate; // same size so all right
            graph = graphWithDeletedChain;
            (terminal) ? Renumerate(graph, x, t) : Renumerate(graph, x, 1);
            w = PairwiseConnectivity(graph, F * (L - T), connectedGraph); // Rsx
        }

        graph = graphWithDeletedChain;
        if (x == 0 || x == 1 || y == 0 || y == 1) // only 1 case can be
            (x == 0 || x == 1) ? ContractEdge(graph, y, x) : ContractEdge(graph, x, y);
            // don't delete pivote node, get t
        else {
            int xy = x;
            if (y < xy) xy--; // not forget about xy
            ContractEdge(graph, y, x);
            Renumerate(graph, xy, 1); // if terminal x or y is pivote
        }

        edge m;
        if (!connectedGraph) {
            visitedNodes = visitedNodesBeforeRenumerate;
            int firstComponentSize = 0;
            for (int i = 0; i < visitedNodes.size(); i++)
                if (visitedNodes[i]) firstComponentSize++;
            int secondComponentSize = visitedNodes.size() - firstComponentSize;

            if (firstComponentSize != 1 && secondComponentSize != 1) {
                decomp2++;
                if ((visitedNodes[x] && visitedNodes[y]) || (!visitedNodes[x] && !visitedNodes[y]))
                    cout << "Eror in decomposition chr" << endl;

                graph = graphWithDeletedChain;
                if (!terminal) s = 0; // else s = s
                if ((visitedNodes[s] && visitedNodes[x]) || (!visitedNodes[s] && !visitedNodes[x]))
                    (terminal) ? Renumerate(graph, x, t) : Renumerate(graph, x, 1);
                else
                    (terminal) ? Renumerate(graph, y, t) : Renumerate(graph, y, 1);

                m = PairwiseConnectivity(graph, F * T, connectedGraph);
            }
            else m = PairwiseConnectivity(graph, F * T, connected);
            // Rs<x,y>, no Renumerate becouse s,t always pivote nodes
        }
        else m = PairwiseConnectivity(graph, F * T, connected);
        // Rs<x,y>, no Renumerate becouse s,t always pivote nodes

        return k + w + m;
    }

    if (typeOfSpecialChain == 2) {
        ch2++;
        vector<int> Ch(Chain);
        vector<int>::iterator it, iteratorOfPivoteNode;

        for (it = Ch.begin(); it<Ch.end(); ++it)
            if (*it == 0) iteratorOfPivoteNode = it;
        int placeOfPivoteNodeS = iteratorOfPivoteNode - Ch.begin(); // place of pivote node s=0 into vector Ch

        for (it = Ch.begin(); it<Ch.end(); ++it)
            if (*it == 1) iteratorOfPivoteNode = it;
        int placeOfPivoteNodeT = iteratorOfPivoteNode - Ch.begin(); // place of pivote node t=1 into vector Ch

        if (placeOfPivoteNodeS > placeOfPivoteNodeT) { // that * edges in chain, placeOfPivoteNodeS<placeOfPivoteNodeT always
            int u = placeOfPivoteNodeS;
            placeOfPivoteNodeS = placeOfPivoteNodeT;
            placeOfPivoteNodeT = u;
        }

        edge P, L;
        P.C.push_back(1);
        L.C.push_back(1);
        vector<vector<edge>> graph(H);

        for (int i = 0; i<placeOfPivoteNodeS; i++)
            L = L*graph[Ch[i]][Ch[i + 1]];
        for (int i = placeOfPivoteNodeS; i<placeOfPivoteNodeT; i++)
            P = P*graph[Ch[i]][Ch[i + 1]];
        for (int i = placeOfPivoteNodeT; i<Ch.size() - 1; i++)
            L = L*graph[Ch[i]][Ch[i + 1]];
        for (int i = 1; i<Ch.size() - 1; i++) {
            DeleteNode(graph, Ch[i]);
            for (int j = 0; j<Ch.size(); j++)
                if (Ch[i] < Ch[j]) Ch[j]--; // not forget about Ch
        }
        int x = Ch.front(), y = Ch.back(); // doesn't matter x<=>y

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

        return F*P + n; // F* becouse we don't make recursive call
    }

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
    DeleteEdge(graphWithDeletedEdge, allowingEdge.node1, allowingEdge.node2);

    bool connectedGraph = ConnectedGraph(graphWithDeletedEdge);
    vector<bool> visitedNodesBeforeRenumerate(visitedNodes);

    if (!connectedGraph) { // allowingEdge.node2 can be 0 or 1
        int firstComponentSize = 0;

        for (int i = 0; i < visitedNodes.size(); i++)
            if (visitedNodes[i]) firstComponentSize++;

        int secondComponentSize = visitedNodes.size() - firstComponentSize;

        if (firstComponentSize != 1 && secondComponentSize != 1) {
            decomp3++;
            vector<vector<edge>> G1(graphWithDeletedEdge);
            vector<vector<edge>> G2(graphWithDeletedEdge);
            edge F3 = Bin.front(); // create pseudo edge for computing
            edge m = PairwiseConnectivity(graphWithDeletedEdge, F2, connectedGraph); // use visitedNodes

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

                return F1*k*w + m; // dont *F twice
            }

            if (allowingEdge.node2 != 0 && allowingEdge.node2 != 1 && ((visitedNodes[0] && visitedNodes[1]) ||
                    (!visitedNodes[0] && !visitedNodes[1]))) {
                visitedNodes = visitedNodesBeforeRenumerate;

                edge k = PairwiseConnectivity(G1, F1, connectedGraph);

                return k + m;
            }

            if (allowingEdge.node2 == 0 || allowingEdge.node2 == 1) { // if 0 and 1 in one component nothing renum
                visitedNodes = visitedNodesBeforeRenumerate;
                if (allowingEdge.node2 == 0 && ((visitedNodes[0] && !visitedNodes[1]) || (!visitedNodes[0] &&
                        visitedNodes[1])))
                    Renumerate(G1, 0, allowingEdge.node1); // becouse can't return 0
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