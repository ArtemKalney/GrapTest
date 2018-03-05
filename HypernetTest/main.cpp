#include "funcs.h"
#include "globals.h"

std::ifstream input("input.txt");
std::ofstream output;

/*int num0 = 0, num2 = 0, num3 = 0, num4 = 0, num5 = 0, chr = 0, ch1 = 0, ch2 = 0, chrs = 0,
        decomp1 = 0, decomp2 = 0, decomp3 = 0, bridges = 0, pendunt = 0, factors = 0;
vector<bool> visitedNodes;
vector<vector<bool> > maskApc;
vector<bool> maskMenc;
*/
std::vector<Branche> Bin;

std::vector<std::vector<Branche>> GetAdjacencyMatrix(std::vector<Branche>& BranchList, const int& n) {
    std::vector<std::vector<Branche>> FN(n);
    for (auto &row : FN) {
        row.resize(FN.size(), Branche::GetZero());
    }
    for (int i = 0; i < FN.size(); i++) {
        for (int j = 0; j < FN[i].size(); j++) {
            for (auto &item : BranchList) {
                if (Branche::EqualNodes(item, i, j)) {
                    FN[i][j] = item;
                }
            }
        }
    }

    return FN;
}

int main() {
    output.open("output.txt");
    setlocale(LC_ALL, "");

    if (!input.is_open()) {
        std::cout << "File can not be opened!\n";
        system("pause>>void");
        return 0;
    }

    char str[50];
    input.getline(str, 50);
    std::cout << "Input graph : " << str << std::endl;
    int buf, n = 0, m = 0, k = 0;
    input >> buf;
    n = buf;
    input >> buf;
    m = buf;
    input >> buf;
    k = buf;

    std::vector<Branche> BranchList;
    // Read all edges from output.txt
    for (int i = 0; i < m; i++) {
        bool simpleEdge = false;
        input >> buf;
        int firstNode = buf - 1;
        input >> buf;
        int secondNode = buf - 1;
        auto newEdge = Branche::GetSimpleBranch(n, firstNode, secondNode);
        // If edge was recorded, add to it simple
        for (auto &edge : BranchList) {
            if (Branche::EqualNodes(edge, newEdge)) {
                edge.SetSimple(edge.GetSimple() + 1);
                simpleEdge = true;
            }
        }
        if (!simpleEdge) BranchList.push_back(std::move(newEdge));
    }
    std::vector<std::vector<int>> F;
    // Read all paths from output.txt
    for (int i = 0; i < k; i++) {
        std::vector<int> path;
        input >> buf;
        while (buf != 0) {
            int node = buf - 1;
            path.push_back(node);
            input >> buf;
        }
        F.push_back(std::move(path));
    }

    // Input should end by $$$
    input >> str;
    if (strcmp(str, "$$$") != 0) {
        std::cout << "Incorrect entry" << std::endl;
        system("pause>>void");
        return 0;
    }

   /* std::cout << "Press 1 to get APC polynomial" << std::endl;
    std::cout << "Press 2 to get MENC polynomial" << std::endl;
    std::cout << "Press 3 to get pairwise connectivities" << std::endl;*/
    int option = 3;
    /*std::cin >> option;
    if (option != 1 && option != 2 && option != 3) {
        std::cout << "Wrong number" << std::endl;
        system("pause>>void");
        return 0;
    }*/

    // In advance, calculate units of degree from 0 to m
    Bin.resize(m + 1, Branche::GetSimpleBranch(m + 1));
    for (int i = 0; i < Bin.size(); i++) {
        Bin[i].SetPower(i);
        if (i != 0) {
            for (int j = 1; j < m + 1; j++) {
                Bin[i].GetC()[j] = Bin[i - 1].GetC()[j - 1] + Bin[i - 1].GetC()[j];
            }
        }
    }

    // Create an adjacency matrix
    auto initalHypernet = H(GetAdjacencyMatrix(BranchList, n), F);
    initalHypernet.PrintHyperNet();

    // In the beginning we consider only connected graphs
    if (!initalHypernet.IsSNconnected()) {
        std::cout << "Unconnected hypernet on input!" << std::endl;
        return 0;
    }

    // Create a pseudo-edge F, which we multiply by the end of the calculations
    Branche pseudoEdge = Branche::GetBranch(n, 0);
    unsigned int startTime = clock();
    double value = 0, p = 0.9, z = 0.1;
    /*Branche sum;*/

    // Computing pairwise connectivities
    if (option == 3) {
        /*maskApc.resize(initialGraph.size());
        for (int i = 0; i < maskApc.size(); i++) {
            maskApc[i].resize(initialGraph[i].size());
            for (int j = 0; j < maskApc[i].size(); j++)
                i != j ? maskApc[i][j] = true : maskApc[i][j] = false;
        }*/

        /*for (int i = 0; i < initialGraph.size(); i++)
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
                }*/
        auto result = PairwiseConnectivity(initalHypernet, pseudoEdge);
    }

/*
    // Computing APC
    if (option == 1) {
        maskApc.resize(initialGraph.size());
        for (int i = 0; i < maskApc.size(); i++) {
            maskApc[i].resize(initialGraph[i].size());
            for (int j = 0; j < maskApc[i].size(); j++)
                i != j ? maskApc[i][j] = true : maskApc[i][j] = false;
        }

        vector<int> checkedNodes;

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
*/

    unsigned int endTime = clock();
    /*int searchTime = endTime - startTime;
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
    cout << "Solution:" << endl;
    if(sum.power < m)
        sum = sum*Bin[m - sum.power];
    sum.PrintEdge();
    for (int i = 0; i < sum.power; i++)
        value += sum.C[i] * pow(p, sum.power - i) * pow(z, i);
    cout << "Value at point " << p << ": " << setprecision(15) << value << endl;

    for (int i = 0; i < sum.C.size(); i++)
        output << setprecision(15) << sum.C[i] << " ";
    output << endl;*/

    input.close();
    output.close();

    /*system("pause>>void");*/
    return 0;
}