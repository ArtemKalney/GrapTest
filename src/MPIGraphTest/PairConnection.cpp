#include "stdafx.h"
#include "funcs.h"
#include "globals.h"
#include "Cclass.h"
#include "ApplicationSettings.h"

vector<int> GetNodePower(vector<vector<Edge>> &H) {
    vector <int> nodepower(H.size());
    int i = 0;
    while (i < H.size()) {
        for (int j = 0; j<H[i].size(); j++) if (H[i][j].Ex) nodepower[i]++;
        i++;
    }
    return nodepower;
}

void DFS(vector<vector<Edge>> &H, int q) {
    Visited[q] = true;
    for (int i = 0; i<H[q].size(); i++) if (H[q][i].Ex && !Visited[i]) DFS(H, i);
}

bool IsConnected(vector<vector<Edge>> &H) {
    int count = 0;
    Visited.resize(H.size());
    for (int i = 0; i<Visited.size(); i++) Visited[i] = false;

    DFS(H, 0);
    for (int i = 0; i<Visited.size(); i++) if (Visited[i]) count++;

    if (count != H.size()) return false;
    else return true;
}

inline void DeleteNode(vector<vector<Edge>> &H, int q) {
    H.erase(H.begin() + q);
    for (int i = 0; i<H.size(); i++) H[i].erase(H[i].begin() + q);
}

inline void DeleteEdge(vector<vector<Edge>> &H, int q, int w) {
    Edge F;
    F.Ex = false;
    H[q][w] = F;
    H[w][q] = F;
}

void ContractEdge(vector<vector<Edge>> &H, int q, int w) {
    for (int i = 0; i<H[q].size(); i++) if (H[q][i].Ex && i != w) { // do not consider H[q][w]
            if (H[w][i].Ex && H[q][i].Power == 1 && H[w][i].Power == 1) { // Power==1 is falg of Simple Edge
                H[w][i].Simple += H[q][i].Simple + 1;
                H[i][w].Simple += H[q][i].Simple + 1;
            }
            if (H[w][i].Ex && (H[q][i].Power != 1 || H[w][i].Power != 1)) { // arise in ChainReduction
                H[w][i] = H[q][i] + H[w][i] - H[q][i] * H[w][i];
                H[i][w] = H[w][i]; // for symmetry and less computing
            }
            if (!H[w][i].Ex) {
                H[w][i] = H[q][i]; // so easy, no need to compute
                H[i][w] = H[q][i];
            }
        }
    DeleteNode(H, q);
}

void Renumerate(vector<vector<Edge>> &H, int s, int t) {
    if (s != t) {
        vector<Edge> F, G;
        F = H[s];
        G = H[t];
        for (int i = 0; i<H.size(); i++) if (i != s && i != t) swap(F[i], G[i]);

        if (!Visited.empty() && max(s, t) < Visited.size()) {
            bool r = Visited[s];
            Visited[s] = Visited[t];
            Visited[t] = r;
        }

        for (int i = 0; i<H.size(); i++) { //clear strings and columns for nodes s,t
            if (H[t][i].Ex) DeleteEdge(H, t, i); //take into account the symmetry
            if (H[s][i].Ex) DeleteEdge(H, s, i);
        }

        for (int i = 0; i<H.size(); i++) { // filling
            if (F[i].Ex && i != s) {
                H[s][i] = F[i];
                if (i != t)  H[i][s] = F[i]; // if do not fill another string
            }
            if (G[i].Ex && i != t) {
                H[t][i] = G[i];
                if (i != s) H[i][t] = G[i];
            }
        }
    }
}

Edge PenduntReduction(vector<vector<Edge>> &H, Edge F, int q, bool find) {
    Pendunt++;
    vector<int> nodepower = GetNodePower(H);
    if (find) nodepower.erase(nodepower.begin() + q); // in PenduntReduction we not consider Pendunt node 0 || 1

    vector <int>::iterator it, smallest = nodepower.end() - 1;
    for (it = nodepower.begin(); it<nodepower.end(); ++it) if (*it == 1) smallest = it;
    int r = smallest - nodepower.begin(); // find Pendunt node, can be 0,1

    if (*smallest == 1 && H.size() > 2) { //otherwise all will be deleted
        for (int i = H[r].size() - 1; i >= 0; i--) if (H[r][i].Ex) {
                //find incident Edge to Pendunt node, can't be 0,1 || 1,0
                if ((r == 1 && i == 0) || (r == 0 && i == 1)) { // that do not have loop
                    q = r;
                    find = true;
                }
                if (r != 0 && r != 1) DeleteNode(H, r);
                if ((r == 1 && i != 0) || (r == 0 && i != 1)) {
                    F = F * H[r][i];
                    DeleteNode(H, r);
                    i--; // after delenode not forget about i
                    if (i != 1) Renumerate(H, i, 1); // always must be 1
                }
                break; // arise the eror if abort this
            }
        return PenduntReduction(H, F, q, find);
    }
    return F;

}

bool BridgeReduction(vector<vector<Edge>> &H) {
    Bridges++;
    if (Visited.size() != H.size()) {
        throw "Eror BridgeReduction!";
    }

    if (Visited[0] && Visited[1]) { // only one of this cases exist
        for (int i = H.size() - 1; i >= 0; i--) if (!Visited[i]) {  // when end is 0,0 make mistake if start from 0
                DeleteNode(H, i);
            }
    }

    if (!Visited[0] && !Visited[1]) {
        for (int i = H.size() - 1; i >= 0; i--) if (Visited[i]) {
                DeleteNode(H, i);
            }
    }

    if ((Visited[0] && !Visited[1]) || (!Visited[0] && Visited[1])) {
        return false;
    }

    return true;
}

Edge GetAllowingEdge(vector<vector<Edge>> &H) {
    Edge F;
    vector<int> nodepower = GetNodePower(H);
    vector<int>::iterator it, smallest = nodepower.end() - 1;

    for (it = nodepower.begin(); it<nodepower.end(); ++it)
        if (*it <= *smallest && (it - nodepower.begin() != 0) && (it - nodepower.begin() != 1)) smallest = it;
    //becouse after DeleteEdge we can get unconnected graph in case nodepower=1
    int r = smallest - nodepower.begin(); //can't be 0,1

    for (int i = H[r].size() - 1; i >= 0; i--) if (H[r][i].Ex) {  //find incident Edge selected node, can be 0,1
            F = H[r][i];
            F.Node1 = r; // Becouse we use only matrix coefficients
            F.Node2 = i;
            break;
        }
    return F;
}
// existence of element in the Visited
bool ElementInside(vector<int> P, int x) {
    if (P.empty()) return false;
    else for (int i = 0; i<P.size(); i++) if (x == P[i]) return true;
    return false;
}

vector<int> GetChain(vector<vector<Edge>> &H, vector<int> &V) {
    vector<int> nodepower = GetNodePower(H);
    vector<int> P; // chain nodes Visited

    bool find = false;
    int i = 0;
    while (!find && i<H.size()) {
        for (int j = 0; j<H[i].size(); j++) {
            if (H[i][j].Ex && !(ElementInside(V, i) && ElementInside(V, j)) && (nodepower[j] == 2 || nodepower[i] == 2)) {
                // find node Power = 2 and add nodes of Edge where is selected node
                P.push_back(i);
                P.push_back(j);
                find = true;
                break;
            }
        }
        i++;
    }

    if (!P.empty()) { // expand Visited in both sides if it is not empty
        i = P.front();
        while (nodepower[i] == 2 && i != P.back())
            // find node incident to left node of our chain and add it
            for (int j = 0; j<H[i].size(); j++) if (H[i][j].Ex && j != P[1]) {
                    if (j != P.back()) P.insert(P.begin(), j);
                    i = j;
                    break;
                }
        i = P.back();
        while (nodepower[i] == 2 && i != P.front())
            for (int j = 0; j<H[i].size(); j++) if (H[i][j].Ex && j != P[P.size() - 2]) {
                    if (j != P.front()) P.push_back(j);
                    i = j;
                    break;
                }

        return P;
    }
    return P;
}

vector<int> ChainReduction(vector<vector<Edge>> &H, vector<int> Ch, vector<int> V, bool find) {
    vector<int> P = GetChain(H, V);
    int count = 0;
    if (!P.empty()) {
        for (int i = 0; i<P.size(); i++) if (P[i] == 0 || P[i] == 1) count++; // count selected nodes in chain

        vector<int>::iterator it, r = P.begin();
        if (count == 1) {
            for (it = P.begin(); it<P.end(); ++it) if (*it == 0 || *it == 1) r = it;
            if ((r == P.begin() || r == P.end() - 1) && !Ch.empty()) count = 0; // reduct special chains
        }

        if ((count == 1 || count == 2) && !find) {
            if (count == 2) { // have only chain with count=2
                find = true;
                V = P;
            }
            if (count == 1) {
                if (!Ch.empty()) {
                    for (it = Ch.begin(); it<Ch.end() - 1; ++it) if (*it == 0 || *it == 1) r = it;  // Ch.back()=count
                    if (r == Ch.begin() || r == Ch.end() - 2) V.clear(); // next recursive call reduct Ch
                }
                for (int j = 0; j<P.size(); j++) if (!ElementInside(V, P[j])) V.push_back(P[j]);
            }

            P.push_back(count);
            Ch = P;

            return ChainReduction(H, Ch, V, find);
        }
        else count = 0; // to reduct chain(s) when we find count=2

        if (count == 0) { // Replace chain by Edge
            Chr++;
            Edge newedge;
            newedge.C.push_back(1);
            for (int j = 0; j<P.size() - 1; j++) newedge = newedge * H[P[j]][P[j + 1]];
            for (int j = 1; j<P.size() - 1; j++) {
                DeleteNode(H, P[j]); // delete only inner nodes of chain
                for (int k = 0; k<P.size(); k++) if (P[j] < P[k]) P[k]--; // not forget about P
                for (int k = 0; k<Ch.size(); k++) if (P[j] < Ch[k]) Ch[k]--; // not forget about Ch
                for (int k = 0; k<V.size(); k++) if (P[j] < V[k]) V[k]--; // not forget about Visited
            }
            int x = P.front(), y = P.back();

            if (H[x][y].Ex) { // Becouse newedge.Power != 1
                H[x][y] = H[x][y] + newedge - H[x][y] * newedge;
                H[y][x] = H[x][y];
            }
            else {
                H[x][y] = newedge;
                H[y][x] = newedge;
            }

            if (!Ch.empty()) { // expand Visited in both sides if it is not empty
                Ch.pop_back();
                vector<int> nodepower = GetNodePower(H);

                int i = Ch.front();
                while (nodepower[i] == 2 && i != Ch.back())
                    // find node incident to left node of our chain and add it
                    for (int j = 0; j<H[i].size(); j++) if (H[i][j].Ex && j != Ch[1]) {
                            if (j != Ch.back()) {
                                Ch.insert(Ch.begin(), j);
                                if (!ElementInside(V, j)) V.push_back(j);
                            }
                            i = j;
                            break;
                        }
                i = Ch.back();
                while (nodepower[i] == 2 && i != Ch.front())
                    for (int j = 0; j<H[i].size(); j++) if (H[i][j].Ex && j != Ch[Ch.size() - 2]) {
                            if (j != Ch.front()) {
                                Ch.push_back(j);
                                if (!ElementInside(V, j)) V.push_back(j);
                            }
                            i = j;
                            break;
                        }

                count = 0;
                for (int i = 0; i<Ch.size(); i++) if (Ch[i] == 0 || Ch[i] == 1) count++; // count selected nodes in chain

                Ch.push_back(count);
            }
            return ChainReduction(H, Ch, V, find);
        }
    }
    return Ch;
}

Edge SimpleCase(const vector<vector<Edge>> &H, const Edge &F) {
    if (H.size() == 2) {
        Num2++;
        return F * H[0][1];
    }
    if (H.size() == 3) {
        Num3++;
        return F * (H[0][1] + H[1][2] * H[0][2] - H[0][1] * H[1][2] * H[0][2]);
    }
    if (H.size() == 4) {
        Num4++;
        return F *
               (H[0][1] + ~H[0][1] * (H[1][2] * H[0][2] + H[0][3] * H[1][3] - H[1][2] * H[0][2] * H[0][3] * H[1][3] +
                                      H[1][2] * H[2][3] * H[0][3] * ~H[1][3] * ~H[0][2] +
                                      ~H[1][2] * H[2][3] * ~H[0][3] * H[1][3] * H[0][2]));
    }

    if (H.size() == 5) {
        Num5++;
        return F * (~(~H[0][1] * ~H[0][2] * (H[1][2] * ~H[1][4] * (H[0][4] * ~H[0][3] * ~H[2][4] *
                                                                   (~H[3][4] + ~H[1][3] * ~H[2][3] * H[3][4]) +
                                                                   H[0][3] * ~H[1][3] * ~H[2][3] *
                                                                   (~H[2][4] + ~H[0][4] * ~H[3][4] * H[2][4])) +
                                             ~H[0][3] * ~H[0][4] * ~(~H[1][2] * ~H[1][3] * ~H[1][4])) +
                      ~H[0][1] * ~H[1][3] * (H[1][4] * ~H[0][4] * (H[0][3] * ~H[0][2] * ~H[3][4] *
                                                                   (~H[2][3] + ~H[1][2] * ~H[2][4] * H[2][3]) +
                                                                   H[0][2] * ~H[1][2] * ~H[2][4] *
                                                                   (~H[3][4] + ~H[0][3] * ~H[2][3] * H[3][4])) +
                                             ~H[1][2] * ~H[1][4]) +
                      H[1][3] * ~H[0][1] * ~H[0][3] * ~H[1][2] *
                      (H[0][2] * ~H[0][4] * ~H[2][3] * (~H[2][4] + ~H[1][4] * ~H[3][4] * H[2][4]) +
                       H[0][4] * ~H[1][4] * ~H[3][4] * (~H[2][3] + ~H[0][2] * ~H[2][4] * H[2][3]))));
    }
}

Edge PairwiseConnectivity(vector<vector<Edge>> &H, Edge F, bool connected, int bufSize, int vectorSize) {
    Factors++;
    if (!connected) {
        Edge N;
        N.Ex = false;
        if (!BridgeReduction(H)) {
            Num0++;
            return N;
        }
        connected = true;
    }

    if (H.size() < 6) {
        return SimpleCase(H, F);
    }

    F = PenduntReduction(H, F, 0, false);

    vector<int> Chain, V;
    Chain = ChainReduction(H, Chain, V, false);
    int count = 0;
    if (!Chain.empty()) {
        count = Chain.back();
        Chain.pop_back();
    }

    if (H.size() < 6) {
        return SimpleCase(H, F);
    }

    if (count == 1) {
        Ch1++;
        int s, t;
        bool terminal = false;
        vector<int> Ch(Chain);
        vector<int>::iterator it, r;
        int coun = 0;
        for (it = Ch.begin(); it < Ch.end(); ++it) if (*it == 0 || *it == 1) r = it;
        int q = r - Ch.begin(); // place of pivote node t=(0 or 1) into Visited Ch
        if (r == Ch.begin() || r == Ch.end() - 1) terminal = true;
        s = *r;
        s == 0 ? t = 1 : t = 0; // need for case terminal

        Edge P, L;
        P.C.push_back(1);
        L.C.push_back(1);
        vector<vector<Edge>> G(H);
        for (int i = 0; i < q; i++) L = L * G[Ch[i]][Ch[i + 1]];
        for (int i = q; i < Ch.size() - 1; i++) P = P * G[Ch[i]][Ch[i + 1]];
        Edge T = P * L;

        for (int i = 1; i < Ch.size() - 1; i++) { // after this we get 2 nodes from chain, one of them can be pivote
            DeleteNode(G, Ch[i]);
            for (int j = 0; j < Ch.size(); j++) if (Ch[i] < Ch[j]) Ch[j]--; // not forget about Ch
        }
        int x = Ch.front(), y = Ch.back(); // matter x<=>y

        vector<vector<Edge>> G1(G);
        bool connec = IsConnected(G);
        vector<bool> visited1(Visited);

        Edge k;
        if (q != 0) {
            (terminal) ? Renumerate(G, y, t) : Renumerate(G, y, 1);  // s=0||1, becouse we don't delete pivote node, after delete: s - node out of chain always 0, so t=1

            int just_value, help_processor = 0;
            if (H.size() >= MAX_DIMENSIONAL) {
                MPI_Status status;
                MPI_Send(&just_value, 0, MPI_INT, HOST_PROCESSOR, I_NEED_HELP_TAG, MPI_COMM_WORLD); // ask for help
                MPI_Recv(&help_processor, 1, MPI_INT, HOST_PROCESSOR, I_NEED_HELP_TAG, MPI_COMM_WORLD, &status); // get answer
            }

            if (help_processor != 0) {
                Str Gp;
                Gp.Graph = G;
                Gp.F.push_back(F*(P - T)); // can't be 0 or 1
                Gp.Ex = connec;
                Gp.Visited = Visited;
                int strBufSize = Gp.F.size()*bufSize + sizeof(int) + sizeof(bool);
                if (!Gp.Ex) strBufSize += Gp.Graph.size() * sizeof(bool);
                for (int i = 1; i < Gp.Graph.size(); i++) strBufSize += i * bufSize + sizeof(int);
                auto buf_str = PackData(Gp, strBufSize, bufSize, vectorSize);
                MPI_Send(buf_str, strBufSize, MPI_PACKED, help_processor, GP_INFO_TAG, MPI_COMM_WORLD);
                delete[] buf_str;
            }
            else k = PairwiseConnectivity(G, F * (P - T), connec, bufSize, vectorSize); // Rsy = k
        }

        Edge w;
        if (q != Ch.size() - 1) {
            Visited = visited1; // same size so all right
            G = G1;
            (terminal) ? Renumerate(G, x, t) : Renumerate(G, x, 1);

            int justValue, helpProcessor = 0;
            if (H.size() >= MAX_DIMENSIONAL) {
                MPI_Status status;
                MPI_Send(&justValue, 0, MPI_INT, HOST_PROCESSOR, I_NEED_HELP_TAG, MPI_COMM_WORLD); // ask for help
                MPI_Recv(&helpProcessor, 1, MPI_INT, HOST_PROCESSOR, I_NEED_HELP_TAG, MPI_COMM_WORLD, &status); // get answer
            }

            if (helpProcessor != 0) {
                Str Gp;
                Gp.Graph = G;
                Gp.F.push_back(F*(L - T)); // can't be 0 or 1
                Gp.Ex = connec;
                Gp.Visited = Visited;
                int str_buf_size = Gp.F.size()*bufSize + sizeof(int) + sizeof(bool);
                if (!Gp.Ex) str_buf_size += Gp.Graph.size() * sizeof(bool);
                for (int i = 1; i < Gp.Graph.size(); i++) str_buf_size += i * bufSize + sizeof(int);
                char *buf_str = PackData(Gp, str_buf_size, bufSize, vectorSize);
                MPI_Send(buf_str, str_buf_size, MPI_PACKED, helpProcessor, GP_INFO_TAG, MPI_COMM_WORLD);
                delete[] buf_str;
            }
            else w = PairwiseConnectivity(G, F * (L - T), connec, bufSize, vectorSize); // Rsx = w
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

        Edge m;

        if (!connec) {
            Visited = visited1;
            int component1Size = 0;
            for (int i = 0; i < Visited.size(); i++) if (Visited[i]) component1Size++;
            int component2_size = Visited.size() - component1Size;

            if (component1Size != 1 && component2_size != 1) {
                vector<vector<Edge>> J(G1);

                if (!terminal) s = 0; // else s = s
                if ((Visited[s] && Visited[x]) || (!Visited[s] && !Visited[x]))
                    (terminal) ? Renumerate(G, x, t) : Renumerate(G, x, 1);
                else
                    (terminal) ? Renumerate(G, y, t) : Renumerate(G, y, 1);

                m = PairwiseConnectivity(J, F * T, connec, bufSize, vectorSize); // dont *F twice
            }
            else m = PairwiseConnectivity(G, F * T, connected, bufSize, vectorSize);
        }
        else m = PairwiseConnectivity(G, F * T, connected, bufSize, vectorSize);

        return k + w + m;
    }
    if (count == 2) {
        Ch2++;
        vector<int> Ch(Chain);
        vector<int>::iterator it, r;
        for (it = Ch.begin(); it < Ch.end(); ++it) if (*it == 0) r = it;
        int q = r - Ch.begin(); // place of pivote node s=0 into Visited Ch
        for (it = Ch.begin(); it < Ch.end(); ++it) if (*it == 1) r = it;
        int w = r - Ch.begin(); // place of pivote node t=1 into Visited Ch
        if (q > w) { // that * edges in chain, q<w always
            int u = q;
            q = w;
            w = u;
        }

        Edge P, L;
        P.C.push_back(1);
        L.C.push_back(1);
        vector<vector<Edge>> G(H);
        for (int i = 0; i < q; i++) L = L * G[Ch[i]][Ch[i + 1]];
        for (int i = q; i < w; i++) P = P * G[Ch[i]][Ch[i + 1]];
        for (int i = w; i < Ch.size() - 1; i++) L = L * G[Ch[i]][Ch[i + 1]];
        for (int i = 1; i < Ch.size() - 1; i++) {
            DeleteNode(G, Ch[i]);
            for (int j = 0; j < Ch.size(); j++) if (Ch[i] < Ch[j]) Ch[j]--; // not forget about Ch
        }
        int x = Ch.front(), y = Ch.back(); // doesn't matter x<=>y

        bool connec = IsConnected(G);

        if (x != 0 && x != 1 && y != 0 && y != 1) {
            Renumerate(G, x, 0);
            Renumerate(G, y, 1);
        }
        if (x == 0 && y != 1) Renumerate(G, y, 1);
        if (x == 1 && y != 0) Renumerate(G, y, 0);
        if (y == 0 && x != 1) Renumerate(G, x, 1);
        if (y == 1 && x != 0) Renumerate(G, x, 0);

        Edge n = F * P;
        n = n + PairwiseConnectivity(G, F * (L - P * L), connec, bufSize, vectorSize); // Rxy = n

        return n;
    }

    Edge W = GetAllowingEdge(H);
    if ((W.Node1 == 1 && W.Node2 == 0) || (W.Node1 == 0 && W.Node2 == 1)) {
        throw "Eror in Allowing Edge";
    }
    if (W.Node1 == 1 || W.Node1 == 0) {
        throw "Eror in Allowing Edge";
    }

    vector<vector<Edge>> H1(H.size());
    vector<vector<Edge>> H2(H.size());
    for (int i = 0; i < H.size(); i++) { // Copy H
        H1[i] = H[i];
        H2[i] = H[i];
    }
    Edge F1, F2;
    if (W.Simple == 0 && W.Power == 1) {
        F1 = F;
        F1.Power++;
        F2 = F;
        F2.Power++;
        F2.C.insert(F2.C.begin(), 0);
        F2.C.pop_back();
    }
    else {
        F1 = F * W;
        F2 = F * ~W;
    }

    DeleteEdge(H2, W.Node1, W.Node2);
    ContractEdge(H1, W.Node1, W.Node2);

    bool connec = IsConnected(H2);
    vector<bool> visited1(Visited);

    if (!connec) {
        Visited = visited1;
        int component1_size = 0;
        for (int i = 0; i < Visited.size(); i++) if (Visited[i]) component1_size++;
        int component2_size = Visited.size() - component1_size;

        if (component1_size != 1 && component2_size != 1) {
            vector<vector<Edge>> J1(H2);
            vector<vector<Edge>> J2(H2);
            Edge F3 = Bin.front();

            Edge m = PairwiseConnectivity(H2, F2, connec, bufSize, vectorSize); // use Visited

            if (W.Node2 != 0 && W.Node2 != 1 && ((Visited[0] && !Visited[1]) || (!Visited[0] && Visited[1]))) {
                Visited = visited1;
                if ((Visited[W.Node1] && Visited[0]) || (!Visited[W.Node1] && !Visited[0])) Renumerate(J1, 1, W.Node1);
                else Renumerate(J1, 1, W.Node2);
                Edge k = PairwiseConnectivity(J1, F3, connec, bufSize, vectorSize);

                Visited = visited1;
                if ((Visited[W.Node1] && Visited[1]) || (!Visited[W.Node1] && !Visited[1])) Renumerate(J2, 0, W.Node1);
                else Renumerate(J2, 0, W.Node2);
                Edge w = PairwiseConnectivity(J2, F3, connec, bufSize, vectorSize);

                return F1 * k*w + m; // dont *F1 twice
            }
            if (W.Node2 != 0 && W.Node2 != 1 && ((Visited[0] && Visited[1]) || (!Visited[0] && !Visited[1]))) {
                Visited = visited1;
                Edge k = PairwiseConnectivity(J1, F1, connec, bufSize, vectorSize);

                return k + m;
            }
            if (W.Node2 == 0 || W.Node2 == 1) { // if 0 and 1 in one component nothing renum
                Visited = visited1;
                if (W.Node2 == 0 && ((Visited[0] && !Visited[1]) || (!Visited[0] && Visited[1])))
                    Renumerate(J1, 0, W.Node1); // becouse can't return 0
                if (W.Node2 == 1 && ((Visited[0] && !Visited[1]) || (!Visited[0] && Visited[1])))
                    Renumerate(J1, 1, W.Node1);
                Edge k = PairwiseConnectivity(J1, F1, connec, bufSize, vectorSize);

                return k + m;
            }
        }
        else return PairwiseConnectivity(H1, F1, connected, bufSize, vectorSize) +
                    PairwiseConnectivity(H2, F2, connec, bufSize, vectorSize);
    }
    else {
        int justValue, helpProcessor = 0;
        if (H.size() >= MAX_DIMENSIONAL) {
            MPI_Status status;
            MPI_Send(&justValue, 0, MPI_INT, HOST_PROCESSOR, I_NEED_HELP_TAG, MPI_COMM_WORLD); // ask for help
            MPI_Recv(&helpProcessor, 1, MPI_INT, HOST_PROCESSOR, I_NEED_HELP_TAG, MPI_COMM_WORLD, &status); // get answer
        }

        if (helpProcessor != 0) { // send half work
            Str Gp;
            Gp.Graph = H1;
            Gp.F.push_back(F1); // can't be 0 or 1
            Gp.Ex = connected;
            Gp.Visited = Visited;
            int str_buf_size = Gp.F.size()*bufSize + sizeof(int) + sizeof(bool);
            if (!Gp.Ex) str_buf_size += Gp.Graph.size() * sizeof(bool);
            for (int i = 1; i < Gp.Graph.size(); i++) str_buf_size += i * bufSize + sizeof(int);
            char *buf_str = PackData(Gp, str_buf_size, bufSize, vectorSize);
            MPI_Send(buf_str, str_buf_size, MPI_PACKED, helpProcessor, GP_INFO_TAG, MPI_COMM_WORLD);
            delete[] buf_str;
        }
        else return PairwiseConnectivity(H1, F1, connected, bufSize, vectorSize) +
                    PairwiseConnectivity(H2, F2, connec, bufSize, vectorSize);

        return PairwiseConnectivity(H2, F2, connec, bufSize, vectorSize); // continue with graph wehere deleted
    }
}