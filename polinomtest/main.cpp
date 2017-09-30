#include "funcs.h"
#include "globals.h"

ifstream input("input.txt");
ofstream output;

int num0 = 0, num2 = 0, num3 = 0, num4 = 0, num5 = 0, chr = 0, ch1 = 0, ch2 = 0, chrs = 0,
    decomp1 = 0, decomp2 = 0, decomp3 = 0, bridges = 0, pendunt = 0, factors = 0;
vector<bool> visitedNodes;
vector<vector<bool> > maskApc;
vector<bool> maskMenc;
vector<edge> Bin;

void MakeAdjacencyMatrix(vector<vector<edge>> &H, vector<edge> &E)
{
	for (int i = 0; i<H.size(); i++)
        H[i].resize(H.size());
	//initialize all strings, so as array is associative, not fill by nulls, or it is not global
	for (int i = 0; i<H.size(); i++)
        for (int j = 0; j<H[i].size(); j++) {
		H[i][j].ex = false;
		for (int w = 0; w<E.size(); w++)
			if ((i == E[w].node1 && j == E[w].node2) || (i == E[w].node2 && j == E[w].node1))
                H[i][j] = E[w];
	}
}

int main() {
    output.open("output.txt");
    setlocale(LC_ALL, "");

    if (!input.is_open()) {
        cout << "File can not be opened!\n";
        return 0;
    }

    char str[50];
    int n = 0, m = 0;
    vector<edge> E;
    input.getline(str, 50);
    int buf, count = 0;
    input >> buf;
    n = buf;
    input >> buf;
    m = buf;

    while (count != m) {
        bool simpleEdge = false;
        edge q; // filling data for new edge
        q.simple = 0;
        q.ex = true;
        q.power = 1;
        q.C.push_back(1);
        q.C.resize(m + 1); // numerate start from 0
        input >> buf;
        q.node1 = buf - 1;
        input >> buf;
        q.node2 = buf - 1;
        for (int i = 0; i < E.size(); i++) // work with multi edges
            if ((E[i].node1 == q.node1 && E[i].node2 == q.node2) || (E[i].node1 == q.node2 && E[i].node2 == q.node1)) {
                E[i].simple++;
                simpleEdge = true;
            }
        if (!simpleEdge) E.push_back(q);
        count++;
    }

    input >> str;
    if (strcmp(str, "$$$") != 0) {
        cout << "Incorrect entry" << endl; // 0 = ; 1 < ; -1 >
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

    Bin.resize(m + 1); // max power is m-1
    for (int i = 0; i < Bin.size(); i++) {
        Bin[i].C.resize(m + 1);
        Bin[i].power = i;
        Bin[i].simple = 0;
        Bin[i].C.front() = 1;
        if (i != 0)
            for (int j = 1; j < m + 1; j++)
                Bin[i].C[j] = Bin[i - 1].C[j - 1] + Bin[i - 1].C[j];
    }

    vector<vector<edge>> initialGraph(n);
    MakeAdjacencyMatrix(initialGraph, E);

    if (!ConnectedGraph(initialGraph)) {
        cout << "Unconnected graph on input!" << endl;
        return 0;
    }

    edge F = Bin.front(); // create pseudo edge for computing
    unsigned int startTime = clock();
    double value = 0, p = 0.9, z = 0.1;
    edge sum;

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
                    if (i != 0 || j != 1) { // after this 0,1
                        if (i != 0 && j != 1) {
                            Renumerate(H, i, 0);
                            Renumerate(H, j, 1);
                        }
                        if (i == 0 && j != 1) Renumerate(H, j, 1);
                        if (i != 0 && j == 1) Renumerate(H, i, 0);
                    }
                    sum = sum + PairwiseConnectivity(H, F, true); // H changed
                }

        for (int i = 0; i < sum.C.size(); i++) sum.C[i] = sum.C[i] / Bin[initialGraph.size()].C[2];
    }

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
                if (i != 1) Renumerate(H, i, 1); // after this i=1
                sum = sum + PairwiseConnectivity(H, F, true); // H changed
            }

        sum = sum + Bin.front();
    }

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
                    if (i != 0 || j != 1) { // after this 0,1
                        if (i != 0 && j != 1) {
                            Renumerate(H, i, 0);
                            Renumerate(H, j, 1);
                        }
                        if (i == 0 && j != 1) Renumerate(H, j, 1);
                        if (i != 0 && j == 1) Renumerate(H, i, 0);
                    }

                    edge R = PairwiseConnectivity(H, F, true); // H changed

                    output << "R" << i + 1 << "," << j + 1 << ":" << endl;
                    for (int k = 0; k < R.C.size(); k++)
                        output << R.C[k] << " ";
                    output << endl << "power = " << R.power << ", simple = " << R.simple << endl;

                    for (int l = 0; l < R.power; l++)
                        value += R.C[l] * pow(p, R.power - l) * pow(z, l);
                    output << "Value at point " << p << ": " << setprecision(15) << value << endl;
                }

        return 0;
    }

    unsigned int endTime = clock();
    int searchTime = endTime - startTime;
    cout << "Time of programm " << searchTime << " msec" << endl;
    cout << "Call prosedure " << factors << endl;
    cout << "Reductions : " << endl;
    cout << " reduced chains initially " << chrs << endl;
    cout << " bridge reductions " << bridges << endl;
    cout << " pendunt reducutions " << pendunt << endl;
    cout << " chains reduced " << chr << endl;
    cout << " 1-st type " << ch1 << endl;
    cout << " 2-nd type " << ch2 << endl;
    cout << " decompositions in ChainReduction " << decomp1 << endl;
    cout << " decompositions in chr " << decomp2 << endl;
    cout << " decompositions in factoring " << decomp3 << endl;
    cout << "Were ends of recursion : " << num0 + num2 + num3 + num4 + num5 << endl;
    cout << " unconnected graphs " << num0 << endl;
    cout << " 2-dimension graphs " << num2 << endl;
    cout << " 3-dimension graphs " << num3 << endl;
    cout << " 4-dimension graphs " << num4 << endl;
    cout << " 5-dimension graphs " << num5 << endl;
    cout << "Solution:" << endl;
    sum.PrintEdge();
    for (int i = 0; i < sum.power; i++)
        value += sum.C[i] * pow(p, sum.power - i) * pow(z, i);
    cout << "Value at point " << p << ": " << fixed << setprecision(15) << value << endl;

    for (int i = 0; i < sum.C.size(); i++)
        output << sum.C[i] << " ";
    output << endl << "power = " << sum.power << ", simple = " << sum.simple << endl;

    input.close();
    output.close();

    system("pause>>void");
    return 0;
}