#include "stdafx.h"
#include "funcs.h"
#include "globals.h"
#include "Cclass.h"

ofstream output("C://output.txt");
ifstream input("C://input.txt");

int num0 = 0, num2 = 0, num3 = 0, num4 = 0, num5 = 0, chr = 0, ch1 = 0, ch2 = 0, chrs = 0,
decomp1 = 0, decomp2 = 0, decomp3 = 0, bridges = 0, pendunt = 0, factors = 0;
double avr_time;
vector<bool> visited;
vector<vector<bool>> mask1;
vector<bool> mask;
vector<edge> Bin;
edge sum;

void edge::outprintedge() {
	if (ex == 1) { // can be 204
		for (int i = 0; i < C.size(); i++) output << C[i] << " ";
		output << endl << "power = " << power << ", simple = " << simple << ", ex = " << ex << endl;
	}
	else output << "empty edge" << endl;
}

void str::outprintstr() {
	for (int i = 0; i < G.size(); i++) {
		output << "H.G[" << i << "] : " << endl;
		for (int j = 0; j < G[i].size(); j++) G[i][j].outprintedge();
	}
	output << "H.F : " << endl;
	for (int i = 0; i < F.size(); i++) F[i].outprintedge();
	output << "H.ex = " << ex << endl;
	output << "H.V : ";
	for (int i = 0; i < V.size(); i++) output << V[i] << " ";
	output << endl;
}

void table(vector<vector<edge>> &H, vector<edge> &E)
{
	for (int i = 0; i<H.size(); i++) H[i].resize(H.size());
	//initialize all strings, so as array is associative, not fill by nulls, or it is not global
	for (int i = 0; i<H.size(); i++) for (int j = 0; j<H[i].size(); j++) {
		H[i][j].ex = false;
		for (int w = 0; w<E.size(); w++)
			if ((i == E[w].node1 && j == E[w].node2) || (i == E[w].node2 && j == E[w].node1)) H[i][j] = E[w];
	}
}

str get_graph()
{
	str Gp;

	srand(time(0)); // Randomized generator of random numbers
	int n = 25, m = 30;
	vector<edge> E;
	output << "G(" << n << "," << m << ")" << endl;
	output << n << " " << m << endl;

	vector<int> P(n);
	vector<int> L;
	for (int i = 0; i < P.size(); i++) P[i] = i;

	int q = rand() % (P.size() - 1);
	for (int i = 0; i < n - 1; i++) {
		edge newedge;
		bool reduct = false;
		L.push_back(P[q]);
		P.erase(P.begin() + q);

		int s = 0, t = 0;
		if (P.size() - 1 != 0) s = rand() % (P.size() - 1);
		if (L.size() - 1 != 0) t = rand() % (L.size() - 1);
		q = s;

		newedge.simple = 0;
		newedge.ex = true;
		newedge.power = 1;
		newedge.C.push_back(1);
		newedge.C.resize(m + 1);
		newedge.node1 = P[s];
		newedge.node2 = L[t];
		output << newedge.node1 + 1 << " " << newedge.node2 + 1 << endl;
		E.push_back(newedge);
	}

	for (int i = 0; i < m - n + 1; i++) {
		edge newedge;
		bool reduct = false;
		int s = 0, t = 0;
		s = rand() % n;
		t = rand() % n;
		while (s == t) t = rand() % n;

		newedge.simple = 0;
		newedge.ex = true;
		newedge.power = 1;
		newedge.C.push_back(1);
		newedge.C.resize(m + 1);
		newedge.node1 = s;
		newedge.node2 = t;
		output << newedge.node1 + 1 << " " << newedge.node2 + 1 << endl;
		for (int i = 0; i < E.size(); i++) // work with multi edges
			if ((E[i].node1 == newedge.node1 && E[i].node2 == newedge.node2) || (E[i].node1 == newedge.node2 && E[i].node2 == newedge.node1)) {
				E[i].simple++;
				output << "E[" << i << "].simple=" << E[i].simple << endl;
				reduct = true;
			}
		if (!reduct) E.push_back(newedge);
	}
	output << "$$$" << endl;

	vector<vector<edge>> S(n);
	table(S, E);

	bool r = gconnected(S);
	if (!r) cout << "Unconnected graph generate!" << endl;

	Gp.G = S;
	Gp.F.push_back(Bin.front());
	Gp.ex = r;
	Gp.V = visited;

	return Gp;
}

void send_control(str H, int buf_size, int vector_size, int size)
{
	stack<int> free_processors; // free proc
	for (int i = 1; i < size; ++i) free_processors.push(i);

	int str_buf_size = H.F.size()*buf_size + sizeof(int) + sizeof(bool);
	if (!H.ex) str_buf_size += H.G.size() * sizeof(bool);
	for (int i = 1; i < H.G.size(); i++) str_buf_size += i*buf_size + sizeof(int);
	char* buf_str = Pack_data(H, str_buf_size, buf_size, vector_size);
	MPI_Send(buf_str, str_buf_size, MPI_PACKED, free_processors.top(), GPINFO_TAG, MPI_COMM_WORLD);
	delete[] buf_str;

	free_processors.pop();

	int just_value;
	MPI_Status status;
	while (free_processors.size() != size - 1) {
		MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status); // проверяем, что нам пришло

		if (status.MPI_TAG == I_AM_FREE_TAG) { // if somebody ger free
			MPI_Recv(&just_value, 0, MPI_INT, status.MPI_SOURCE, I_AM_FREE_TAG, MPI_COMM_WORLD, &status); // добавляем свободный процессор обратно в стек/очередь/etc
			free_processors.push(status.MPI_SOURCE);
		}
		else if (status.MPI_TAG == I_NEED_HELP_TAG) { // if somebody need help
			MPI_Recv(&just_value, 0, MPI_INT, status.MPI_SOURCE, I_NEED_HELP_TAG, MPI_COMM_WORLD, &status);

			int help_processor = 0;
			if (free_processors.size() > 0) { // if have free
				help_processor = free_processors.top();
				free_processors.pop();
			}

			MPI_Send(&help_processor, 1, MPI_INT, status.MPI_SOURCE, I_NEED_HELP_TAG, MPI_COMM_WORLD); // отправляем ноль или номер процесса
		}
	}
}

void master(int size)
{
	int m = 30;
	Bin.resize(m + 1);
	for (int i = 0; i < Bin.size(); i++) {
		Bin[i].C.resize(m + 1);
		Bin[i].power = i;
		Bin[i].simple = 0;
		Bin[i].ex = true;
		Bin[i].C.front() = 1;
		if (i != 0) for (int j = 1; j < m + 1; j++) Bin[i].C[j] = Bin[i - 1].C[j - 1] + Bin[i - 1].C[j];
	}

	int vector_size = Bin.front().C.size();
	int buf_size = vector_size * sizeof(double) + 2 * sizeof(int) + sizeof(bool);

	MPI_Bcast(&buf_size, 1, MPI_INT, HOST_PROCESSOR, MPI_COMM_WORLD);
	MPI_Bcast(&vector_size, 1, MPI_INT, HOST_PROCESSOR, MPI_COMM_WORLD);
	int vector_buf_size = Bin.size()*buf_size + sizeof(int);
	MPI_Bcast(&vector_buf_size, 1, MPI_INT, HOST_PROCESSOR, MPI_COMM_WORLD);
	char* buf_vector = Pack_vector(Bin, buf_size, vector_size);
	MPI_Bcast(buf_vector, vector_buf_size, MPI_PACKED, HOST_PROCESSOR, MPI_COMM_WORLD);
	delete[] buf_vector;

	int count = 30;
	for (int i = 0; i < count; i++) {
		str Gp = get_graph();
		double startTime = MPI_Wtime();
/*
		mask1.resize(Gp.G.size());
		for (int i = 0; i < mask1.size(); i++) {
			mask1[i].resize(Gp.G.size());
			for (int j = 0; j < mask1[i].size(); j++) i != j ? mask1[i][j] = true : mask1[i][j] = false;
		}

		vector<int> V;
		chainreduction1(Gp.G, V, Gp.ex, buf_size, vector_size, size);

		for (int i = 0; i < Gp.G.size(); i++) for (int j = i + 1; j < Gp.G[i].size(); j++) if (mask1[i][j]) {
			//cout << "+R" << i + 1 << j + 1 << endl;
			str H(Gp);
			if (i != 0 || j != 1) { // after this 0,1
				if (i != 0 && j != 1) {
					renumerate(H.G, i, 0);
					renumerate(H.G, j, 1);
				}
				if (i == 0 && j != 1) renumerate(H.G, j, 1);
				if (i != 0 && j == 1) renumerate(H.G, i, 0);
			}

			send_control(H, buf_size, vector_size, size);
		}
*/

		mask.resize(Gp.G.size());
		for (int i = 0; i<mask.size(); i++) i == 0 ? mask[i] = false : mask[i] = true;

		vector<int> weight(Gp.G.size());
		for (int i = 0; i<weight.size(); i++) weight[i] = 1;

		chainreduction2(Gp.G, weight, Gp.ex, buf_size, vector_size, size);

		for (int i = 0; i < Gp.G.size(); i++) if (mask[i]) {
			//cout << "+R1" << i + 1 << endl;
			str H(Gp);
			if (i != 1) renumerate(H.G, i, 1); // after this i=1

			send_control(H, buf_size, vector_size, size);
		}
	
		double endTime = MPI_Wtime();
		avr_time += endTime - startTime;
		output << "Time of programm : " << endTime - startTime << endl;

		//mask1.~vector();
	}

	for (int i = 1; i < size; i++) MPI_Send(&i, 0, MPI_INT, i, STOP_TAG, MPI_COMM_WORLD);

	avr_time = avr_time / count;
	cout << "Avarage time = " << avr_time;

}

void slaves(int rank)
{
	int buf_size, vector_size, vector_buf_size;
	MPI_Bcast(&buf_size, 1, MPI_INT, HOST_PROCESSOR, MPI_COMM_WORLD);
	MPI_Bcast(&vector_size, 1, MPI_INT, HOST_PROCESSOR, MPI_COMM_WORLD);
	MPI_Bcast(&vector_buf_size, 1, MPI_INT, HOST_PROCESSOR, MPI_COMM_WORLD);

	char *buf_vector = new char[vector_buf_size];
	MPI_Bcast(buf_vector, vector_buf_size, MPI_PACKED, HOST_PROCESSOR, MPI_COMM_WORLD);
	Bin = Unpack_vector(buf_vector, buf_size, vector_size, 0);
	delete[] buf_vector;

	int just_value;
	MPI_Status status;
	do { // have 2 tags
		MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

		if (status.MPI_TAG == GPINFO_TAG) {
			int get_size;
			MPI_Get_count(&status, MPI_PACKED, &get_size);
			char *buf_str = new char[get_size];
			MPI_Recv(buf_str, get_size, MPI_PACKED, MPI_ANY_SOURCE, GPINFO_TAG, MPI_COMM_WORLD, &status);
			str Gp = Unpack_data(buf_str, buf_size, vector_size);
			delete[] buf_str;
			if (!Gp.ex) visited = Gp.V;

			sum = sum + procedure(Gp.G, Gp.F.front(), Gp.ex, buf_size, vector_size);

			MPI_Send(&just_value, 0, MPI_INT, HOST_PROCESSOR, I_AM_FREE_TAG, MPI_COMM_WORLD);
		}
	} while (status.MPI_TAG != STOP_TAG);
}

int main(int argc, char **argv)
{
	int rank, size;
	MPI_Init(&argc, &argv); // выполняет инициализацию библиотеки MPI, работает с начала main
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // возвращает идентификатор (номер, ранг) процесса в рамках заданного коммуникатора
	MPI_Comm_size(MPI_COMM_WORLD, &size); // OUT size

	rank == 0 ? master(size) : slaves(rank);

	MPI_Finalize();

	input.close();
	output.close();
	return 0;
}