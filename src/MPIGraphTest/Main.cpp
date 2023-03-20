#include "stdafx.h"
#include "funcs.h"
#include "globals.h"
#include "../Helpers/InputParser.h"
#include "ApplicationSettings.h"
#include "../Helpers/Settings.h"

int n, m;
int Num0, Num2, Num3, Num4, Num5, Chr, Ch1, Ch2, Chrs,
        Decomp1, Decomp2, Decomp3, Bridges, Pendunt, Factors;
vector<bool> Visited;
vector<vector<bool>> Mask1;
vector<bool> Mask2;
vector<Edge> Bin;
Edge Sum;
double averageTime;
Settings AppSettings;
ifstream input;
ofstream output;

void MakeAdjacencyMatrix(vector<vector<Edge>> &H, vector<Edge> &E) {
    for (int i = 0; i < H.size(); i++) {
        H[i].resize(H.size());
    }
    for (int i = 0; i < H.size(); i++) {
        for (int j = 0; j < H[i].size(); j++) {
            H[i][j].Ex = false;
            for (int w = 0; w < E.size(); w++) {
                if ((i == E[w].Node1 && j == E[w].Node2) || (i == E[w].Node2 && j == E[w].Node1)) {
                    H[i][j] = E[w];
                }
            }
        }
    }
}

void ComputeBinomialCoefficients() {
    Bin.resize(m + 1);
    for (int i = 0; i < Bin.size(); i++) {
        Bin[i].C.resize(m + 1);
        Bin[i].Power = i;
        Bin[i].Simple = 0;
        Bin[i].Ex = true;
        Bin[i].C.front() = 1;
        if (i != 0) {
            for (int j = 1; j < m + 1; j++) {
                Bin[i].C[j] = Bin[i - 1].C[j - 1] + Bin[i - 1].C[j];
            }
        }
    }
}

Str GetData() {
    Str Gp;
    if (!input.is_open()) {
        throw "GetData: File can not be opened!";
    }

    char string[50];
    vector<Edge> E;
    input.getline(string, 50);
    cout << "Input graph : " << string << endl;
    int buff, count = 0;
    input >> buff;
    n = buff;
    input >> buff;
    m = buff;

    while (count != m) {
        bool reduct = false;
        Edge q;
        q.Simple = 0;
        q.Ex = true;
        q.Power = 1;
        q.C.push_back(1);
        q.C.resize(m + 1);
        input >> buff;
        q.Node1 = buff - 1;
        input >> buff;
        q.Node2 = buff - 1;
        for (int i = 0; i < E.size(); i++) {
            if ((E[i].Node1 == q.Node1 && E[i].Node2 == q.Node2) || (E[i].Node1 == q.Node2 && E[i].Node2 == q.Node1)) {
                E[i].Simple++;
                reduct = true;
            }
        }
        if (!reduct) {
            E.push_back(q);
        }
        count++;
    }

    input >> string;
    if (strcmp(string, "$$$") != 0) {
        cout << "Uncorrect entry" << endl;
        return Gp;
    }
    cout << "Correct entry" << endl;

    ComputeBinomialCoefficients();

    vector<vector<Edge>> S(n);
    MakeAdjacencyMatrix(S, E);
    bool r = IsConnected(S);
    if (!r) {
        cout << "Unconnected graph on input!" << endl;
    }
    Gp.Graph = S;
    Gp.F.push_back(Bin.front());
    Gp.Ex = r;
    Gp.Visited = Visited;

    return Gp;
}

Str GetRandomGraph() {
    Str Gp;
    srand(time(0)); // Randomized generator of random numbers
    vector<Edge> E;

    vector<int> P(n);
    vector<int> L;
    for (int i = 0; i < P.size(); i++) P[i] = i;

    int q = rand() % (P.size() - 1);
    for (int i = 0; i < n - 1; i++) {
        Edge newEdge;
        bool reduct = false;
        L.push_back(P[q]);
        P.erase(P.begin() + q);

        int s = 0, t = 0;
        if (P.size() - 1 != 0) s = rand() % (P.size() - 1);
        if (L.size() - 1 != 0) t = rand() % (L.size() - 1);
        q = s;

        newEdge.Simple = 0;
        newEdge.Ex = true;
        newEdge.Power = 1;
        newEdge.C.push_back(1);
        newEdge.C.resize(m + 1);
        newEdge.Node1 = P[s];
        newEdge.Node2 = L[t];
        E.push_back(newEdge);
    }

    for (int i = 0; i < m - n + 1; i++) {
        Edge newEdge;
        bool reduct = false;
        int s = 0, t = 0;
        s = rand() % n;
        t = rand() % n;
        while (s == t) t = rand() % n;

        newEdge.Simple = 0;
        newEdge.Ex = true;
        newEdge.Power = 1;
        newEdge.C.push_back(1);
        newEdge.C.resize(m + 1);
        newEdge.Node1 = s;
        newEdge.Node2 = t;
        for (int i = 0; i < E.size(); i++) // work with multi edges
            if ((E[i].Node1 == newEdge.Node1 && E[i].Node2 == newEdge.Node2) || (E[i].Node1 == newEdge.Node2 && E[i].Node2 == newEdge.Node1)) {
                E[i].Simple++;
                reduct = true;
            }
        if (!reduct) E.push_back(newEdge);
    }

    vector<vector<Edge>> S(n);
    MakeAdjacencyMatrix(S, E);

    bool r = IsConnected(S);
    if (!r) cout << "Unconnected graph generate!" << endl;

    Gp.Graph = S;
    Gp.F.push_back(Bin.front());
    Gp.Ex = r;
    Gp.Visited = Visited;

    return Gp;
}

void SendControl(Str H, int bufSize, int vectorSize, int size) {
    stack<int> freeProcessors;
    for (int i = 1; i < size; ++i) {
        freeProcessors.push(i);
    }

    int strBufSize = H.F.size() * bufSize + sizeof(int) + sizeof(bool);
    if (!H.Ex) {
        strBufSize += H.Graph.size() * sizeof(bool);
    }
    for (int i = 1; i < H.Graph.size(); i++) {
        strBufSize += i * bufSize + sizeof(int);
    }
   auto bufStr = PackData(H, strBufSize, bufSize, vectorSize);
    MPI_Send(bufStr, strBufSize, MPI_PACKED, freeProcessors.top(), GP_INFO_TAG, MPI_COMM_WORLD);
    delete[] bufStr;

    freeProcessors.pop();

    int justValue;
    MPI_Status status;
    while (freeProcessors.size() != size - 1) {
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        if (status.MPI_TAG == I_AM_FREE_TAG) {
            MPI_Recv(&justValue, 0, MPI_INT, status.MPI_SOURCE, I_AM_FREE_TAG, MPI_COMM_WORLD, &status);
            freeProcessors.push(status.MPI_SOURCE);
        } else if (status.MPI_TAG == I_NEED_HELP_TAG) {
            MPI_Recv(&justValue, 0, MPI_INT, status.MPI_SOURCE, I_NEED_HELP_TAG, MPI_COMM_WORLD, &status);
            int helpProcessor = 0;
            if (freeProcessors.size() > 0) {
                helpProcessor = freeProcessors.top();
                freeProcessors.pop();
            }
            MPI_Send(&helpProcessor, 1, MPI_INT, status.MPI_SOURCE, I_NEED_HELP_TAG, MPI_COMM_WORLD);
        }
    }
}

void ComputeAPC (Str &Gp, int &bufSize, int &vectorSize, int &size) {
    Mask1.resize(Gp.Graph.size());
    for (int i = 0; i < Mask1.size(); i++) {
        Mask1[i].resize(Gp.Graph[i].size());
        for (int j = 0; j < Mask1[i].size(); j++) i != j ? Mask1[i][j] = true : Mask1[i][j] = false;
    }
    vector<int> V;
    ChainReduction1(Gp.Graph, V, Gp.Ex, bufSize, vectorSize, size);
    for (int i = 0; i < Gp.Graph.size(); i++) {
        for (int j = i + 1; j < Gp.Graph[i].size(); j++) {
            if (Mask1[i][j]) {
                Str H(Gp);
                if (i != 0 || j != 1) {
                    if (i != 0 && j != 1) {
                        Renumerate(H.Graph, i, 0);
                        Renumerate(H.Graph, j, 1);
                    }
                    if (i == 0 && j != 1) {
                        Renumerate(H.Graph, j, 1);
                    }
                    if (i != 0 && j == 1) {
                        Renumerate(H.Graph, i, 0);
                    }
                }
                SendControl(H, bufSize, vectorSize, size);
            }
        }
    }
}

void ComputeMENC (Str &Gp, int &bufSize, int &vectorSize, int &size) {
    Mask2.resize(Gp.Graph.size());
    for (int i = 0; i < Mask2.size(); i++) {
        i == 0 ? Mask2[i] = false : Mask2[i] = true;
    }
    vector<int> weight(Gp.Graph.size());
    for (int i = 0; i < weight.size(); i++) {
        weight[i] = 1;
    }
    ChainReduction2(Gp.Graph, weight, Gp.Ex, bufSize, vectorSize, size);
    for (int i = 0; i < Gp.Graph.size(); i++) {
        if (Mask2[i]) {
            Str H(Gp);
            if (i != 1) Renumerate(H.Graph, i, 1);
            SendControl(H, bufSize, vectorSize, size);
        }
    }
}

void ComputeGraph(Str &Gp, int &bufSize, int &vectorSize, int &size, int &option) {
    if (option == 1) {
        ComputeAPC(Gp, bufSize, vectorSize, size);
    } else if (option == 2) {
        ComputeMENC(Gp, bufSize, vectorSize, size);
    }
}

void Master(int size) {
    setlocale(LC_ALL, "");

    Str Gp;
    if (IS_TEST_TIME == 1) {
        n = TEST_GRAPH_NODE_SIZE;
        m = TEST_GRAPH_EDGE_SIZE;
        ComputeBinomialCoefficients();
    } else {
        Gp = GetData();
        if (Gp.Graph.empty()) {
            return;
        }
    }

    if (IS_TEST_TIME == 1) {
        std::cout << "Press 1 to test APC polynomials" << std::endl;
        std::cout << "Press 2 to test MENC polynomials" << std::endl;
    } else {
        std::cout << "Press 1 to get APC polynomial" << std::endl;
        std::cout << "Press 2 to get MENC polynomial" << std::endl;
    }
    int option;
    std::cin >> option;
    if (option != 1 && option != 2) {
        std::cout << "Wrong number" << std::endl;
        return;
    }

    int vectorSize = Bin.front().C.size();
    int bufSize = vectorSize * sizeof(double) + 2 * sizeof(int) + sizeof(bool);
    MPI_Bcast(&bufSize, 1, MPI_INT, HOST_PROCESSOR, MPI_COMM_WORLD);
    MPI_Bcast(&vectorSize, 1, MPI_INT, HOST_PROCESSOR, MPI_COMM_WORLD);
    int vectorBufSize = Bin.size() * bufSize + sizeof(int);
    MPI_Bcast(&vectorBufSize, 1, MPI_INT, HOST_PROCESSOR, MPI_COMM_WORLD);
    auto buf_vector = PackVector(Bin, bufSize, vectorSize);
    MPI_Bcast(buf_vector, vectorBufSize, MPI_PACKED, HOST_PROCESSOR, MPI_COMM_WORLD);
    delete[] buf_vector;
    double startTime;

    if (IS_TEST_TIME == 1) {
        for (int i = 0; i < TEST_GRAPH_COUNT; i++) {
            Gp = GetRandomGraph();
            startTime = MPI_Wtime();
            ComputeGraph(Gp, bufSize, vectorSize, size, option);
            double endTime = MPI_Wtime();
            averageTime += endTime - startTime;
            output << "Time = " << endTime - startTime << endl;
        }
    } else {
        startTime = MPI_Wtime();
        ComputeGraph(Gp, bufSize, vectorSize, size, option);
    }

    for (int i = 1; i < size; i++) {
        MPI_Send(&i, 0, MPI_INT, i, STOP_TAG, MPI_COMM_WORLD);
    }

    if (IS_TEST_TIME == 1) {
        averageTime = averageTime / TEST_GRAPH_COUNT;
        cout << "Average time = " << averageTime;
        output << "Average time = " << averageTime;
    } else {
        MPI_Status status;
        for (int i = 1; i < size; i++) {
            auto buf = new char[bufSize];
            MPI_Recv(buf, bufSize, MPI_PACKED, MPI_ANY_SOURCE, SUM_TAG, MPI_COMM_WORLD, &status);
            Edge s = Unpack(buf, bufSize, vectorSize, 0);
            delete[] buf;
            Sum = Sum + s;
            int buff;
            MPI_Recv(&buff, 1, MPI_INT, MPI_ANY_SOURCE, NUM0_TAG, MPI_COMM_WORLD, &status);
            Num0 += buff;
            MPI_Recv(&buff, 1, MPI_INT, MPI_ANY_SOURCE, NUM2_TAG, MPI_COMM_WORLD, &status);
            Num2 += buff;
            MPI_Recv(&buff, 1, MPI_INT, MPI_ANY_SOURCE, NUM3_TAG, MPI_COMM_WORLD, &status);
            Num3 += buff;
            MPI_Recv(&buff, 1, MPI_INT, MPI_ANY_SOURCE, NUM4_TAG, MPI_COMM_WORLD, &status);
            Num4 += buff;
            MPI_Recv(&buff, 1, MPI_INT, MPI_ANY_SOURCE, NUM5_TAG, MPI_COMM_WORLD, &status);
            Num5 += buff;
            MPI_Recv(&buff, 1, MPI_INT, MPI_ANY_SOURCE, CHR_TAG, MPI_COMM_WORLD, &status);
            Chr += buff;
            MPI_Recv(&buff, 1, MPI_INT, MPI_ANY_SOURCE, CH1_TAG, MPI_COMM_WORLD, &status);
            Ch1 += buff;
            MPI_Recv(&buff, 1, MPI_INT, MPI_ANY_SOURCE, CH2_TAG, MPI_COMM_WORLD, &status);
            Ch2 += buff;
            MPI_Recv(&buff, 1, MPI_INT, MPI_ANY_SOURCE, CHRS_TAG, MPI_COMM_WORLD, &status);
            Chrs += buff;
            MPI_Recv(&buff, 1, MPI_INT, MPI_ANY_SOURCE, DECOMP1_TAG, MPI_COMM_WORLD, &status);
            Decomp1 += buff;
            MPI_Recv(&buff, 1, MPI_INT, MPI_ANY_SOURCE, DECOMP2_TAG, MPI_COMM_WORLD, &status);
            Decomp2 += buff;
            MPI_Recv(&buff, 1, MPI_INT, MPI_ANY_SOURCE, DECOMP3_TAG, MPI_COMM_WORLD, &status);
            Decomp3 += buff;
            MPI_Recv(&buff, 1, MPI_INT, MPI_ANY_SOURCE, BRIDGES_TAG, MPI_COMM_WORLD, &status);
            Bridges += buff;
            MPI_Recv(&buff, 1, MPI_INT, MPI_ANY_SOURCE, PENDUNT_TAG, MPI_COMM_WORLD, &status);
            Pendunt += buff;
            MPI_Recv(&buff, 1, MPI_INT, MPI_ANY_SOURCE, FACTORS_TAG, MPI_COMM_WORLD, &status);
            Factors += buff;
        }
        if (option == 1) {
            for (int i = 0; i < Sum.C.size(); i++) {
                Sum.C[i] = Sum.C[i] / Bin[Gp.Graph.size()].C[2];
            }
        } else if (option == 2) {
            Sum = Sum + Bin.front();
        }
        double endTime = MPI_Wtime();
        cout << "Time of programm : " << endTime - startTime << endl;
        cout << "Call prosedure " << Factors << endl;
        cout << "Reductions : " << endl;
        cout << " reduced chains initially " << Chrs << endl;
        cout << " bridge reductions " << Bridges << endl;
        cout << " Pendunt reducutions " << Pendunt << endl;
        cout << " chains reduced " << Chr << endl;
        cout << " 1-st type " << Ch1 << endl;
        cout << " 2-nd type " << Ch2 << endl;
        cout << " decompositions in ChainReduction " << Decomp1 << endl;
        cout << " decompositions in Chr " << Decomp2 << endl;
        cout << " decompositions in factoring " << Decomp3 << endl;
        cout << "Were ends of recursion : " << Num0 + Num2 + Num3 + Num4 + Num5 << endl;
        cout << " unconnected graphs " << Num0 << endl;
        cout << " 2-dimension graphs " << Num2 << endl;
        cout << " 3-dimension graphs " << Num3 << endl;
        cout << " 4-dimension graphs " << Num4 << endl;
        cout << " 5-dimension graphs " << Num5 << endl;

        double value = 0, p = AppSettings.ReliabilityValue, z = 1 - AppSettings.ReliabilityValue;
        int q = Sum.Power;
        for (int i = 0; i < q; i++) {
            value += Sum.C[i] * pow(p, q - i) * pow(z, i);
        }
        cout << "Value at point " << p << ": " << fixed << setprecision(15) << value << endl;
        for (int i = 0; i < Sum.C.size(); i++) {
            output << Sum.C[i] << " ";
        }
    }
}

void Slaves(int rank) {
    int bufSize, vectorSize, vectorBufSize;
    MPI_Bcast(&bufSize, 1, MPI_INT, HOST_PROCESSOR, MPI_COMM_WORLD);
    MPI_Bcast(&vectorSize, 1, MPI_INT, HOST_PROCESSOR, MPI_COMM_WORLD);
    MPI_Bcast(&vectorBufSize, 1, MPI_INT, HOST_PROCESSOR, MPI_COMM_WORLD);

    auto bufVector = new char[vectorBufSize];
    MPI_Bcast(bufVector, vectorBufSize, MPI_PACKED, HOST_PROCESSOR, MPI_COMM_WORLD);
    Bin = UnpackVector(bufVector, bufSize, vectorSize, 0);
    delete[] bufVector;

    int justValue;
    MPI_Status status;
    do {
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        if (status.MPI_TAG == GP_INFO_TAG) {
            int getSize;
            MPI_Get_count(&status, MPI_PACKED, &getSize);
            auto bufStr = new char[getSize];
            MPI_Recv(bufStr, getSize, MPI_PACKED, MPI_ANY_SOURCE, GP_INFO_TAG, MPI_COMM_WORLD, &status);
            Str Gp = UnpackData(bufStr, bufSize, vectorSize);
            delete[] bufStr;
            if (!Gp.Ex) Visited = Gp.Visited;
            Sum = Sum + PairwiseConnectivity(Gp.Graph, Gp.F.front(), Gp.Ex, bufSize, vectorSize);
            MPI_Send(&justValue, 0, MPI_INT, HOST_PROCESSOR, I_AM_FREE_TAG, MPI_COMM_WORLD);
        }
    } while (status.MPI_TAG != STOP_TAG);

    auto buf_edge = Pack(Sum, bufSize, vectorSize);
    MPI_Send(buf_edge, bufSize, MPI_PACKED, HOST_PROCESSOR, SUM_TAG, MPI_COMM_WORLD);
    delete[] buf_edge;
    MPI_Send(&Num0, 1, MPI_INT, HOST_PROCESSOR, NUM0_TAG, MPI_COMM_WORLD);
    MPI_Send(&Num2, 1, MPI_INT, HOST_PROCESSOR, NUM2_TAG, MPI_COMM_WORLD);
    MPI_Send(&Num3, 1, MPI_INT, HOST_PROCESSOR, NUM3_TAG, MPI_COMM_WORLD);
    MPI_Send(&Num4, 1, MPI_INT, HOST_PROCESSOR, NUM4_TAG, MPI_COMM_WORLD);
    MPI_Send(&Num5, 1, MPI_INT, HOST_PROCESSOR, NUM5_TAG, MPI_COMM_WORLD);
    MPI_Send(&Chr, 1, MPI_INT, HOST_PROCESSOR, CHR_TAG, MPI_COMM_WORLD);
    MPI_Send(&Ch1, 1, MPI_INT, HOST_PROCESSOR, CH1_TAG, MPI_COMM_WORLD);
    MPI_Send(&Ch2, 1, MPI_INT, HOST_PROCESSOR, CH2_TAG, MPI_COMM_WORLD);
    MPI_Send(&Chrs, 1, MPI_INT, HOST_PROCESSOR, CHRS_TAG, MPI_COMM_WORLD);
    MPI_Send(&Decomp1, 1, MPI_INT, HOST_PROCESSOR, DECOMP1_TAG, MPI_COMM_WORLD);
    MPI_Send(&Decomp2, 1, MPI_INT, HOST_PROCESSOR, DECOMP2_TAG, MPI_COMM_WORLD);
    MPI_Send(&Decomp3, 1, MPI_INT, HOST_PROCESSOR, DECOMP3_TAG, MPI_COMM_WORLD);
    MPI_Send(&Bridges, 1, MPI_INT, HOST_PROCESSOR, BRIDGES_TAG, MPI_COMM_WORLD);
    MPI_Send(&Pendunt, 1, MPI_INT, HOST_PROCESSOR, PENDUNT_TAG, MPI_COMM_WORLD);
    MPI_Send(&Factors, 1, MPI_INT, HOST_PROCESSOR, FACTORS_TAG, MPI_COMM_WORLD);
}

void SetGlobals(int argc, char** argv, int rank) {
    n = 0;
    m = 0;
    Num0 = 0;
    Num2 = 0;
    Num3 = 0;
    Num4 = 0;
    Num5 = 0;
    Chr = 0;
    Ch1 = 0;
    Ch2 = 0;
    Chrs = 0;
    Decomp1 = 0;
    Decomp2 = 0;
    Decomp3 = 0;
    Bridges = 0;
    Pendunt = 0;
    Factors = 0;

    InputParser inputParser(argc, argv);
    std::string str;
    str = inputParser.getCmdOption("-p");
    AppSettings.ReliabilityValue = !str.empty() ? std::stod(str) : RELIABILITY_VALUE;

    if (rank == 0) {
        str = inputParser.getCmdOption("-input");
        input.open(!str.empty() ? str : "input.txt");
        str = inputParser.getCmdOption("-output");
        output.open(!str.empty() ? str : "output.txt");
    }
}

void ErrorHandler(const char *str) {
    std::cout << "--------------------------------" << std::endl;
    std::cout << "Occurred next error:" << std::endl;
    std::cout << str << std::endl;
    std::cout << "--------------------------------" << std::endl;
}

int main(int argc, char **argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    try {
        SetGlobals(argc, argv, rank);

        if (rank == 0) {
            Master(size);
        } else {
            Slaves(rank);
        }
    }
    catch (const char *str) {
        ErrorHandler(str);

        return EXIT_FAILURE;
    }

    MPI_Finalize();
    input.close();
    output.close();
    return 0;
}