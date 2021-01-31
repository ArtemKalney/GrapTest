#include "stdafx.h"
#include "funcs.h"
#include "globals.h"

void ChainReduction1(vector<vector<Edge>> &H, vector<int> V, bool connected, int bufSize, int vectorSize,
                     int size) {
    vector<int> Chain = GetChain(H, V);
    for (int i = 0; i<Chain.size(); i++) if (!ElementInside(V, Chain[i])) V.push_back(Chain[i]);

    if (!Chain.empty()) {
        Chrs++;
        vector<int>::iterator largest = max_element(Chain.begin(), Chain.end());

        for (int i = 0; i<*largest; i++) if (!ElementInside(Chain, i)) {
                Edge T;
                T.C.push_back(1);
                vector<int> Ch(Chain);
                vector<vector<Edge>> G(H);
                for (int j = 0; j<Ch.size() - 1; j++) T = T * G[Ch[j]][Ch[j + 1]];

                Edge F1, F2;
                int F3 = 0;
                for (int j = 0; j<Ch.size() - 1; j++) if (Ch[j] > i && Mask1[i][Ch[j]]) { // j=Ch.size() - 1 doesn't make anything
                        Edge L; // save Power,Simple, but lost P.C
                        L.C.push_back(1);
                        for (int k = 0; k<j; k++) L = L * G[Ch[k]][Ch[k + 1]];
                        F1 = F1 + (L - T);
                    }
                for (int j = 1; j<Ch.size(); j++) if (Ch[j] > i && Mask1[i][Ch[j]]) { // j=0 doesn't make anything
                        Edge P;
                        P.C.push_back(1);
                        for (int k = j; k<Ch.size() - 1; k++) P = P * G[Ch[k]][Ch[k + 1]];
                        F2 = F2 + (P - T);
                    }

                for (int j = 0; j<Ch.size(); j++) if (Ch[j] > i && Mask1[i][Ch[j]]) F3++;

                for (int j = 0; j<Ch.size(); j++) if (Ch[j] > i && Mask1[i][Ch[j]]) {
                        Mask1[i][Ch[j]] = false;
                        Mask1[Ch[j]][i] = false;
                    }

                int s = i;
                for (int j = 1; j<Ch.size() - 1; j++) { // after this we get 2 nodes from chain, one of them can be pivote
                    DeleteNode(G, Ch[j]);
                    if (Ch[j] < s) s--; // not forget about s
                    for (int k = 0; k<Ch.size(); k++) if (Ch[j] < Ch[k]) Ch[k]--; // not forget about Ch
                }
                int x = Ch.front(), y = Ch.back(); // matter x<=>y

                vector<vector<Edge>> G1(G);
                int s1 = s;
                bool connec = IsConnected(G);
                vector<bool> visited1(Visited);

                if (!F2.C.empty()) {
                    if (s != 0 && s != 1 && y != 0 && y != 1) {
                        Renumerate(G, s, 0);
                        Renumerate(G, y, 1);
                    }
                    if (s == 0 && y != 1) Renumerate(G, y, 1);
                    if (s == 1 && y != 0) Renumerate(G, y, 0);
                    if (y == 0 && s != 1) Renumerate(G, s, 1);
                    if (y == 1 && s != 0) Renumerate(G, s, 0);

                    Str Gp;
                    Gp.Graph = G;
                    Gp.F.push_back(F2);
                    Gp.Ex = connec;
                    Gp.Visited = Visited;
                    SendControl(Gp, bufSize, vectorSize, size); // Rsy
                }

                if (!F1.C.empty()) {
                    Visited = visited1;
                    G = G1;

                    if (s != 0 && s != 1 && x != 0 && x != 1) {
                        Renumerate(G, s, 0);
                        Renumerate(G, x, 1);
                    }
                    if (s == 0 && x != 1) Renumerate(G, x, 1);
                    if (s == 1 && x != 0) Renumerate(G, x, 0);
                    if (x == 0 && s != 1) Renumerate(G, s, 1);
                    if (x == 1 && s != 0) Renumerate(G, s, 0);

                    Str Gp;
                    Gp.Graph = G;
                    Gp.F.push_back(F1);
                    Gp.Ex = connec;
                    Gp.Visited = Visited;
                    SendControl(Gp, bufSize, vectorSize, size); // Rsx
                }

                G = G1;
                int xy = x;
                if (y < xy) xy--; // not forget
                if (y < s) s--; // not forget
                ContractEdge(G, y, x); // x or y can't be s
                if (s != 0 && s != 1 && xy != 0 && xy != 1) {
                    Renumerate(G, s, 0);
                    Renumerate(G, xy, 1);
                }
                if (s == 0 && xy != 1) Renumerate(G, xy, 1);
                if (s == 1 && xy != 0) Renumerate(G, xy, 0);
                if (xy == 0 && s != 1) Renumerate(G, s, 1);
                if (xy == 1 && s != 0) Renumerate(G, s, 0);
                Edge m;

                if (!connec) {
                    Visited = visited1;
                    int component1_size = 0;
                    for (int i = 0; i < Visited.size(); i++) if (Visited[i]) component1_size++;
                    int component2_size = Visited.size() - component1_size;

                    if (component1_size != 1 && component2_size != 1) {
                        Decomp1++;
                        vector<vector<Edge>> J(G1);

                        s = s1;
                        if ((Visited[s] && Visited[x]) || (!Visited[s] && !Visited[x])) {
                            if (s != 0 && s != 1 && x != 0 && x != 1) {
                                Renumerate(J, s, 0);
                                Renumerate(J, x, 1);
                            }
                            if (s == 0 && x != 1) Renumerate(J, x, 1);
                            if (s == 1 && x != 0) Renumerate(J, x, 0);
                            if (x == 0 && s != 1) Renumerate(J, s, 1);
                            if (x == 1 && s != 0) Renumerate(J, s, 0);
                        }
                        else {
                            if (s != 0 && s != 1 && y != 0 && y != 1) {
                                Renumerate(J, s, 0);
                                Renumerate(J, y, 1);
                            }
                            if (s == 0 && y != 1) Renumerate(J, y, 1);
                            if (s == 1 && y != 0) Renumerate(J, y, 0);
                            if (y == 0 && s != 1) Renumerate(J, s, 1);
                            if (y == 1 && s != 0) Renumerate(J, s, 0);
                        }

                        Str Gp;
                        Gp.Graph = J;
                        Gp.F.push_back(F3*T); // can't be 0, F3>=1
                        Gp.Ex = connec;
                        Gp.Visited = Visited;
                        SendControl(Gp, bufSize, vectorSize, size); // Rs,xy
                    }
                    else {
                        Str Gp;
                        Gp.Graph = G;
                        Gp.F.push_back(F3*T); // can't be 0, F3>=1
                        Gp.Ex = connected;
                        Gp.Visited = Visited;
                        SendControl(Gp, bufSize, vectorSize, size); // Rs,xy
                    }
                }
                else {
                    Str Gp;
                    Gp.Graph = G;
                    Gp.F.push_back(F3*T); // can't be 0, F3>=1
                    Gp.Ex = connected;
                    Gp.Visited = Visited;
                    SendControl(Gp, bufSize, vectorSize, size); // Rs,xy
                }
            }
            else {
                vector<int> Ch(Chain);
                vector<int>::iterator it, r;
                for (it = Ch.begin(); it<Ch.end(); ++it) if (*it == i) r = it;
                int q = r - Ch.begin(); // place of pivote node s=i into Visited Ch

                Edge T;
                T.C.push_back(1);
                vector<vector<Edge>> G(H);
                for (int j = 0; j<Ch.size() - 1; j++) T = T * G[Ch[j]][Ch[j + 1]];

                Edge F1, F2;
                int F3 = 0;
                for (int j = 0; j<Ch.size(); j++) if (Ch[j] > i && Mask1[i][Ch[j]]) {
                        Edge P; // save Power,Simple, but lost P.C
                        P.C.push_back(1);
                        if (j < q) for (int k = j; k<q; k++) P = P * G[Ch[k]][Ch[k + 1]];
                        else for (int k = q; k<j; k++) P = P * G[Ch[k]][Ch[k + 1]];
                        F1 = F1 + P;
                    }
                for (int j = 0; j<Ch.size(); j++) if (Ch[j] > i && Mask1[i][Ch[j]]) {
                        Edge L;
                        L.C.push_back(1);
                        if (j < q) {
                            for (int k = 0; k<j; k++) L = L * G[Ch[k]][Ch[k + 1]];
                            for (int k = q; k<Ch.size() - 1; k++) L = L * G[Ch[k]][Ch[k + 1]];
                        }
                        else {
                            for (int k = 0; k<q; k++) L = L * G[Ch[k]][Ch[k + 1]];
                            for (int k = j; k<Ch.size() - 1; k++) L = L * G[Ch[k]][Ch[k + 1]];
                        }
                        F2 = F2 + L;
                    }

                for (int j = 0; j<Ch.size(); j++) if (Ch[j] > i && Mask1[i][Ch[j]]) F3++;

                for (int j = 0; j<Ch.size(); j++) if (Ch[j] > i && Mask1[i][Ch[j]]) {
                        Mask1[i][Ch[j]] = false;
                        Mask1[Ch[j]][i] = false;
                    }

                for (int j = 1; j<Ch.size() - 1; j++) {
                    DeleteNode(G, Ch[j]);
                    for (int k = 0; k<Ch.size(); k++) if (Ch[j] < Ch[k]) Ch[k]--; // not forget about Ch
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

                Str Gp;
                Gp.Graph = G;
                Gp.F.push_back(F2 - F3 * T); // can't be 0 or 1, F3>=1
                Gp.Ex = connec;
                Gp.Visited = Visited;
                SendControl(Gp, bufSize, vectorSize, size); // Rxy

                Sum = Sum + F1;
            }
        ChainReduction1(H, V, connected, bufSize, vectorSize, size);
    }
}