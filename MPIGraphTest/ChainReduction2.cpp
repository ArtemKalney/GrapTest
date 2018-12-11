#include "stdafx.h"
#include "funcs.h"
#include "globals.h"

void ChainReduction2(vector<vector<Edge>> &H, vector<int> weight, bool connected, int bufSize, int vectorSize,
                     int size) {
    vector<int> V; // considered nodes
    for (int i = 0; i<Mask2.size(); i++) if (!Mask2[i]) V.push_back(i);
    vector<int> C = GetChain(H, V);

    if (!C.empty()) {
        Chrs++;
        for (int i = 0; i<C.size(); i++) if (!ElementInside(V, C[i])) Mask2[C[i]] = false;

        if (!ElementInside(C, 0)) {
            Edge T;
            T.C.push_back(1);
            vector<int> Ch = C;
            vector<vector<Edge>> G(H.size());
            for (int i = 0; i<H.size(); i++) G[i] = H[i];
            for (int i = 0; i<Ch.size() - 1; i++) T = T * G[Ch[i]][Ch[i + 1]];

            Edge F1, F2;
            int F3 = 0;
            for (int i = 0; i<Ch.size() - 1; i++) if (!ElementInside(V, Ch[i])) { // i=Ch.size() - 1 doesn't make anything
                    Edge L; // save Power,Simple, but lost P.C
                    L.C.push_back(1);
                    for (int j = 0; j<i; j++) L = L * G[Ch[j]][Ch[j + 1]];
                    F1 = F1 + weight[Ch[i]] * (L - T); // here arise size=1 - Edge
                }
            for (int i = 1; i<Ch.size(); i++) if (!ElementInside(V, Ch[i])) { // i=0 doesn't make anything
                    Edge P;
                    P.C.push_back(1);
                    for (int j = i; j<Ch.size() - 1; j++) P = P * G[Ch[j]][Ch[j + 1]];
                    F2 = F2 + weight[Ch[i]] * (P - T);
                }

            for (int i = 0; i<Ch.size(); i++) if (!ElementInside(V, Ch[i])) F3 += weight[Ch[i]];

            for (int i = 1; i<Ch.size() - 1; i++) { // after this we get 2 nodes from chain, one of them can be pivote
                DeleteNode(G, Ch[i]);
                for (int j = 0; j<Ch.size(); j++) if (Ch[i] < Ch[j]) Ch[j]--; // not forget about Ch
            }
            int x = Ch.front(), y = Ch.back(); // matter x<=>y

            vector<vector<Edge>> G1(G.size());
            for (int i = 0; i<G.size(); i++) G1[i] = G[i]; // after reduction Graph changed
            bool connec = IsConnected(G);
            vector<bool> visited1 = Visited;

            if (!F2.C.empty()) {
                Renumerate(G, y, 1);  // s - node out of chain always 0, so t=1

                Str Gp;
                Gp.Graph = G;
                Gp.F.push_back(F2);
                Gp.Ex = connec;
                Gp.Visited = Visited;
                SendControl(Gp, bufSize, vectorSize, size);
            } // R1y

            if (!F1.C.empty()) {
                Visited = visited1;
                G.resize(G1.size());
                for (int i = 0; i<G1.size(); i++) G[i] = G1[i];
                Renumerate(G, x, 1);

                Str Gp;
                Gp.Graph = G;
                Gp.F.push_back(F1);
                Gp.Ex = connec;
                Gp.Visited = Visited;
                SendControl(Gp, bufSize, vectorSize, size);
            } // R1x

            G.resize(G1.size());
            for (int i = 0; i<G1.size(); i++) G[i] = G1[i];
            int xy = x;
            if (y < xy) xy--; // not forget
            ContractEdge(G, y, x); // x or y can't be s

            if (xy != 1) Renumerate(G, xy, 1); // 0 can not be in Ch

            Edge m;

            if (!connec) {
                Visited = visited1;
                int component1_size = 0;
                for (int i = 0; i < Visited.size(); i++) if (Visited[i]) component1_size++;
                int component2_size = Visited.size() - component1_size;

                if (component1_size != 1 && component2_size != 1) {
                    Decomp1++;
                    vector<vector<Edge>> J(G1.size());
                    for (int i = 0; i < G1.size(); i++) J[i] = G1[i];

                    if ((Visited[0] && Visited[x]) || (!Visited[0] && !Visited[x])) Renumerate(J, x, 1);
                    else Renumerate(J, y, 1);

                    Str Gp;
                    Gp.Graph = J;
                    Gp.F.push_back(F3*T);
                    Gp.Ex = connec;
                    Gp.Visited = Visited;
                    SendControl(Gp, bufSize, vectorSize, size); // R1xy
                }
                else {
                    Str Gp;
                    Gp.Graph = G;
                    Gp.F.push_back(F3*T);
                    Gp.Ex = connected;
                    Gp.Visited = Visited;
                    SendControl(Gp, bufSize, vectorSize, size); // R1xy
                }
            }
            else {
                Str Gp;
                Gp.Graph = G;
                Gp.F.push_back(F3*T);
                Gp.Ex = connected;
                Gp.Visited = Visited;
                SendControl(Gp, bufSize, vectorSize, size); // R1xy
            }
        }
        else {
            vector<int> Ch = C;
            vector<int>::iterator it, r;
            for (it = Ch.begin(); it<Ch.end(); ++it) if (*it == 0) r = it;
            int q = r - Ch.begin(); // place of pivote node s=0 into Visited Ch

            Edge T;
            T.C.push_back(1);
            vector<vector<Edge>> G(H.size());
            for (int i = 0; i<H.size(); i++) G[i] = H[i];
            for (int i = 0; i<Ch.size() - 1; i++) T = T * G[Ch[i]][Ch[i + 1]];

            Edge F1, F2;
            int F3 = 0;
            for (int i = 0; i<Ch.size(); i++) if (i != q && !ElementInside(V, Ch[i])) {
                    Edge P; // save Power,Simple, but lost P.C
                    P.C.push_back(1);
                    if (i < q) for (int j = i; j<q; j++) P = P * G[Ch[j]][Ch[j + 1]];
                    else for (int j = q; j<i; j++) P = P * G[Ch[j]][Ch[j + 1]];
                    F1 = F1 + weight[Ch[i]] * P;
                }
            for (int i = 0; i<Ch.size(); i++) if (i != q && !ElementInside(V, Ch[i])) {
                    Edge L;
                    L.C.push_back(1);
                    if (i < q) {
                        for (int j = 0; j < i; j++) L = L * G[Ch[j]][Ch[j + 1]];
                        for (int j = q; j<Ch.size() - 1; j++) L = L * G[Ch[j]][Ch[j + 1]];
                    }
                    else {
                        for (int j = 0; j < q; j++) L = L * G[Ch[j]][Ch[j + 1]];
                        for (int j = i; j<Ch.size() - 1; j++) L = L * G[Ch[j]][Ch[j + 1]];
                    }
                    F2 = F2 + weight[Ch[i]] * L;
                }

            for (int i = 0; i<Ch.size(); i++) if (!ElementInside(V, Ch[i])) F3 += weight[Ch[i]];

            for (int i = 1; i<Ch.size() - 1; i++) {
                DeleteNode(G, Ch[i]);
                for (int j = 0; j<Ch.size(); j++) if (Ch[i] < Ch[j]) Ch[j]--; // not forget about Ch
            }

            bool connec = IsConnected(G);

            int x = Ch.front(), y = Ch.back(); // doesn't matter x<=>y
            if (x != 0 && x != 1 && y != 0 && y != 1) {
                Renumerate(G, x, 0);
                Renumerate(G, y, 1);
            }
            if (x == 0 && y != 1) Renumerate(G, y, 1);
            if (x == 1 && y != 0) Renumerate(G, y, 0);
            if (y == 0 && x != 1) Renumerate(G, x, 1);
            if (y == 1 && x != 0) Renumerate(G, x, 0);

            Sum = Sum + F1;

            Str Gp;
            Gp.Graph = G;
            Gp.F.push_back(F2 - F3 * T);
            Gp.Ex = connec;
            Gp.Visited = Visited;
            SendControl(Gp, bufSize, vectorSize, size);
        }
        ChainReduction2(H, weight, connected, bufSize, vectorSize, size);
    }
}