#include "Funcs.h"
#include "Globals.h"

std::ifstream input("input.txt");
std::ofstream output;

int n = 0, m = 0, k = 0;
int FreeId = 0;
int ReliableHypernetsCount = 0, UnconnectedHypernetsCount = 0, TwoNodesHypernetsCount = 0, ChainsReduced = 0,
        UnconnectedNodesReduced = 0, PairConnectivityCalls = 0, EdgesReduced = 0, UnsimpleChains = 0;
std::vector<Branch> Bin;

void NormalizeSolution(Branch &branch){
    branch.GetC().resize(m + 1);
    if (branch.GetPower() < m) {
        branch = branch * Bin[m - branch.GetPower()];
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
    FreeId = m;

    std::vector<Branch> branchList;
    // Read all branches from input.txt
    for (int i = 0; i < m; i++) {
        input >> buf;
        int firstNode = buf - 1;
        input >> buf;
        int secondNode = buf - 1;
        branchList.push_back(Branch::GetSimpleBranch(i, firstNode, secondNode));
    }
    // Fill all nodes
    std::vector<Node> nodeList;
    for (int i = 0; i < n; i++) {
        Node node = Node(i, false);
        nodeList.push_back(node);
    }
    // Read all edges from input.txt
    std::vector<std::shared_ptr<std::vector<int>>> F;
    for (int i = 0; i < k; i++) {
        std::vector<int> edge;
        input >> buf;
        while (buf != 0) {
            int node = buf - 1;
            edge.push_back(node);
            input >> buf;
        }

        auto ptr = std::make_shared<std::vector<int>>(edge);
        for (int j = 0; j < edge.size() - 1; j++) {
            auto it = std::find_if(branchList.begin(), branchList.end(), [edge, j](Branch &branch) ->
                    bool { return Branch::EqualNodes(branch, edge[j], edge[j + 1]); });
            branchList[it - branchList.begin()].GetEdges().push_back(ptr);
        }
        F.push_back(ptr);
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
    int option;
    std::cin >> option;
    if (option != 1 && option != 2 && option != 3) {
        std::cout << "Wrong number" << std::endl;
        system("pause>>void");
        return 0;
    }
    // In advance, calculate units of degree from 0 to m
    Bin.resize(m + 1, Branch::GetBranch(m + 1, 0));
    for (int i = 0; i < Bin.size(); i++) {
        Bin[i].SetPower(i);
        if (i != 0) {
            for (int j = 1; j < m + 1; j++) {
                Bin[i].GetC()[j] = Bin[i - 1].GetC()[j - 1] + Bin[i - 1].GetC()[j];
            }
        }
    }
    // Create an initialHypernet
    H initialHypernet = H(std::move(branchList), std::move(nodeList), std::move(F));
    initialHypernet.RemoveEmptyBranches();
    // In the beginning we consider only connected hypernets
    if (!initialHypernet.IsSNconnected()) {
        std::cout << "Unconnected hypernet on input!" << std::endl;
        return 0;
    }
    // Create a pseudo-edge F, which we multiply by the end of the calculations
    Branch pseudoEdge = Branch::GetBranch(0);
    unsigned int startTime = clock();
    double value = 0, p = 0.9, z = 0.1;
    Branch sum;

    try {
        // Computing pairwise connectivities
        if (option == 3) {
//            H H = initialHypernet;
//            H.RenumerateNodes(6, 0);
//            H.RenumerateNodes(7, 1);
//            if (H.IsSNconnected()) {
//                auto result = PairConnectivity(H, pseudoEdge);
//                sum = sum + result;
//            }
            //debug
//            auto customHypernet = initialHypernet;
//            //customHypernet.RenumerateNodesForGen(2, 0);
//            customHypernet.RenumerateNodesForGen(2, 1);
//            std::vector<bool> brancheMask(customHypernet.GetFN().size(), false);
//            int startPos = 0;
//            GenCombinations(customHypernet, customHypernet.GetFN(), sum, brancheMask, startPos);
            // All connectivities
            for (int i = 0; i < n; i++) {
                for (int j = i + 1; j < n; j++) {
                    auto H = initialHypernet;
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
                    if (H.IsSNconnected()) {
                        auto result = PairConnectivity(H, pseudoEdge);
                        NormalizeSolution(result);
                        for (auto &item : result.GetC()) {
                            output << std::setprecision(15) << item << " ";
                        }
                        output << std::endl;
                    }
                }
            }
            return 0;
        }
        // Computing APC
        if (option == 1) {
            for (int i = 0; i < n; i++) {
                for (int j = i + 1; j < n; j++) {
                    auto H = initialHypernet;
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
                    if (H.IsSNconnected()) {
                        auto result = PairConnectivity(H, pseudoEdge);
                        sum = sum + result;
                    }
                    //debug
//                    auto customHypernet = initialHypernet;
//                    if (i != 0 || j != 1) {
//                        if (i != 0 && j != 1) {
//                            customHypernet.RenumerateNodesForGen(i, 0);
//                            customHypernet.RenumerateNodesForGen(j, 1);
//                        }
//                        if (i == 0 && j != 1) {
//                            customHypernet.RenumerateNodesForGen(j, 1);
//                        }
//                        if (i != 0 && j == 1) {
//                            customHypernet.RenumerateNodesForGen(i, 0);
//                        }
//                    }
//                    std::vector<bool> brancheMask(customHypernet.GetFN().size(), false);
//                    int startPos = 0;
//                    GenCombinations(customHypernet, customHypernet.GetFN(), sum, brancheMask, startPos);

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
                if (i != 1) {
                    H.RenumerateNodes(i, 1);
                }
                if (H.IsSNconnected()) {
                    auto result = PairConnectivity(H, pseudoEdge);
                    sum = sum + result;
                }
                //debug
//                auto customHypernet = initialHypernet;
//                if (i != 1) {
//                    customHypernet.RenumerateNodesForGen(i, 1);
//                }
//                std::vector<bool> brancheMask(customHypernet.GetFN().size());
//                int startPos = 0;
//                GenCombinations(customHypernet, customHypernet.GetFN(), sum, brancheMask, startPos);

                //std::cout << "+R1" << i + 1 << std::endl;
            }

            sum = sum + Branch::GetUnity();
            //sum.GetC().resize(m + 1, 1);
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
    std::cout << "PairConnectivityCalls " << PairConnectivityCalls << std::endl;
    std::cout << "Reductions : " << std::endl;
    std::cout << " UnconnectedNodesReduced " << UnconnectedNodesReduced << std::endl;
    std::cout << " EdgesReduced " << EdgesReduced << std::endl;
    std::cout << " ChainsReduced " << ChainsReduced << std::endl;
//    std::cout << " UnsimpleChains " << UnsimpleChains << std::endl;
    std::cout << "Were ends of recursion : " << ReliableHypernetsCount + UnconnectedHypernetsCount + TwoNodesHypernetsCount << std::endl;
    std::cout << " ReliableHypernetsCount " << ReliableHypernetsCount << std::endl;
    std::cout << " UnconnectedHypernetsCount " << UnconnectedHypernetsCount << std::endl;
    std::cout << " TwoNodesHypernetsCount " << TwoNodesHypernetsCount << std::endl;
    std::cout << "Solution:" << std::endl;
    if (!sum.IsZero()) {
        NormalizeSolution(sum);
        sum.PrintBranch();
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

    system("pause>>void");
    return 0;
}