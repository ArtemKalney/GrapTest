#include "stdafx.h"
#include "funcs.h"
#include "globals.h"
#include "Cclass.h"

ofstream output("C://output.txt");
ifstream input("C://input.txt");

int num0 = 0, num2 = 0, num3 = 0, num4 = 0, num5 = 0, chr = 0, ch1 = 0, ch2 = 0, chrs = 0,
decomp1 = 0, decomp2 = 0, decomp3 = 0, bridges = 0, pendunt = 0, factors = 0;
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

str get_info()
{
	str Gp;

	if (!input.is_open()) cout << "File can not be opened!\n";
	else {
		char string[50];
		int n = 0, m = 0;
		vector<edge> E;
		input.getline(string, 50);
		int buff, count = 0;
		input >> buff; output << "Number of nodes n=" << buff << endl;
		n = buff;
		input >> buff; output << "Number of edges m=" << buff << endl;
		m = buff;

		while (count != m) {
			bool reduct = false;
			edge q;
			q.simple = 0;
			q.ex = true;
			q.power = 1;
			q.C.push_back(1);
			q.C.resize(m + 1);
			input >> buff;
			q.node1 = buff - 1;
			input >> buff;
			q.node2 = buff - 1;
			for (int i = 0; i < E.size(); i++) // work with multi edges
				if ((E[i].node1 == q.node1 && E[i].node2 == q.node2) || (E[i].node1 == q.node2 && E[i].node2 == q.node1)) {
					E[i].simple++;
					output << "E[" << i << "].simple=" << E[i].simple << endl;
					reduct = true;
				}
			if (!reduct) E.push_back(q);
			count++;
		}

		input >> string;
		if (strcmp(string, "$$$") != 0) cout << "Uncorrect entry" << endl; // 0 = ; 1 < ; -1 >
		else {
			cout << "Correct entry" << endl;

			Bin.resize(m + 1);
			for (int i = 0; i < Bin.size(); i++) {
				Bin[i].C.resize(m + 1);
				Bin[i].power = i;
				Bin[i].simple = 0;
				Bin[i].ex = true;
				Bin[i].C.front() = 1;
				if (i != 0) for (int j = 1; j < m + 1; j++) Bin[i].C[j] = Bin[i - 1].C[j - 1] + Bin[i - 1].C[j];
			}

			output << "Bin:" << endl;
			for (int i = 0; i<Bin.size(); i++) {
				for (int j = 0; j<Bin[i].C.size(); j++) output << Bin[i].C[j] << " ";
				output << endl << "power = " << Bin[i].power << ", simple = " << Bin[i].simple << endl;
			}

			output << "Simple edges:" << endl;
			for (int i = 0; i<E.size(); i++) output << E[i].simple << " ";
			output << endl;

			vector<vector<edge>> S(n);
			table(S, E);

			output << "Adjacency matrix:" << endl;
			for (int i = 0; i<S.size(); i++) { for (int j = 0; j<S[i].size(); j++) output << S[i][j].ex << " "; output << endl; }

			bool r = gconnected(S);
			if (!r) cout << "Unconnected graph on input!" << endl;

			Gp.G = S;
			Gp.F.push_back(Bin.front());
			Gp.ex = r;
			Gp.V = visited;

			return Gp;
		}
		return Gp;
	}
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
	str Gp = get_info();
	if (!Gp.G.empty()) {
		int vector_size = Bin.front().C.size();
		int buf_size = vector_size * sizeof(double) + 2 * sizeof(int) + sizeof(bool);

		MPI_Bcast(&buf_size, 1, MPI_INT, HOST_PROCESSOR, MPI_COMM_WORLD);
		MPI_Bcast(&vector_size, 1, MPI_INT, HOST_PROCESSOR, MPI_COMM_WORLD);
		int vector_buf_size = Bin.size()*buf_size + sizeof(int);
		MPI_Bcast(&vector_buf_size, 1, MPI_INT, HOST_PROCESSOR, MPI_COMM_WORLD);
		char* buf_vector = Pack_vector(Bin, buf_size, vector_size);
		MPI_Bcast(buf_vector, vector_buf_size, MPI_PACKED, HOST_PROCESSOR, MPI_COMM_WORLD);
		delete[] buf_vector;
		double startTime = MPI_Wtime();

		mask1.resize(Gp.G.size());
		for (int i = 0; i < mask1.size(); i++) {
			mask1[i].resize(Gp.G[i].size());
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

/*
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
*/
		for (int i = 1; i < size; i++) MPI_Send(&i, 0, MPI_INT, i, STOP_TAG, MPI_COMM_WORLD);

		MPI_Status status;
		for (int i = 1; i < size; i++) {
			char* buf = new char[buf_size];
			MPI_Recv(buf, buf_size, MPI_PACKED, MPI_ANY_SOURCE, SUM_TAG, MPI_COMM_WORLD, &status);
			edge s = Unpack(buf, buf_size, vector_size, 0);
			delete[] buf;
			sum = sum + s;
			int buff;
			MPI_Recv(&buff, 1, MPI_INT, MPI_ANY_SOURCE, NUM0_TAG, MPI_COMM_WORLD, &status);
			num0 += buff;
			MPI_Recv(&buff, 1, MPI_INT, MPI_ANY_SOURCE, NUM2_TAG, MPI_COMM_WORLD, &status);
			num2 += buff;
			MPI_Recv(&buff, 1, MPI_INT, MPI_ANY_SOURCE, NUM3_TAG, MPI_COMM_WORLD, &status);
			num3 += buff;
			MPI_Recv(&buff, 1, MPI_INT, MPI_ANY_SOURCE, NUM4_TAG, MPI_COMM_WORLD, &status);
			num4 += buff;
			MPI_Recv(&buff, 1, MPI_INT, MPI_ANY_SOURCE, NUM5_TAG, MPI_COMM_WORLD, &status);
			num5 += buff;
			MPI_Recv(&buff, 1, MPI_INT, MPI_ANY_SOURCE, CHR_TAG, MPI_COMM_WORLD, &status);
			chr += buff;
			MPI_Recv(&buff, 1, MPI_INT, MPI_ANY_SOURCE, CH1_TAG, MPI_COMM_WORLD, &status);
			ch1 += buff;
			MPI_Recv(&buff, 1, MPI_INT, MPI_ANY_SOURCE, CH2_TAG, MPI_COMM_WORLD, &status);
			ch2 += buff;
			MPI_Recv(&buff, 1, MPI_INT, MPI_ANY_SOURCE, CHRS_TAG, MPI_COMM_WORLD, &status);
			chrs += buff;
			MPI_Recv(&buff, 1, MPI_INT, MPI_ANY_SOURCE, DECOMP1_TAG, MPI_COMM_WORLD, &status);
			decomp1 += buff;
			MPI_Recv(&buff, 1, MPI_INT, MPI_ANY_SOURCE, DECOMP2_TAG, MPI_COMM_WORLD, &status);
			decomp2 += buff;
			MPI_Recv(&buff, 1, MPI_INT, MPI_ANY_SOURCE, DECOMP3_TAG, MPI_COMM_WORLD, &status);
			decomp3 += buff;
			MPI_Recv(&buff, 1, MPI_INT, MPI_ANY_SOURCE, BRIDGES_TAG, MPI_COMM_WORLD, &status);
			bridges += buff;
			MPI_Recv(&buff, 1, MPI_INT, MPI_ANY_SOURCE, PENDUNT_TAG, MPI_COMM_WORLD, &status);
			pendunt += buff;
			MPI_Recv(&buff, 1, MPI_INT, MPI_ANY_SOURCE, FACTORS_TAG, MPI_COMM_WORLD, &status);
			factors += buff;
		}

		double endTime = MPI_Wtime();
		cout << "Time of programm : " << endTime - startTime << endl;
		cout << "Call prosedure " << factors << endl;
		cout << "Reductions : " << endl;
		cout << " reduced chains initially " << chrs << endl;
		cout << " bridge reductions " << bridges << endl;
		cout << " pendunt reducutions " << pendunt << endl;
		cout << " chains reduced " << chr << endl;
		cout << " 1-st type " << ch1 << endl;
		cout << " 2-nd type " << ch2 << endl;
		cout << " decompositions in chainreduction " << decomp1 << endl;
		cout << " decompositions in chr " << decomp2 << endl;
		cout << " decompositions in factoring " << decomp3 << endl;
		cout << "Were ends of recursion : " << num0 + num2 + num3 + num4 + num5 << endl;
		cout << " unconnected graphs " << num0 << endl;
		cout << " 2-dimension graphs " << num2 << endl;
		cout << " 3-dimension graphs " << num3 << endl;
		cout << " 4-dimension graphs " << num4 << endl;
		cout << " 5-dimension graphs " << num5 << endl;

		for (int i = 0; i < sum.C.size(); i++) sum.C[i] = sum.C[i] / Bin[Gp.G.size()].C[2];
		//sum = sum + Bin.front();
		cout << "Solution:" << endl;
		sum.printedge();

		double value = 0, p = 0.9, z = 0.1;
		int q = sum.power;
		for (int i = 0; i < q; i++) value += sum.C[i] * pow(p, q - i)*pow(z, i);
		cout << "Value at point " << p << ": " << fixed << setprecision(15) << value << endl;
	}

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

	char *buf_edge = Pack(sum, buf_size, vector_size);
	MPI_Send(buf_edge, buf_size, MPI_PACKED, HOST_PROCESSOR, SUM_TAG, MPI_COMM_WORLD);
	delete[] buf_edge;
	MPI_Send(&num0, 1, MPI_INT, HOST_PROCESSOR, NUM0_TAG, MPI_COMM_WORLD);
	MPI_Send(&num2, 1, MPI_INT, HOST_PROCESSOR, NUM2_TAG, MPI_COMM_WORLD);
	MPI_Send(&num3, 1, MPI_INT, HOST_PROCESSOR, NUM3_TAG, MPI_COMM_WORLD);
	MPI_Send(&num4, 1, MPI_INT, HOST_PROCESSOR, NUM4_TAG, MPI_COMM_WORLD);
	MPI_Send(&num5, 1, MPI_INT, HOST_PROCESSOR, NUM5_TAG, MPI_COMM_WORLD);
	MPI_Send(&chr, 1, MPI_INT, HOST_PROCESSOR, CHR_TAG, MPI_COMM_WORLD);
	MPI_Send(&ch1, 1, MPI_INT, HOST_PROCESSOR, CH1_TAG, MPI_COMM_WORLD);
	MPI_Send(&ch2, 1, MPI_INT, HOST_PROCESSOR, CH2_TAG, MPI_COMM_WORLD);
	MPI_Send(&chrs, 1, MPI_INT, HOST_PROCESSOR, CHRS_TAG, MPI_COMM_WORLD);
	MPI_Send(&decomp1, 1, MPI_INT, HOST_PROCESSOR, DECOMP1_TAG, MPI_COMM_WORLD);
	MPI_Send(&decomp2, 1, MPI_INT, HOST_PROCESSOR, DECOMP2_TAG, MPI_COMM_WORLD);
	MPI_Send(&decomp3, 1, MPI_INT, HOST_PROCESSOR, DECOMP3_TAG, MPI_COMM_WORLD);
	MPI_Send(&bridges, 1, MPI_INT, HOST_PROCESSOR, BRIDGES_TAG, MPI_COMM_WORLD);
	MPI_Send(&pendunt, 1, MPI_INT, HOST_PROCESSOR, PENDUNT_TAG, MPI_COMM_WORLD);
	MPI_Send(&factors, 1, MPI_INT, HOST_PROCESSOR, FACTORS_TAG, MPI_COMM_WORLD);
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