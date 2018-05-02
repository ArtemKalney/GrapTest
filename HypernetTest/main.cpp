#include "funcs.h"
#include "globals.h"

std::ifstream input("input.txt");
std::ofstream output;

int reliable = 0, unconnected = 0, num2 = 0, chr = 0, bridges = 0, pendunt = 0, factors = 0, n = 0, m = 0, k = 0;
std::vector<Branche> Bin;

void OutPrintBranche(Branche& branche) {
    if (branche.IsExisting()) {
        output << "Branche:" << std::endl;
        for (auto &item : branche.GetC()) {
            output << item << " ";
        }
        output << std::endl << "power=" << branche.GetPower() << ",simple=" << branche.GetSimple() << std::endl;
    }
    else {
        output << "empty edge" << std::endl;
    }
}

void OutPrintHypernet(H &H) {
    output << "FN:" << std::endl;
    for (int i = 0; i < H.GetFN().size(); i++) {
        for (int j = 0; j < H.GetFN()[i].size(); j++) {
            if (i < j && H.GetFN()[i][j].IsExisting()) {
                output << "(" << i << "," << j << ")" << std::endl;
                OutPrintBranche(H.GetFN()[i][j]);
            }
        }
    }
    output << "F:" << std::endl;
    for (auto &row : H.GetF()) {
        for (auto &cell : row) {
            output << cell << " ";
        }
        output << std::endl;
    }
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
    int buf;
    input >> buf;n = buf;
    input >> buf;m = buf;
    input >> buf;k = buf;

    std::vector<Branche> branchList;
    // Read all branches from output.txt
    for (int i = 0; i < m; i++) {
        bool isSimpleBranch = false;
        input >> buf;
        int firstNode = buf - 1;
        input >> buf;
        int secondNode = buf - 1;
        Branche newBranch = Branche::GetSimpleBranch(firstNode, secondNode);
        // If edge was recorded, add to it simple
        for (auto &item : branchList) {
            if (Branche::EqualNodes(item, newBranch)) {
                item.SetSimple(item.GetSimple() + 1);
                isSimpleBranch = true;
            }
        }
        if (!isSimpleBranch) {
            branchList.push_back(std::move(newBranch));
        }
    }
    std::vector<std::vector<int>> F;
    // Read all edges from output.txt
    for (int i = 0; i < k; i++) {
        std::vector<int> edge;
        input >> buf;
        while (buf != 0) {
            int node = buf - 1;
            edge.push_back(node);
            input >> buf;
        }
        F.push_back(std::move(edge));
    }
    // Input should end by $$$
    input >> str;
    if (strcmp(str, "$$$") != 0) {
        std::cout << "Incorrect entry" << std::endl;
        system("pause>>void");
        return 0;
    }

    std::cout << "Press 1 to get APC polynomial" << std::endl;
    std::cout << "Press 2 to get MENC polynomial" << std::endl;
    std::cout << "Press 3 to get pairwise connectivities" << std::endl;
    int option = 3;
    std::cin >> option;
    if (option != 1 && option != 2 && option != 3) {
        std::cout << "Wrong number" << std::endl;
        system("pause>>void");
        return 0;
    }
    // In advance, calculate units of degree from 0 to m
    Bin.resize(m + 1, Branche::GetBranch(m + 1, 0));
    for (int i = 0; i < Bin.size(); i++) {
        Bin[i].SetPower(i);
        if (i != 0) {
            for (int j = 1; j < m + 1; j++) {
                Bin[i].GetC()[j] = Bin[i - 1].GetC()[j - 1] + Bin[i - 1].GetC()[j];
            }
        }
    }
    // Create an initialHypernet
    H initialHypernet = H(H::GetAdjacencyMatrix(branchList), F);
    initialHypernet.RemoveEmptyBranches();
    output << "Input Hypernet:" << std::endl;
    OutPrintHypernet(initialHypernet);
    // In the beginning we consider only connected hypernets
    if (!initialHypernet.IsSNconnected()) {
        std::cout << "Unconnected hypernet on input!" << std::endl;
        return 0;
    }
    // Create a pseudo-edge F, which we multiply by the end of the calculations
    Branche pseudoEdge = Branche::GetBranch(0);
    unsigned int startTime = clock();
    double value = 0, p = 0.9, z = 0.1;
    Branche sum;

    try {
        // Computing pairwise connectivities
        if (option == 3) {
            H H = initialHypernet;
            //H.RenumerateNodes(1, 0);
            //H.RenumerateNodes(4, 1);
            sum = sum + PairwiseConnectivity(H, pseudoEdge);
            //debug
           /* auto customHypernet = initialHypernet;
            //customHypernet.RenumerateNodesForGen(1, 0);
            //customHypernet.RenumerateNodesForGen(8, 1);
            auto branchListForGen = customHypernet.GetBranchList();
            std::vector<int> brancheMask(branchListForGen.size());
            int startPos = 0;
            GenCombinations(customHypernet, branchListForGen, sum, brancheMask, startPos);*/
        }
        // Computing APC
        if (option == 1) {
            for (int i = 0; i < n; i++) {
                for (int j = i + 1; j < n; j++) {
                    auto H = initialHypernet;
                    //When calculating the pairwise connection, the pivot nodes have numbers 0,1 in the adjacency matrix
                    if (i != 0 || j != 1) {
                        if (i != 0 && j != 1) {
                            H.RenumerateNodes(i, 0);
                            H.RenumerateNodes(j, 1);
                        }
                        if (i == 0 && j != 1) {
                            H.RenumerateNodes(j, 1);
                        }
                        if (i != 0 && j == 1) {
                            H.RenumerateNodes(i, 0);
                        }
                    }
                    Branche result = PairwiseConnectivity(H, pseudoEdge);
                    if (result.IsExisting()) {
                        sum = sum + result;
                    } else {
                        throw "PairwiseConnectivity: empty result";
                    }
                    //sum = sum + PairwiseConnectivity(H, pseudoEdge);
                    //debug
                   /* auto customHypernet = initialHypernet;
                    if (i != 0 || j != 1) {
                        if (i != 0 && j != 1) {
                            customHypernet.RenumerateNodesForGen(i, 0);
                            customHypernet.RenumerateNodesForGen(j, 1);
                        }
                        if (i == 0 && j != 1) {
                            customHypernet.RenumerateNodesForGen(j, 1);
                        }
                        if (i != 0 && j == 1) {
                            customHypernet.RenumerateNodesForGen(i, 0);
                        }
                    }
                    auto branchListForGen = customHypernet.GetBranchList();
                    std::vector<int> brancheMask(branchListForGen.size());
                    int startPos = 0;
                    GenCombinations(customHypernet, branchListForGen, sum, brancheMask, startPos);*/

                    //std::cout << "+R" << i + 1 << j + 1 << std::endl;
                }
            }

            for (int i = 0; i < sum.GetC().size(); i++) {
                auto sumVector = sum.GetC();
                sumVector[i] = sumVector[i] / Bin[n].GetC()[2];
                sum.SetC(sumVector);
            }
        }
        // Computing MENC
        if (option == 2) {
            for (int i = 1; i < n; i++) {
                auto H = initialHypernet;
                // When calculating the pairwise connection, the pivot nodes have numbers 0,1 in the adjacency matrix
                if (i != 1) {
                    H.RenumerateNodes(i, 1);
                }
                Branche result = PairwiseConnectivity(H, pseudoEdge);
                if (result.IsExisting()) {
                    sum = sum + result;
                } else {
                    throw "PairwiseConnectivity: empty result";
                }
                //sum = sum + PairwiseConnectivity(H, pseudoEdge);
                //debug
                /*auto customHypernet = initialHypernet;
                if (i != 1) {
                    customHypernet.RenumerateNodesForGen(i, 1);
                }
                auto branchListForGen = customHypernet.GetBranchList();
                std::vector<int> brancheMask(branchListForGen.size());
                int startPos = 0;
                GenCombinations(customHypernet, branchListForGen, sum, brancheMask, startPos);*/

                //std::cout << "+R1" << i + 1 << std::endl;
            }

            sum = sum + Branche::GetUnity();
        }
    }
    catch (const char *str) {
        std::cout << "--------------------------------" << std::endl;
        std::cout << "Occurred next error:" << std::endl;
        std::cout << str << std::endl;
        std::cout << "--------------------------------" << std::endl;
    }

    unsigned int endTime = clock();
    int searchTime = endTime - startTime;
    std::cout << "Time of programm " << searchTime << " msec" << std::endl;
    std::cout << "Call prosedure " << factors << std::endl;
    std::cout << "Reductions : " << std::endl;
    std::cout << " bridge reductions " << bridges << std::endl;
    std::cout << " pendunt reductions " << pendunt << std::endl;
    std::cout << " chains reduced " << chr << std::endl;
    std::cout << "Were ends of recursion : " << reliable + unconnected + num2 << std::endl;
    std::cout << " reliable graphs " << reliable << std::endl;
    std::cout << " unconnected graphs " << unconnected << std::endl;
    std::cout << " 2-nodes graphs " << num2 << std::endl;
    std::cout << "Solution:" << std::endl;
    if (sum.IsExisting()) {
        sum.GetC().resize(m + 1);
        if (sum.GetPower() < m) {
            sum = sum * Bin[m - sum.GetPower()];
        }
        sum.PrintBranche();
        for (int i = 0; i < sum.GetPower(); i++) {
            value += sum.GetC()[i] * pow(p, sum.GetPower() - i) * pow(z, i);
        }
        std::cout << "Value at point " << p << ": " << std::setprecision(15) << value << std::endl;

        output << "Solution:" << std::endl;
        for (auto &item : sum.GetC()) {
            output << std::setprecision(15) << item << " ";
        }
        output << std::endl;
    } else {
        std::cout << "no sum :(" << std::endl;
    }

    input.close();
    output.close();

    /*system("pause>>void");*/
    return 0;
}