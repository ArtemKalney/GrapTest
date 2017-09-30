#include "stdafx.h"
#include "funcs.h"
#include "globals.h"

vector<int> GetNodePowers(vector<vector<edge>> &H)
{
    vector <int> nodepower(H.size());
    int i = 0;
    while (i < H.size()) {
        for (int j = 0; j<H[i].size(); j++)
            if (H[i][j].ex) nodepower[i]++;
        i++;
    }

    return nodepower;
}

void DFS(vector<vector<edge>> &H, int q)
{
    visited[q] = true;
    for (int i = 0; i<H[q].size(); i++)
        if (H[q][i].ex && !visited[i]) DFS(H, i);
}

bool ConnectedGraph(vector<vector<edge>> &H)
{
    int count = 0;
    visited.resize(H.size());

    for (int i = 0; i<visited.size(); i++) visited[i] = false;

    DFS(H, 0);
    for (int i = 0; i<visited.size(); i++)
        if (visited[i]) count++;

    return count == H.size();
}

inline void DeleteNode(vector<vector<edge>> &H, int q)
{
    H.erase(H.begin() + q);
    for (int i = 0; i<H.size(); i++)
        H[i].erase(H[i].begin() + q);
}

inline void deledge(vector<vector<edge>> &H, int q, int w)
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

            if (H[w][i].ex && (H[q][i].power != 1 || H[w][i].power != 1)) { // arise in chainreduction
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

        if (!visited.empty() && max(s, t) < visited.size()) {
            bool r = visited[s];
            visited[s] = visited[t];
            visited[t] = r;
        }

        for (int i = 0; i<H.size(); i++) { //clear strings and columns for nodes s,t
            if (H[t][i].ex) deledge(H, t, i); //take into account the symmetry
            if (H[s][i].ex) deledge(H, s, i);
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

edge penduntreduction(vector<vector<edge>> &H, edge F, int q, bool find)
{
    pendunt++;
    vector<int> nodepower = GetNodePowers(H);
    if (find) nodepower.erase(nodepower.begin() + q); // in penduntreduction we not consider pendunt node 0 || 1

    vector <int>::iterator it, smallest = nodepower.end() - 1;
    for (it = nodepower.begin(); it<nodepower.end(); ++it)
        if (*it == 1) smallest = it;
    int r = smallest - nodepower.begin(); // find pendunt node, can be 0,1

    if (*smallest == 1 && H.size() > 2) { //otherwise all will be deleted
        for (int i = H[r].size() - 1; i >= 0; i--)
            if (H[r][i].ex) {
                //find incident edge to pendunt node, can't be 0,1 || 1,0
                if ((r == 1 && i == 0) || (r == 0 && i == 1)) { // that do not have loop
                    q = r;
                    find = true;
                }

                if (r != 0 && r != 1) DeleteNode(H, r);

                if ((r == 1 && i != 0) || (r == 0 && i != 1)) {
                    F = F*H[r][i];
                    DeleteNode(H, r);
                    i--; // after delenode not forget about i
                    if (i != 1) Renumerate(H, i, 1); // always must be 1
                }
                break; // arise the eror if abort this
            }
        return penduntreduction(H, F, q, find);
    }
    return F;

}

bool bridgereduction(vector<vector<edge>> &H)
{
    bridges++;
    if (visited.size() != H.size())
        cout << "Eror bridgereduction!" << endl;

    if (visited[0] && visited[1]) { // only one of this cases exist
        for (int i = H.size() - 1; i >= 0; i--)
            if (!visited[i]) DeleteNode(H, i);  // when end is 0,0 make mistake if start from 0
    }

    if (!visited[0] && !visited[1]) {
        for (int i = H.size() - 1; i >= 0; i--)
            if (visited[i]) DeleteNode(H, i);
    }

    if ((visited[0] && !visited[1]) || (!visited[0] && visited[1]))
        return false;

    return true;
}

edge fedge(vector<vector<edge>> &H)
{
    edge F;
    vector<int> nodepower = GetNodePowers(H);
    vector<int>::iterator it, smallest = nodepower.end() - 1;

    for (it = nodepower.begin(); it<nodepower.end(); ++it)
        if (*it <= *smallest && (it - nodepower.begin() != 0) && (it - nodepower.begin() != 1))
            smallest = it;
    //becouse after deledge we can get unconnected graph in case nodepower=1
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

vector<int> GetChain(vector<vector<edge>> &H, vector<int> &V)
{
    vector<int> nodepower = GetNodePowers(H);
    vector<int> P; // chain nodes vector

    bool find = false;
    int i = 0;
    while (!find && i<H.size()) {
        for (int j = 0; j<H[i].size(); j++) {
            if (H[i][j].ex && !(ElementInside(V, i) && ElementInside(V, j)) && (nodepower[j] == 2 || nodepower[i] == 2)) {
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
        while (nodepower[i] == 2 && i != P.back())
            // find node incident to left node of our chain and add it
            for (int j = 0; j<H[i].size(); j++)
                if (H[i][j].ex && j != P[1]) {
                    if (j != P.back()) P.insert(P.begin(), j);
                    i = j;
                    break;
                }
        i = P.back();
        while (nodepower[i] == 2 && i != P.front())
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

vector<int> chainreduction(vector<vector<edge>> &H, vector<int> &Ch, vector<int> &V, bool find)
{
    vector<int> P = GetChain(H, V);
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

        if ((countSelectedNodes == 1 || countSelectedNodes == 2) && !find) {
            if (countSelectedNodes == 2) { // have only chain with countSelectedNodes=2
                find = true;
                V = P;
            }
            if (countSelectedNodes == 1) {
                if (!Ch.empty()) {
                    for (it = Ch.begin(); it<Ch.end() - 1; ++it)
                        if (*it == 0 || *it == 1) r = it;  // Ch.back()=countSelectedNodes
                    if (r == Ch.begin() || r == Ch.end() - 2)
                        V.clear(); // next recursive call reduct Ch
                }
                for (int j = 0; j<P.size(); j++)
                    if (!ElementInside(V, P[j])) V.push_back(P[j]);
            }

            P.push_back(countSelectedNodes);
            Ch = P;

            return chainreduction(H, Ch, V, find);
        }
        else
            countSelectedNodes = 0; // to reduct chain(s) when we find countSelectedNodes=2

        if (countSelectedNodes == 0) { // Replace chain by edge
            chr++;
            edge newedge;
            newedge.C.push_back(1);

            for (int j = 0; j<P.size() - 1; j++)
                newedge = newedge*H[P[j]][P[j + 1]];

            for (int j = 1; j<P.size() - 1; j++) {
                DeleteNode(H, P[j]); // delete only inner nodes of chain
                for (int k = 0; k<P.size(); k++)
                    if (P[j] < P[k]) P[k]--; // not forget about P
                for (int k = 0; k<Ch.size(); k++)
                    if (P[j] < Ch[k]) Ch[k]--; // not forget about Ch
                for (int k = 0; k<V.size(); k++)
                    if (P[j] < V[k]) V[k]--; // not forget about V
            }

            int x = P.front(), y = P.back();

            if (H[x][y].ex) { // Becouse newedge.power != 1
                H[x][y] = H[x][y] + newedge - H[x][y] * newedge;
                H[y][x] = H[x][y];
            }
            else {
                H[x][y] = newedge;
                H[y][x] = newedge;
            }

            if (!Ch.empty()) { // expand vector in both sides if it is not empty
                Ch.pop_back();
                vector<int> nodepower = GetNodePowers(H);

                int i = Ch.front();
                while (nodepower[i] == 2 && i != Ch.back())
                    // find node incident to left node of our chain and add it
                    for (int j = 0; j<H[i].size(); j++)
                        if (H[i][j].ex && j != Ch[1]) {
                            if (j != Ch.back()) {
                                Ch.insert(Ch.begin(), j);
                                if (!ElementInside(V, j)) V.push_back(j);
                            }
                            i = j;
                            break;
                        }

                i = Ch.back();
                while (nodepower[i] == 2 && i != Ch.front())
                    for (int j = 0; j<H[i].size(); j++)
                        if (H[i][j].ex && j != Ch[Ch.size() - 2]) {
                            if (j != Ch.front()) {
                                Ch.push_back(j);
                                if (!ElementInside(V, j)) V.push_back(j);
                            }
                            i = j;
                            break;
                        }

                countSelectedNodes = 0;
                for (int i = 0; i<Ch.size(); i++)
                    if (Ch[i] == 0 || Ch[i] == 1) countSelectedNodes++; // countSelectedNodes selected nodes in chain

                Ch.push_back(countSelectedNodes);
            }

            return chainreduction(H, Ch, V, find);
        }
    }

    return Ch;
}

edge SimpleCase (vector<vector<edge>> &H, edge F)
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
        edge N;
        if (!bridgereduction(H)) {
            num0++;
            return N;
        }
        connected = true;
    }

    if (H.size() < MAX_DIMENSIONAL)
        return SimpleCase(H,F);

    F = penduntreduction(H, F, 0, false);

    int typeOfSpecialChain = 0;
    vector<int> Chain, V;
    Chain = chainreduction(H, Chain, V, false);
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
        vector<vector<edge>> G(H);
        for (int i = 0; i<placeOfPivoteNodeT; i++)
            L = L*G[Ch[i]][Ch[i + 1]];
        for (int i = placeOfPivoteNodeT; i<Ch.size() - 1; i++)
            P = P*G[Ch[i]][Ch[i + 1]];
        edge T = P*L;

        for (int i = 1; i<Ch.size() - 1; i++) { // after this we get 2 nodes from chain, one of them can be pivote
            DeleteNode(G, Ch[i]);
            for (int j = 0; j<Ch.size(); j++)
                if (Ch[i] < Ch[j]) Ch[j]--; // not forget about Ch
        }
        int x = Ch.front(), y = Ch.back(); // matter x<=>y

        vector<vector<edge>> G1(G);
        bool connec = ConnectedGraph(G);
        vector<bool> visited1 (visited);

        edge k;
        if (placeOfPivoteNodeT != 0) {
            (terminal) ? Renumerate(G, y, t) : Renumerate(G, y, 1);  // s=0||1, becouse we don't delete pivote node
            // after delete: s - node out of chain always 0, so t=1
            k = PairwiseConnectivity(G, F * (P - T), connec); // Rsy
        }

        edge w;
        if (placeOfPivoteNodeT != Ch.size() - 1) {
            visited = visited1; // same size so all right
            G = G1;
            (terminal) ? Renumerate(G, x, t) : Renumerate(G, x, 1);
            w = PairwiseConnectivity(G, F * (L - T), connec); // Rsx
        }

        G = G1;
        if (x == 0 || x == 1 || y == 0 || y == 1) // only 1 case can be
            (x == 0 || x == 1) ? ContractEdge(G, y, x) : ContractEdge(G, x, y); // don't delete pivote node, get t
        else {
            int xy = x;
            if (y < xy) xy--; // not forget about xy
            ContractEdge(G, y, x);
            Renumerate(G, xy, 1); // if terminal x or y is pivote
        }

        edge m;
        if (!connec) {
            visited = visited1;
            int component1_size = 0;
            for (int i = 0; i < visited.size(); i++)
                if (visited[i]) component1_size++;
            int component2_size = visited.size() - component1_size;

            if (component1_size != 1 && component2_size != 1) {
                decomp2++;
                if ((visited[x] && visited[y]) || (!visited[x] && !visited[y]))
                    cout << "Eror in decomposition chr" << endl;

                vector<vector<edge>> J(G1);
                if (!terminal) s = 0; // else s = s
                if ((visited[s] && visited[x]) || (!visited[s] && !visited[x]))
                    (terminal) ? Renumerate(J, x, t) : Renumerate(J, x, 1);
                else
                    (terminal) ? Renumerate(J, y, t) : Renumerate(J, y, 1);

                m = PairwiseConnectivity(J, F * T, connec);
            }
            else m = PairwiseConnectivity(G, F * T, connected); // Rs<x,y>, no Renumerate becouse s,t always pivote nodes
        }
        else m = PairwiseConnectivity(G, F * T, connected); // Rs<x,y>, no Renumerate becouse s,t always pivote nodes

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
        vector<vector<edge>> G(H);
        for (int i = 0; i<placeOfPivoteNodeS; i++)
            L = L*G[Ch[i]][Ch[i + 1]];
        for (int i = placeOfPivoteNodeS; i<placeOfPivoteNodeT; i++)
            P = P*G[Ch[i]][Ch[i + 1]];
        for (int i = placeOfPivoteNodeT; i<Ch.size() - 1; i++)
            L = L*G[Ch[i]][Ch[i + 1]];
        for (int i = 1; i<Ch.size() - 1; i++) {
            DeleteNode(G, Ch[i]);
            for (int j = 0; j<Ch.size(); j++)
                if (Ch[i] < Ch[j]) Ch[j]--; // not forget about Ch
        }
        int x = Ch.front(), y = Ch.back(); // doesn't matter x<=>y

        bool connec = ConnectedGraph(G);

        if (x != 0 && x != 1 && y != 0 && y != 1) {
            Renumerate(G, x, 0);
            Renumerate(G, y, 1);
        }
        if (x == 0 && y != 1) Renumerate(G, y, 1);
        if (x == 1 && y != 0) Renumerate(G, y, 0);
        if (y == 0 && x != 1) Renumerate(G, x, 1);
        if (y == 1 && x != 0) Renumerate(G, x, 0);
        edge n = PairwiseConnectivity(G, F * (L - P * L), connec); // Rxy

        return F*P + n; // F* becouse we don't make recursive call
    }

    edge W = fedge(H);
    if (W.node1 == 1 || W.node1 == 0 ) cout << "Eror in Allowing edge" << endl;

    vector<vector<edge>> H1(H);
    vector<vector<edge>> H2(H);

    edge F1, F2;
    if (W.simple == 0 && W.power == 1) {
        F1 = F;
        F1.power++;
        F2 = F;
        F2.power++;
        F2.C.insert(F2.C.begin(), 0);
        F2.C.pop_back();
    }
    else {
        F1 = F*W;
        F2 = F*~W;
    }

    ContractEdge(H1, W.node1, W.node2);
    deledge(H2, W.node1, W.node2);

    bool connec = ConnectedGraph(H2);
    vector<bool> visited1(visited);

    if (!connec) { // W.node2 can be 0 or 1
        int component1_size = 0;

        for (int i = 0; i < visited.size(); i++)
            if (visited[i]) component1_size++;

        int component2_size = visited.size() - component1_size;

        if (component1_size != 1 && component2_size != 1) {
            decomp3++;
            vector<vector<edge>> J1(H2);
            vector<vector<edge>> J2(H2);
            edge F3 = Bin.front(); // create pseudo edge for computing

            edge m = PairwiseConnectivity(H2, F2, connec); // use visited

            if (W.node2 != 0 && W.node2 != 1 && ((visited[0] && !visited[1]) || (!visited[0] && visited[1]))) {
                visited = visited1;
                if ((visited[W.node1] && visited[0]) || (!visited[W.node1] && !visited[0]))
                    Renumerate(J1, 1, W.node1);
                else
                    Renumerate(J1, 1, W.node2);
                edge k = PairwiseConnectivity(J1, F3, connec);

                visited = visited1;
                if ((visited[W.node1] && visited[1]) || (!visited[W.node1] && !visited[1]))
                    Renumerate(J2, 0, W.node1);
                else
                    Renumerate(J2, 0, W.node2);
                edge w = PairwiseConnectivity(J2, F3, connec);

                return F1*k*w + m; // dont *F twice
            }

            if (W.node2 != 0 && W.node2 != 1 && ((visited[0] && visited[1]) || (!visited[0] && !visited[1]))) {
                visited = visited1;
                edge k = PairwiseConnectivity(J1, F1, connec);

                return k + m;
            }

            if (W.node2 == 0 || W.node2 == 1) { // if 0 and 1 in one component nothing renum
                visited = visited1;
                if (W.node2 == 0 && ((visited[0] && !visited[1]) || (!visited[0] && visited[1])))
                    Renumerate(J1, 0, W.node1); // becouse can't return 0
                if (W.node2 == 1 && ((visited[0] && !visited[1]) || (!visited[0] && visited[1])))
                    Renumerate(J1, 1, W.node1);
                edge k = PairwiseConnectivity(J1, F1, connec);

                return k + m;
            }
        }
        else return PairwiseConnectivity(H1, F1, connected) + PairwiseConnectivity(H2, F2, connec);
    }
    else return PairwiseConnectivity(H1, F1, connected) + PairwiseConnectivity(H2, F2, connec);
}