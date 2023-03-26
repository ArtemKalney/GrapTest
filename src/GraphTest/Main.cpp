#include "Funcs.h"
#include "Globals.h"
#include "ApplicationSettings.h"
#include "../Helpers/InputParser.h"

int num0, num2, num3, num4, num5, chr, ch1, ch2, chrs,
    decomp1, decomp2, decomp3, bridges, pendunt, factors;
vector<bool> visitedNodes;
vector<vector<bool> > maskApc;
vector<bool> maskMenc;
vector<edge> Bin;
Settings AppSettings;
ifstream input;
ofstream output;

void MakeAdjacencyMatrix(vector<vector<edge>> &H, vector<edge> &E)
{
	for (int i = 0; i<H.size(); i++)
        H[i].resize(H.size());
	for (int i = 0; i<H.size(); i++)
        for (int j = 0; j<H[i].size(); j++) {
		H[i][j].ex = false;
		for (int w = 0; w<E.size(); w++)
			if ((i == E[w].node1 && j == E[w].node2) || (i == E[w].node2 && j == E[w].node1))
                H[i][j] = E[w];
	}
}

std::vector<edge> GetRandomGraph(int n, int m) {
    srand(time(0)); // Randomized generator of random numbers
    vector<edge> E;

    vector<int> P(n);
    vector<int> L;
    for (int i = 0; i < P.size(); i++) P[i] = i;

    int q = rand() % (P.size() - 1);
    for (int i = 0; i < n - 1; i++) {
        edge newEdge;
        bool reduct = false;
        L.push_back(P[q]);
        P.erase(P.begin() + q);

        int s = 0, t = 0;
        if (P.size() - 1 != 0) s = rand() % (P.size() - 1);
        if (L.size() - 1 != 0) t = rand() % (L.size() - 1);
        q = s;

        newEdge.simple = 0;
        newEdge.power = 1;
        newEdge.C.push_back(1);
        newEdge.C.resize(m + 1);
        newEdge.node1 = P[s];
        newEdge.node2 = L[t];
        E.push_back(newEdge);
    }

    for (int i = 0; i < m - n + 1; i++) {
        edge newEdge;
        bool reduct = false;
        int s = 0, t = 0;
        s = rand() % n;
        t = rand() % n;
        while (s == t) t = rand() % n;

        newEdge.simple = 0;
        newEdge.power = 1;
        newEdge.C.push_back(1);
        newEdge.C.resize(m + 1);
        newEdge.node1 = s;
        newEdge.node2 = t;
        for (int i = 0; i < E.size(); i++) // work with multi edges
            if ((E[i].node1 == newEdge.node1 && E[i].node2 == newEdge.node2) || (E[i].node1 == newEdge.node2 && E[i].node2 == newEdge.node1)) {
                E[i].simple++;
                reduct = true;
            }
        if (!reduct)
        {
            E.push_back(newEdge);
        }
    }

    vector<vector<edge>> S(n);
    MakeAdjacencyMatrix(S, E);

//    bool r = IsConnected(S);
//    if (!r) cout << "Unconnected graph generate!" << endl;

    return E;
}

void ConvertGraphToKAOFO(std::vector<edge> &graph, int nodeCount, std::vector<int> &KAO, std::vector<int> &FO) {
    int count = 0;
    KAO.push_back(count);
    for (int node = 0; node < nodeCount; ++node) {
        for(auto item : graph) {
            if (item.node1 == node || item.node2 == node) {
                count++;
                FO.push_back(item.node1 == node ? item.node2 : item.node1);
            }
        }
        KAO.push_back(count);
    }
}

void OutputGraph(std::vector<edge> &graph, int nodeCount) {
    output << "Graph(" << nodeCount << "," << graph.size() << ")" << std::endl;
    output << nodeCount << " " << graph.size() << std::endl;
    for(auto item : graph) {
        output << item.node1 + 1 << " " << item.node2 + 1 << std::endl;
    }
    output << "$$$" << std::endl;
}

void OutputKAOFO(std::vector<int> &KAO, std::vector<int> &FO) {
    output << FO.size() << std::endl;
    output << KAO.size() << std::endl;

    for(int i=0; i < KAO.size(); i++) {
        if (i != KAO.size() - 1) {
            output << KAO[i] << ",";
        }
        else {
            output << KAO[i] << std::endl;
        }
    }

    for(int i=0; i < FO.size(); i++) {
        if (i != FO.size() - 1) {
            output << FO[i] + 1 << ",";
        }
        else {
            output << FO[i] + 1 << std::endl;
        }
    }
}

void SetGlobals(int argc, char** argv) {
    num0 = 0;
    num2 = 0;
    num3 = 0;
    num4 = 0;
    num5 = 0;
    chr = 0;
    ch1 = 0;
    ch2 = 0;
    chrs = 0;
    decomp1 = 0;
    decomp2 = 0;
    decomp3 = 0;
    bridges = 0;
    pendunt = 0;
    factors = 0;

    AppSettings.IsNumberComputation = IS_NUMBER_COMPUTATION;

    InputParser inputParser(argc, argv);
    std::string str;
    str = inputParser.getCmdOption("-p");
    AppSettings.ReliabilityValue = !str.empty() ? std::stod(str) : RELIABILITY_VALUE;

    str = inputParser.getCmdOption("-input");
    input.open(!str.empty() ? str : "input.txt");
    str = inputParser.getCmdOption("-output");
    output.open(!str.empty() ? str : "output.txt");
}

int main(int argc, char** argv) {
    SetGlobals(argc, argv);
    setlocale(LC_ALL, "");

    if (IS_GRAPH_GENERATION == 1) {
        auto graph = GetRandomGraph(GRAPH_GENERATION_NODE_SIZE, GRAPH_GENERATION_EDGE_SIZE);

        std::vector<int> KAO, FO;
        ConvertGraphToKAOFO(graph, GRAPH_GENERATION_NODE_SIZE, KAO, FO);

        OutputGraph(graph, GRAPH_GENERATION_NODE_SIZE);
        OutputKAOFO(KAO, FO);

        return 0;
    }

    if (!input.is_open()) {
        cout << "File can not be opened!\n";

        return 0;
    }

    char str[50];
    int n = 0, m = 0;
    vector<edge> E;
    input.getline(str, 50);
    cout << "Input graph : " << str << endl;
    int buf, count = 0;
    input >> buf;
    n = buf;
    input >> buf;
    m = buf;

    // Read all edges from output.txt
    while (count != m) {
        bool simpleEdge = false;
        edge q;
        q.simple = 0;
        q.ex = true;
        q.power = 1;
        q.C.push_back(1);
        q.C.resize(m + 1);
        input >> buf;
        q.node1 = buf - 1;
        input >> buf;
        q.node2 = buf - 1;
        // If edge was recorded, add to it simple
        for (int i = 0; i < E.size(); i++)
            if ((E[i].node1 == q.node1 && E[i].node2 == q.node2) || (E[i].node1 == q.node2 && E[i].node2 == q.node1)) {
                E[i].simple++;
                simpleEdge = true;
            }
        if (!simpleEdge) E.push_back(q);
        count++;
    }

    // Input should end by $$$
    input >> str;
    if (strcmp(str, "$$$") != 0) {
        cout << "Incorrect entry" << endl;

        return 0;
    }

    cout << "Press 1 to get APC polynomial" << endl;
    cout << "Press 2 to get MENC polynomial" << endl;
    cout << "Press 3 to get pairwise connectivities" << endl;
    int option = 1;
    cin >> option;
    if (option != 1 && option != 2 && option != 3) {
        cout << "Wrong number" << endl;

        return 0;
    }

    // In advance, calculate units of degree from 0 to m
    Bin.resize(m + 1);
    for (int i = 0; i < Bin.size(); i++) {
        Bin[i].C.resize(m + 1);
        Bin[i].power = i;
        Bin[i].simple = 0;
        Bin[i].C.front() = 1;
        if (i != 0)
            for (int j = 1; j < m + 1; j++)
                Bin[i].C[j] = Bin[i - 1].C[j - 1] + Bin[i - 1].C[j];
    }

    // Create an adjacency matrix
    vector<vector<edge>> initialGraph(n);
    MakeAdjacencyMatrix(initialGraph, E);

    // In the beginning we consider only connected graphs
    if (!ConnectedGraph(initialGraph)) {
        cout << "Unconnected graph on input!" << endl;
        return 0;
    }

    // Create a pseudo-edge F, which we multiply by the end of the calculations
    edge F = Bin.front();
    unsigned int startTime = clock();
    double value = 0, p = AppSettings.ReliabilityValue, z = 1 - AppSettings.ReliabilityValue;
    edge sum;

    // Computing APC
    if (option == 1) {
        maskApc.resize(initialGraph.size());
        for (int i = 0; i < maskApc.size(); i++) {
            maskApc[i].resize(initialGraph[i].size());
            for (int j = 0; j < maskApc[i].size(); j++)
                i != j ? maskApc[i][j] = true : maskApc[i][j] = false;
        }

        vector<int> checkedNodes;
        sum = ChainReduction1(initialGraph, sum, checkedNodes, true);

        for (int i = 0; i < initialGraph.size(); i++)
            for (int j = i + 1; j < initialGraph[i].size(); j++)
                if (maskApc[i][j]) {
                    vector<vector<edge>> H(initialGraph);
                    // When calculating the pairwise connection, the pivot nodes have numbers 0,1
                    // in the adjacency matrix
                    if (i != 0 || j != 1) {
                        if (i != 0 && j != 1) {
                            Renumerate(H, i, 0);
                            Renumerate(H, j, 1);
                        }
                        if (i == 0 && j != 1) Renumerate(H, j, 1);
                        if (i != 0 && j == 1) Renumerate(H, i, 0);
                    }
                    sum = sum + PairwiseConnectivity(H, F, true); // H changed
                }

        for (int i = 0; i < sum.C.size(); i++)
            sum.C[i] = sum.C[i] / Bin[initialGraph.size()].C[2];
    }

    // Computing MENC
    if (option == 2) {
        maskMenc.resize(initialGraph.size());
        for (int i = 0; i < maskMenc.size(); i++)
            i == 0 ? maskMenc[i] = false : maskMenc[i] = true;

        vector<int> weight(initialGraph.size());
        for (int i = 0; i < weight.size(); i++) weight[i] = 1;

        sum = ChainReduction2(initialGraph, sum, weight, true);

        for (int i = 0; i < initialGraph.size(); i++)
            if (maskMenc[i]) {
                vector<vector<edge>> H(initialGraph);
                // When calculating the pairwise connection, the pivot nodes have numbers 0,1
                // in the adjacency matrix
                if (i != 1) Renumerate(H, i, 1);
                sum = sum + PairwiseConnectivity(H, F, true);
            }

        sum = sum + Bin.front();
    }

    // Computing pairwise connectivities
    if (option == 3) {
        maskApc.resize(initialGraph.size());
        for (int i = 0; i < maskApc.size(); i++) {
            maskApc[i].resize(initialGraph[i].size());
            for (int j = 0; j < maskApc[i].size(); j++)
                i != j ? maskApc[i][j] = true : maskApc[i][j] = false;
        }

        for (int i = 0; i < initialGraph.size(); i++)
            for (int j = i + 1; j < initialGraph[i].size(); j++)
                if (maskApc[i][j]) {
                    vector<vector<edge>> H(initialGraph);
                    // When calculating the pairwise connection, the pivot nodes have numbers 0,1
                    // in the adjacency matrix
                    if (i != 0 || j != 1) {
                        if (i != 0 && j != 1) {
                            Renumerate(H, i, 0);
                            Renumerate(H, j, 1);
                        }
                        if (i == 0 && j != 1) Renumerate(H, j, 1);
                        if (i != 0 && j == 1) Renumerate(H, i, 0);
                    }

                    edge R = PairwiseConnectivity(H, F, true);

                    if(R.power < m)
                        R = R*Bin[m - R.power];
                    for (int k = 0; k < R.C.size(); k++)
                        output << setprecision(15) << R.C[k] << " ";
                    output << endl;
                }
    }

    unsigned int endTime = clock();
    int searchTime = endTime - startTime;
    cout << "Time of programm " << searchTime << " msec" << endl;
    cout << "Call prosedure " << factors << endl;
    cout << "Reductions : " << endl;
    cout << " reduced chains initially " << chrs << endl;
    cout << " bridge reductions " << bridges << endl;
    cout << " pendunt reductions " << pendunt << endl;
    cout << " chains reduced " << chr << endl;
    cout << "  1-st type " << ch1 << endl;
    cout << "  2-nd type " << ch2 << endl;
    cout << " decompositions in ChainReduction " << decomp1 << endl;
    cout << " decompositions in special chains " << decomp2 << endl;
    cout << " decompositions in factoring " << decomp3 << endl;
    cout << "Were ends of recursion : " << num0 + num2 + num3 + num4 + num5 << endl;
    cout << " unconnected graphs " << num0 << endl;
    cout << " 2-nodes graphs " << num2 << endl;
    cout << " 3-nodes graphs " << num3 << endl;
    cout << " 4-nodes graphs " << num4 << endl;
    cout << " 5-nodes graphs " << num5 << endl;

    if (option == 3) {
        return 0;
    }

    if(sum.power < m)
        sum = sum*Bin[m - sum.power];

    if (AppSettings.IsNumberComputation == 1) {
        value = sum.value;
        cout << "Value: " << setprecision(15) << value << endl;
        output << "Value: " << setprecision(15) << value << endl;
    }
    else {
        for (int i = 0; i < sum.power; i++)
            value += sum.C[i] * pow(p, sum.power - i) * pow(z, i);
        cout << "Value at point " << p << ": " << setprecision(15) << value << endl;

        for (int i = 0; i < sum.C.size(); i++)
            output << setprecision(15) << sum.C[i] << " ";
        output << endl;
    }

    input.close();
    output.close();

    return 0;
}