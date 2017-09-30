#include "stdafx.h"
#include "funcs.h"
#include "globals.h"
#include "Cclass.h"

edge ChainReduction1(vector<vector<edge>> &H, edge sum, vector<int> V, bool connected)
{
    vector<int> Chain = GetChain(H, V);
    for (int i = 0; i<Chain.size(); i++)
        if (!ElementInside(V, Chain[i])) V.push_back(Chain[i]);

    if (!Chain.empty()) {
        chrs++;
        vector<int>::iterator largest = max_element(Chain.begin(), Chain.end());

        for (int i = 0; i<*largest; i++)
            if (!ElementInside(Chain, i)) {
                edge T;
                T.C.push_back(1);
                vector<int> Ch(Chain);
                vector<vector<edge>> G(H);
                for (int j = 0; j<Ch.size() - 1; j++)
                    T = T*G[Ch[j]][Ch[j + 1]];

                edge F1, F2;
                int F3 = 0;
                for (int j = 0; j<Ch.size() - 1; j++)
                    if (Ch[j] > i && mask1[i][Ch[j]]) { // j=Ch.size() - 1 doesn't make anything
                        edge L; // save power,simple, but lost P.C
                        L.C.push_back(1);
                        for (int k = 0; k<j; k++)
                            L = L*G[Ch[k]][Ch[k + 1]];
                        F1 = F1 + (L - T);
                    }

                for (int j = 1; j<Ch.size(); j++)
                    if (Ch[j] > i && mask1[i][Ch[j]]) { // j=0 doesn't make anything
                        edge P;
                        P.C.push_back(1);
                        for (int k = j; k<Ch.size() - 1; k++)
                            P = P*G[Ch[k]][Ch[k + 1]];
                        F2 = F2 + (P - T);
                    }

                for (int j = 0; j<Ch.size(); j++)
                    if (Ch[j] > i && mask1[i][Ch[j]]) F3++;

                for (int j = 0; j<Ch.size(); j++)
                    if (Ch[j] > i && mask1[i][Ch[j]]) {
                        mask1[i][Ch[j]] = false;
                        mask1[Ch[j]][i] = false;
                    }

                int s = i;
                for (int j = 1; j<Ch.size() - 1; j++) { // after this we get 2 nodes from chain, one of them can be pivote
                    DeleteNode(G, Ch[j]);
                    if (Ch[j] < s) s--; // not forget about s
                    for (int k = 0; k<Ch.size(); k++)
                        if (Ch[j] < Ch[k]) Ch[k]--; // not forget about Ch
                }
                int x = Ch.front(), y = Ch.back(); // matter x<=>y

                vector<vector<edge>> G1(G);
                int s1 = s;
                bool connec = ConnectedGraph(G);
                vector<bool> visited1(visited);

                edge k;
                if (!F2.C.empty()) {
                    if (s != 0 && s != 1 && y != 0 && y != 1) {
                        Renumerate(G, s, 0);
                        Renumerate(G, y, 1);
                    }

                    if (s == 0 && y != 1) Renumerate(G, y, 1);
                    if (s == 1 && y != 0) Renumerate(G, y, 0);
                    if (y == 0 && s != 1) Renumerate(G, s, 1);
                    if (y == 1 && s != 0) Renumerate(G, s, 0);
                    k = PairwiseConnectivity(G, F2, connec); // Rsy
                }

                edge w;
                if (!F1.C.empty()) {
                    visited = visited1;
                    G = G1;

                    if (s != 0 && s != 1 && x != 0 && x != 1) {
                        Renumerate(G, s, 0);
                        Renumerate(G, x, 1);
                    }
                    if (s == 0 && x != 1) Renumerate(G, x, 1);
                    if (s == 1 && x != 0) Renumerate(G, x, 0);
                    if (x == 0 && s != 1) Renumerate(G, s, 1);
                    if (x == 1 && s != 0) Renumerate(G, s, 0);
                    w = PairwiseConnectivity(G, F1, connec); // Rsx
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
                edge m;

                if (!connec) {
                    visited = visited1;
                    int component1_size = 0;
                    for (int i = 0; i < visited.size(); i++)
                        if (visited[i]) component1_size++;
                    int component2_size = visited.size() - component1_size;

                    if (component1_size != 1 && component2_size != 1) {
                        decomp1++;
                        if ((visited[x] && visited[y]) || (!visited[x] && !visited[y]))
                            cout << "Eror in decomposition chainreductiom" << endl;

                        vector<vector<edge>> J(G1);
                        s = s1;
                        if ((visited[s] && visited[x]) || (!visited[s] && !visited[x])) {
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

                        m = PairwiseConnectivity(J, F3 * T, connec); // dont *F twice
                    }
                    else m = PairwiseConnectivity(G, F3 * T, connected); // Rs,xy
                }
                else m = PairwiseConnectivity(G, F3 * T, connected); // Rs,xy

                sum = sum + (k + w + m);
            }
            else {
                vector<int> Ch(Chain);
                vector<int>::iterator it, iteratorOfPivoteNode;
                for (it = Ch.begin(); it<Ch.end(); ++it) if (*it == i) iteratorOfPivoteNode = it;
                int placeOfPivoteNodeS = iteratorOfPivoteNode - Ch.begin(); // place of pivote node s=i into vector Ch

                edge T;
                T.C.push_back(1);
                vector<vector<edge>> G(H);
                for (int j = 0; j<Ch.size() - 1; j++)
                    T = T*G[Ch[j]][Ch[j + 1]];

                edge F1, F2;
                int F3 = 0;
                for (int j = 0; j<Ch.size(); j++)
                    if (Ch[j] > i && mask1[i][Ch[j]]) {
                        edge P; // save power,simple, but lost P.C
                        P.C.push_back(1);
                        if (j < placeOfPivoteNodeS)
                            for (int k = j; k<placeOfPivoteNodeS; k++)
                                P = P*G[Ch[k]][Ch[k + 1]];
                        else
                            for (int k = placeOfPivoteNodeS; k<j; k++)
                                P = P*G[Ch[k]][Ch[k + 1]];
                        F1 = F1 + P;
                    }

                for (int j = 0; j<Ch.size(); j++)
                    if (Ch[j] > i && mask1[i][Ch[j]]) {
                        edge L;
                        L.C.push_back(1);
                        if (j < placeOfPivoteNodeS) {
                            for (int k = 0; k<j; k++)
                                L = L*G[Ch[k]][Ch[k + 1]];
                            for (int k = placeOfPivoteNodeS; k<Ch.size() - 1; k++)
                                L = L*G[Ch[k]][Ch[k + 1]];
                        }
                        else {
                            for (int k = 0; k<placeOfPivoteNodeS; k++)
                                L = L*G[Ch[k]][Ch[k + 1]];
                            for (int k = j; k<Ch.size() - 1; k++)
                                L = L*G[Ch[k]][Ch[k + 1]];
                        }

                        F2 = F2 + L;
                    }

                for (int j = 0; j<Ch.size(); j++)
                    if (Ch[j] > i && mask1[i][Ch[j]]) F3++;

                for (int j = 0; j<Ch.size(); j++)
                    if (Ch[j] > i && mask1[i][Ch[j]]) {
                        mask1[i][Ch[j]] = false;
                        mask1[Ch[j]][i] = false;
                    }

                for (int j = 1; j<Ch.size() - 1; j++) {
                    DeleteNode(G, Ch[j]);
                    for (int k = 0; k<Ch.size(); k++) if (Ch[j] < Ch[k]) Ch[k]--; // not forget about Ch
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
                edge n = PairwiseConnectivity(G, F2 - F3 * T, connec); // Rxy

                sum = sum + (F1 + n);
            }
        return ChainReduction1(H, sum, V, connected);
    }

    return sum;
}