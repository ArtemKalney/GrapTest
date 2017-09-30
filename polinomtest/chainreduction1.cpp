#include "stdafx.h"
#include "funcs.h"
#include "globals.h"
#include "Cclass.h"

edge ChainReduction1(vector<vector<edge>> &H, edge sum, vector<int> checkedNodes, bool connected)
{
    vector<int> Chain = GetChain(H, checkedNodes);
    for (int i = 0; i<Chain.size(); i++)
        if (!ElementInside(checkedNodes, Chain[i])) checkedNodes.push_back(Chain[i]);

    if (!Chain.empty()) {
        chrs++;
        vector<int>::iterator largestNumber = max_element(Chain.begin(), Chain.end());

        for (int i = 0; i<*largestNumber; i++)
            if (!ElementInside(Chain, i)) {
                edge T;
                T.C.push_back(1);
                vector<int> Ch(Chain);
                vector<vector<edge>> graph(H);
                for (int j = 0; j<Ch.size() - 1; j++)
                    T = T*graph[Ch[j]][Ch[j + 1]];

                edge F1, F2;
                int F3 = 0;
                for (int j = 0; j<Ch.size() - 1; j++)
                    if (Ch[j] > i && maskApc[i][Ch[j]]) { // j=Ch.size() - 1 doesn't make anything
                        edge L; // save power,simple, but lost P.C
                        L.C.push_back(1);
                        for (int k = 0; k<j; k++)
                            L = L*graph[Ch[k]][Ch[k + 1]];
                        F1 = F1 + (L - T);
                    }

                for (int j = 1; j<Ch.size(); j++)
                    if (Ch[j] > i && maskApc[i][Ch[j]]) { // j=0 doesn't make anything
                        edge P;
                        P.C.push_back(1);
                        for (int k = j; k<Ch.size() - 1; k++)
                            P = P*graph[Ch[k]][Ch[k + 1]];
                        F2 = F2 + (P - T);
                    }

                for (int j = 0; j<Ch.size(); j++)
                    if (Ch[j] > i && maskApc[i][Ch[j]]) F3++;

                for (int j = 0; j<Ch.size(); j++)
                    if (Ch[j] > i && maskApc[i][Ch[j]]) {
                        maskApc[i][Ch[j]] = false;
                        maskApc[Ch[j]][i] = false;
                    }

                int s = i;
                for (int j = 1; j<Ch.size() - 1; j++) { // after this we get 2 nodes from chain, one of them can be pivote
                    DeleteNode(graph, Ch[j]);
                    if (Ch[j] < s) s--; // not forget about s
                    for (int k = 0; k<Ch.size(); k++)
                        if (Ch[j] < Ch[k]) Ch[k]--; // not forget about Ch
                }
                int x = Ch.front(), y = Ch.back(); // matter x<=>y

                vector<vector<edge>> graphWithDeletedChain(graph);
                int sBeforeRenumerate = s;
                bool connectedGraph = ConnectedGraph(graph);
                vector<bool> visitedNodesBeforeRenumerate(visitedNodes);

                edge k;
                if (!F2.C.empty()) {
                    if (s != 0 && s != 1 && y != 0 && y != 1) {
                        Renumerate(graph, s, 0);
                        Renumerate(graph, y, 1);
                    }

                    if (s == 0 && y != 1) Renumerate(graph, y, 1);
                    if (s == 1 && y != 0) Renumerate(graph, y, 0);
                    if (y == 0 && s != 1) Renumerate(graph, s, 1);
                    if (y == 1 && s != 0) Renumerate(graph, s, 0);

                    k = PairwiseConnectivity(graph, F2, connectedGraph); // Rsy
                }

                edge w;
                if (!F1.C.empty()) {
                    visitedNodes = visitedNodesBeforeRenumerate;
                    graph = graphWithDeletedChain;

                    if (s != 0 && s != 1 && x != 0 && x != 1) {
                        Renumerate(graph, s, 0);
                        Renumerate(graph, x, 1);
                    }

                    if (s == 0 && x != 1) Renumerate(graph, x, 1);
                    if (s == 1 && x != 0) Renumerate(graph, x, 0);
                    if (x == 0 && s != 1) Renumerate(graph, s, 1);
                    if (x == 1 && s != 0) Renumerate(graph, s, 0);

                    w = PairwiseConnectivity(graph, F1, connectedGraph); // Rsx
                }

                graph = graphWithDeletedChain;
                int xy = x;
                if (y < xy) xy--; // not forget
                if (y < s) s--; // not forget
                ContractEdge(graph, y, x); // x or y can't be s
                if (s != 0 && s != 1 && xy != 0 && xy != 1) {
                    Renumerate(graph, s, 0);
                    Renumerate(graph, xy, 1);
                }
                if (s == 0 && xy != 1) Renumerate(graph, xy, 1);
                if (s == 1 && xy != 0) Renumerate(graph, xy, 0);
                if (xy == 0 && s != 1) Renumerate(graph, s, 1);
                if (xy == 1 && s != 0) Renumerate(graph, s, 0);
                edge m;

                if (!connectedGraph) {
                    visitedNodes = visitedNodesBeforeRenumerate;
                    int firstComponentSize = 0;
                    for (int i = 0; i < visitedNodes.size(); i++)
                        if (visitedNodes[i]) firstComponentSize++;
                    int secondComponentSize = visitedNodes.size() - firstComponentSize;

                    if (firstComponentSize != 1 && secondComponentSize != 1) {
                        decomp1++;
                        if ((visitedNodes[x] && visitedNodes[y]) || (!visitedNodes[x] && !visitedNodes[y]))
                            cout << "Eror in decomposition chainreductiom" << endl;

                        graph = graphWithDeletedChain;
                        s = sBeforeRenumerate;
                        if ((visitedNodes[s] && visitedNodes[x]) || (!visitedNodes[s] && !visitedNodes[x])) {
                            if (s != 0 && s != 1 && x != 0 && x != 1) {
                                Renumerate(graph, s, 0);
                                Renumerate(graph, x, 1);
                            }
                            if (s == 0 && x != 1) Renumerate(graph, x, 1);
                            if (s == 1 && x != 0) Renumerate(graph, x, 0);
                            if (x == 0 && s != 1) Renumerate(graph, s, 1);
                            if (x == 1 && s != 0) Renumerate(graph, s, 0);
                        }
                        else {
                            if (s != 0 && s != 1 && y != 0 && y != 1) {
                                Renumerate(graph, s, 0);
                                Renumerate(graph, y, 1);
                            }
                            if (s == 0 && y != 1) Renumerate(graph, y, 1);
                            if (s == 1 && y != 0) Renumerate(graph, y, 0);
                            if (y == 0 && s != 1) Renumerate(graph, s, 1);
                            if (y == 1 && s != 0) Renumerate(graph, s, 0);
                        }

                        m = PairwiseConnectivity(graph, F3 * T, connectedGraph); // dont *F twice
                    }
                    else m = PairwiseConnectivity(graph, F3 * T, connected); // Rs,xy
                }
                else m = PairwiseConnectivity(graph, F3 * T, connected); // Rs,xy

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
                    if (Ch[j] > i && maskApc[i][Ch[j]]) {
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
                    if (Ch[j] > i && maskApc[i][Ch[j]]) {
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
                    if (Ch[j] > i && maskApc[i][Ch[j]]) F3++;

                for (int j = 0; j<Ch.size(); j++)
                    if (Ch[j] > i && maskApc[i][Ch[j]]) {
                        maskApc[i][Ch[j]] = false;
                        maskApc[Ch[j]][i] = false;
                    }

                for (int j = 1; j<Ch.size() - 1; j++) {
                    DeleteNode(G, Ch[j]);
                    for (int k = 0; k<Ch.size(); k++) if (Ch[j] < Ch[k]) Ch[k]--; // not forget about Ch
                }
                int x = Ch.front(), y = Ch.back(); // doesn't matter x<=>y

                bool connectedGraph = ConnectedGraph(G);

                if (x != 0 && x != 1 && y != 0 && y != 1) {
                    Renumerate(G, x, 0);
                    Renumerate(G, y, 1);
                }

                if (x == 0 && y != 1) Renumerate(G, y, 1);
                if (x == 1 && y != 0) Renumerate(G, y, 0);
                if (y == 0 && x != 1) Renumerate(G, x, 1);
                if (y == 1 && x != 0) Renumerate(G, x, 0);
                edge n = PairwiseConnectivity(G, F2 - F3 * T, connectedGraph); // Rxy

                sum = sum + (F1 + n);
            }
        return ChainReduction1(H, sum, checkedNodes, connected);
    }

    return sum;
}