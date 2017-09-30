#include "stdafx.h"
#include "funcs.h"
#include "globals.h"

edge ChainReduction2(vector<vector<edge>> &H, edge sum, vector<int> weight, bool connected)
{
    vector<int> checkedNodes; // considered nodes
    for (int i = 0; i<maskMenc.size(); i++)
        if (!maskMenc[i]) checkedNodes.push_back(i);
    vector<int> Chain = GetChain(H, checkedNodes);

    if (!Chain.empty()) {
        chrs++;
        for (int i = 0; i<Chain.size(); i++)
            if (!ElementInside(checkedNodes, Chain[i]))
                maskMenc[Chain[i]] = false;

        if (!ElementInside(Chain, 0)) {
            edge T;
            T.C.push_back(1);
            vector<int> Ch = Chain;
            vector<vector<edge>> graph(H.size());
            for (int i = 0; i<H.size(); i++) graph[i] = H[i];
            for (int i = 0; i<Ch.size() - 1; i++)
                T = T*graph[Ch[i]][Ch[i + 1]];

            edge F1, F2;
            int F3 = 0;
            for (int i = 0; i<Ch.size() - 1; i++)
                if (!ElementInside(checkedNodes, Ch[i])) { // i=Ch.size() - 1 doesn't make anything
                    edge L; // save power,simple, but lost P.C
                    L.C.push_back(1);
                    for (int j = 0; j<i; j++)
                        L = L*graph[Ch[j]][Ch[j + 1]];
                    F1 = F1 + weight[Ch[i]] * (L - T); // here arise size=1 - edge
                }
            for (int i = 1; i<Ch.size(); i++)
                if (!ElementInside(checkedNodes, Ch[i])) { // i=0 doesn't make anything
                    edge P;
                    P.C.push_back(1);
                    for (int j = i; j<Ch.size() - 1; j++)
                        P = P*graph[Ch[j]][Ch[j + 1]];
                    F2 = F2 + weight[Ch[i]] * (P - T);
                }

            for (int i = 0; i<Ch.size(); i++)
                if (!ElementInside(checkedNodes, Ch[i]))
                    F3 += weight[Ch[i]];

            for (int i = 1; i<Ch.size() - 1; i++) { // after this we get 2 nodes from chain, one of them can be pivote
                DeleteNode(graph, Ch[i]);
                for (int j = 0; j<Ch.size(); j++)
                    if (Ch[i] < Ch[j]) Ch[j]--; // not forget about Ch
            }
            int x = Ch.front(), y = Ch.back(); // matter x<=>y

            vector<vector<edge>> graphWithDeletedChain(graph.size());
            for (int i = 0; i<graph.size(); i++) graphWithDeletedChain[i] = graph[i]; // after reduction graph changed
            bool connectedGraph = ConnectedGraph(graph);
            vector<bool> visitedNodesBeforeRenumerate = visitedNodes;

            edge k;
            if (!F2.C.empty()) {
                Renumerate(graph, y, 1);  // s - node out of chain always 0, so t=1
                k = PairwiseConnectivity(graph, F2, connectedGraph); // R1y
            }

            edge w;
            if (!F1.C.empty()) {
                visitedNodes = visitedNodesBeforeRenumerate;
                graph.resize(graphWithDeletedChain.size());
                for (int i = 0; i < graphWithDeletedChain.size(); i++) graph[i] = graphWithDeletedChain[i];
                Renumerate(graph, x, 1);
                w = PairwiseConnectivity(graph, F1, connectedGraph);// R1x
            }

            graph = graphWithDeletedChain;
            int xy = x;
            if (y < xy) xy--; // not forget
            ContractEdge(graph, y, x); // x or y can't be s

            if (xy != 1) Renumerate(graph, xy, 1); // 0 can not be in Ch

            edge m;
            if (!connectedGraph) {
                visitedNodes = visitedNodesBeforeRenumerate;
                int firstComponentSize = 0;
                for (int i = 0; i < visitedNodes.size(); i++)
                    if (visitedNodes[i]) firstComponentSize++;
                int secondComponentSize = visitedNodes.size() - firstComponentSize;

                if (firstComponentSize != 1 && secondComponentSize != 1) {
                    decomp1++;
                    graph = graphWithDeletedChain;
                    if ((visitedNodes[0] && visitedNodes[x]) || (!visitedNodes[0] && !visitedNodes[x]))
                        Renumerate(graph, x, 1); // why not
                    else
                        Renumerate(graph, y, 1); // why not

                    m = PairwiseConnectivity(graph, F3 * T, connectedGraph); // dont *F twice
                }
                else m = PairwiseConnectivity(graph, F3 * T, connected); // R1xy
            }
            else m = PairwiseConnectivity(graph, F3 * T, connected); // R1xy

            sum = sum + (k + w + m);
        }
        else {
            vector<int> Ch = Chain;
            vector<int>::iterator it, iteratorOfPivoteNode;
            for (it = Ch.begin(); it<Ch.end(); ++it)
                if (*it == 0) iteratorOfPivoteNode = it;
            int placeOfPivoteNodeS = iteratorOfPivoteNode - Ch.begin(); // place of pivote node s=0 into vector Ch

            edge T;
            T.C.push_back(1);
            vector<vector<edge>> graph(H.size());
            for (int i = 0; i<H.size(); i++) graph[i] = H[i];
            for (int i = 0; i<Ch.size() - 1; i++)
                T = T*graph[Ch[i]][Ch[i + 1]];

            edge F1, F2;
            int F3 = 0;
            for (int i = 0; i<Ch.size(); i++)
                if (i != placeOfPivoteNodeS && !ElementInside(checkedNodes, Ch[i])) {
                    edge P; // save power,simple, but lost P.C
                    P.C.push_back(1);
                    if (i < placeOfPivoteNodeS)
                        for (int j = i; j<placeOfPivoteNodeS; j++)
                            P = P*graph[Ch[j]][Ch[j + 1]];
                    else
                        for (int j = placeOfPivoteNodeS; j<i; j++) P = P*graph[Ch[j]][Ch[j + 1]];

                    F1 = F1 + weight[Ch[i]] * P;
                }

            for (int i = 0; i<Ch.size(); i++)
                if (i != placeOfPivoteNodeS && !ElementInside(checkedNodes, Ch[i])) {
                    edge L;
                    L.C.push_back(1);
                    if (i < placeOfPivoteNodeS) {
                        for (int j = 0; j < i; j++)
                            L = L*graph[Ch[j]][Ch[j + 1]];
                        for (int j = placeOfPivoteNodeS; j<Ch.size() - 1; j++)
                            L = L*graph[Ch[j]][Ch[j + 1]];
                    }
                    else {
                        for (int j = 0; j < placeOfPivoteNodeS; j++
                                ) L = L*graph[Ch[j]][Ch[j + 1]];
                        for (int j = i; j<Ch.size() - 1; j++)
                            L = L*graph[Ch[j]][Ch[j + 1]];
                    }

                    F2 = F2 + weight[Ch[i]] * L;
                }

            for (int i = 0; i<Ch.size(); i++)
                if (!ElementInside(checkedNodes, Ch[i]))
                    F3 += weight[Ch[i]];

            for (int i = 1; i<Ch.size() - 1; i++) {
                DeleteNode(graph, Ch[i]);
                for (int j = 0; j<Ch.size(); j++)
                    if (Ch[i] < Ch[j]) Ch[j]--; // not forget about Ch
            }

            bool connectedGraph = ConnectedGraph(graph);

            int x = Ch.front(), y = Ch.back(); // doesn't matter x<=>y

            if (x != 0 && x != 1 && y != 0 && y != 1) {
                Renumerate(graph, x, 0);
                Renumerate(graph, y, 1);
            }

            if (x == 0 && y != 1) Renumerate(graph, y, 1);
            if (x == 1 && y != 0) Renumerate(graph, y, 0);
            if (y == 0 && x != 1) Renumerate(graph, x, 1);
            if (y == 1 && x != 0) Renumerate(graph, x, 0);

            edge n = PairwiseConnectivity(graph, F2 - F3 * T, connectedGraph); // Rxy

            sum = sum + (F1 + n);
        }
        return ChainReduction2(H, sum, weight, connected);
    }
    return sum;
}