#include "stdafx.h"

ofstream output("C://output.txt");
ifstream input("C://input.txt");

int num = 0, num2 = 0, num3 = 0, num4 = 0, num5 = 0, chr = 0, ch1 = 0, ch2 = 0;
vector<bool> visited;
vector<edge> Bin;
bool busy;
edge sum;

void edge::printedge() {
	if (ex == 1) { // can be 204
		for (int i = 0; i < C.size(); i++) cout << C[i] << " ";
		cout << endl << "power = " << power << ", simple = " << simple << ", ex = " << ex << endl;
	}
	else cout << "empty edge" << endl;
}

void str::printstr() {
	for (int i = 0; i < G.size(); i++) {
		cout << "H.G[" << i << "] : " << endl;
		for (int j = 0; j < G[i].size(); j++) G[i][j].printedge();
	}
	cout << "H.F : " << endl;
	for (int i = 0; i < F.size(); i++) F[i].printedge();
	cout << "H.ex = " << ex << endl;
}

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
}

char *Pack(edge e, int buf_size, int vector_size) {
	char *buf = new char[buf_size];
	if (e.C.empty()) e.C.resize(vector_size);
	double *V = &e.C.front();; // vector <-> massive
	int *a = &e.power, *b = &e.simple;
	bool *c = &e.ex;
	int position = 0;

	MPI_Pack(V, vector_size, MPI_DOUBLE, buf, buf_size, &position, MPI_COMM_WORLD);
	MPI_Pack(a, 1, MPI_INT, buf, buf_size, &position, MPI_COMM_WORLD);
	MPI_Pack(b, 1, MPI_INT, buf, buf_size, &position, MPI_COMM_WORLD);
	MPI_Pack(c, 1, MPI_BYTE, buf, buf_size, &position, MPI_COMM_WORLD);

	return buf;
}

edge Unpack(char buf[], int buf_size, int vector_size, int position) {
	edge e;
	e.C.resize(vector_size); // need size
	double *V = &e.C.front();
	int *a = &e.power, *b = &e.simple;
	bool *c = &e.ex;

	MPI_Unpack(buf, buf_size, &position, V, vector_size, MPI_DOUBLE, MPI_COMM_WORLD);
	MPI_Unpack(buf, buf_size, &position, a, 1, MPI_INT, MPI_COMM_WORLD);
	MPI_Unpack(buf, buf_size, &position, b, 1, MPI_INT, MPI_COMM_WORLD);
	MPI_Unpack(buf, buf_size, &position, c, 1, MPI_BYTE, MPI_COMM_WORLD);

	if (!e.ex) {
		edge n; // not fill anything
		n.ex = false;
		return n;
	}
	return e;
}

char *Pack_vector(vector<edge> F, int buf_size, int vector_size) {
	int vector_buf_size = F.size()*buf_size + sizeof(int), a = F.size();
	char *buf = new char[vector_buf_size], *buf_edge;
	int position = 0;
	MPI_Pack(&a, 1, MPI_INT, buf, vector_buf_size, &position, MPI_COMM_WORLD);
	for (int i = 0; i < F.size(); i++) {
		buf_edge = Pack(F[i], buf_size, vector_size);
		MPI_Pack(buf_edge, buf_size, MPI_PACKED, buf, vector_buf_size, &position, MPI_COMM_WORLD); //packed?
	}

	return buf;
}

vector<edge> Unpack_vector(char buf[], int buf_size, int vector_size, int position) {
	edge e;
	vector<edge> F;
	int count, shift;
	MPI_Unpack(buf, sizeof(int), &position, &count, 1, MPI_INT, MPI_COMM_WORLD); // size IN buf is not write
	for (int i = 0; i < count; i++) {
		shift = position + i*buf_size;
		e = Unpack(buf, buf_size, vector_size, shift); // buf don't reduce, return another buf_size?
		F.push_back(e);
	}

	return F;
}

char* Pack_data(str H, int buf_size, int vector_size)
{
	int position = 0, vector_buf_size;
	int str_buf_size = H.F.size()*buf_size + sizeof(int) + sizeof(bool);
	for (int i = 1; i < H.G.size(); i++) str_buf_size += i*buf_size + sizeof(int);
	char *buf = new char[str_buf_size], *buf_vector;
	for (int i = 0; i < H.G.size(); i++) for (int j = 0; j <= i; j++) H.G[i].erase(H.G[i].begin());

	for (int i = 0; i < H.G.size() - 1; i++) {
		vector_buf_size = H.G[i].size()*buf_size + sizeof(int);
		buf_vector = Pack_vector(H.G[i], buf_size, vector_size);
		MPI_Pack(buf_vector, vector_buf_size, MPI_PACKED, buf, str_buf_size, &position, MPI_COMM_WORLD); //packed?
	}
	vector_buf_size = H.F.size()*buf_size + sizeof(int);
	buf_vector = Pack_vector(H.F, buf_size, vector_size);
	MPI_Pack(buf_vector, vector_buf_size, MPI_PACKED, buf, str_buf_size, &position, MPI_COMM_WORLD); //packed?
	MPI_Pack(&H.ex, 1, MPI_BYTE, buf, str_buf_size, &position, MPI_COMM_WORLD); //packed?

	return buf;
}


str Unpack_data(char* buf, int buf_size, int vector_size)
{
	str H;
	edge N;
	N.ex = false;
	H.G.push_back(Unpack_vector(buf, buf_size, vector_size, 0)); // vector_buf_size is enough for OUT buf, buf don't reduce
	int position = H.G[0].size()*buf_size + sizeof(int), count;
	H.G[0].insert(H.G[0].begin(), N); // get matrix size
	H.G.resize(H.G[0].size()); // assume is squre

	for (int i = 1; i < H.G.size() - 1; i++) {
		H.G[i] = Unpack_vector(buf, buf_size, vector_size, position);
		position += H.G[i].size()*buf_size + sizeof(int);
	}
	H.F = Unpack_vector(buf, buf_size, vector_size, position);
	position += H.F.size()*buf_size + sizeof(int);
	MPI_Unpack(buf, 1, &position, &H.ex, 1, MPI_BYTE, MPI_COMM_WORLD);

	for (int i = 1; i < H.G.size(); i++) for (int j = 0; j <= i; j++) H.G[i].insert(H.G[i].begin(), N);
	for (int i = 1; i < H.G.size(); i++) for (int j = 0; j < i; j++) H.G[i][j] = H.G[j][i];

	return H;
}

edge operator *(edge x, edge y)
{
	edge F;
	if (!x.C.empty() && !y.C.empty()) { // for edges not zero attendance
		if (x.C.size() == 1) return y; // * on 1; can't +, -, ~ for 1
		if (y.C.size() == 1) return x;
		F.C.resize(x.C.size());
		F.power = 0;
		F.simple = 0;
		F.ex = true; // need for chainreduction

		if (x.simple > 0 && x.power != 1 && x.C.front() != 1) cout << "Eror in Operator +" << endl;
		if (y.simple > 0 && y.power != 1 && y.C.front() != 1) cout << "Eror in Operator +" << endl;

		if (x.simple > 0) { // paralel reduction
			x.power += x.simple;
			x = Bin[x.power];
			x.C[x.power] = 0;
		}
		if (y.simple > 0) {
			y.power += y.simple;
			y = Bin[y.power];
			y.C[y.power] = 0;
		}

		try {
			F.power = x.power + y.power;
			for (int i = 0; i<x.C.size(); i++) for (int j = 0; j<y.C.size(); j++) {
				if (x.C[i] != 0 && y.C[j] != 0) {
					if (i + j >= F.C.size())  throw "Eror - vector is out of size";
					F.C[i + j] += x.C[i] * y.C[j];
				}
			}
		}
		catch (char *str) {
			output << str << endl;
			for (int i = 0; i<x.C.size(); i++) output << x.C[i] << " ";
			output << endl << "*" << endl;;
			for (int i = 0; i<y.C.size(); i++) output << y.C[i] << " ";
			output << endl;
		}
	}
	return F;
}

edge operator +(edge x, edge y)
{
	edge F;
	if (!x.C.empty() && !y.C.empty()) { // for edges not zero attendance

		if (x.C.size() == 1 && y.C.size() != 1) {
			x.C.resize(y.C.size());
			x.power = 0;
			x.simple = 0;
		}
		if (x.C.size() != 1 && y.C.size() == 1) {
			y.C.resize(x.C.size());
			y.power = 0;
			y.simple = 0;
		}

		F.C.resize(x.C.size());
		F.simple = 0;
		F.ex = true; // need for chainreduction

		if (x.simple > 0 && x.power != 1 && x.C.front() != 1) cout << "Eror in Operator +" << endl;
		if (y.simple > 0 && y.power != 1 && y.C.front() != 1) cout << "Eror in Operator +" << endl;

		if (x.simple > 0) { // paralel reduction
			x.power += x.simple;
			x = Bin[x.power];
			x.C[x.power] = 0;
		}
		if (y.simple > 0) {
			y.power += y.simple;
			y = Bin[y.power];
			y.C[y.power] = 0;
		}

		if (x.power != y.power) {
			edge I = Bin[abs(x.power - y.power)];
			if (x.power < y.power) x = x*I;
			if (y.power < x.power) y = y*I;
		}

		F.power = x.power;
		for (int i = 0; i<F.C.size(); i++) F.C[i] = x.C[i] + y.C[i];
	}
	if (!x.C.empty() && y.C.empty()) F = x;
	if (x.C.empty() && !y.C.empty()) F = y;
	return F;
}

edge operator -(edge x, edge y)
{
	edge F;
	if (!x.C.empty() && !y.C.empty()) { // for edges not zero attendance

		if (x.C.size() == 1 && y.C.size() != 1) {
			x.C.resize(y.C.size());
			x.power = 0;
			x.simple = 0;
		}
		if (x.C.size() != 1 && y.C.size() == 1) {
			y.C.resize(x.C.size());
			y.power = 0;
			y.simple = 0;
		}

		F.C.resize(x.C.size());
		F.simple = 0;
		F.ex = true;


		if (x.simple > 0 && x.power != 1 && x.C.front() != 1) cout << "Eror in Operator +" << endl;
		if (y.simple > 0 && y.power != 1 && y.C.front() != 1) cout << "Eror in Operator +" << endl;

		if (x.simple > 0) { // paralel reduction
			x.power += x.simple;
			x = Bin[x.power];
			x.C[x.power] = 0;
		}
		if (y.simple > 0) {
			y.power += y.simple;
			y = Bin[y.power];
			y.C[y.power] = 0;
		}

		if (x.power != y.power) {
			edge I = Bin[abs(x.power - y.power)];
			if (x.power < y.power) x = x*I;
			if (y.power < x.power) y = y*I;
		}

		F.power = x.power;
		for (int i = 0; i<F.C.size(); i++) F.C[i] = x.C[i] - y.C[i];
	}
	if (!x.C.empty() && y.C.empty()) F = x;
	if (x.C.empty() && !y.C.empty()) {
		//for (int i = 0; i<y.C.size(); i++) y.C[i] = -y.C[i]; // ?
		F = y;
	}
	return F;
}

edge operator ~(edge x)
{
	edge F;
	if (!x.C.empty()) { // for edges not zero attendance
		if (x.C.size() == 1) return F;
		F.C.resize(x.C.size());
		F.simple = 0;
		F.ex = true;

		if (x.simple > 0 && x.power != 1 && x.C.front() != 1) cout << "Eror in Operator +" << endl;

		if (x.simple > 0) { // paralel reduction
			x.power += x.simple;
			x = Bin[x.power];
			x.C[x.power] = 0;
		}

		F.power = x.power;
		edge I = Bin[F.power];
		for (int i = 0; i<F.C.size(); i++) F.C[i] = I.C[i] - x.C[i];
	}
	else F.C.push_back(1);
	return F;
}

void table(vector<vector<edge>> &H, vector<edge> &E)
{
	for (int i = 0; i<H.size(); i++) H[i].resize(H.size());
	for (int i = 0; i<H.size(); i++) for (int j = 0; j<H[i].size(); j++) for (int w = 0; w<E.size(); w++)
		if ((i == E[w].node1 && j == E[w].node2) || (i == E[w].node2 && j == E[w].node1)) H[i][j] = E[w];
}

vector<int> gnodepower(vector<vector<edge>> &H)
{
	vector <int> nodepower(H.size());
	int i = 0;
	while (i < H.size()) {
		for (int j = 0; j<H[i].size(); j++) if (H[i][j].ex) nodepower[i]++;
		i++;
	}
	return nodepower;
}

void DFS(vector<vector<edge>> &H, int q)
{
	visited[q] = true;
	for (int i = 0; i<H[q].size(); i++) if (H[q][i].ex && !visited[i]) DFS(H, i);
}

bool gconnected(vector<vector<edge>> &H)
{
	int count = 0;
	visited.resize(H.size());
	for (int i = 0; i<visited.size(); i++) visited[i] = false;

	DFS(H, 0);
	for (int i = 0; i<visited.size(); i++) if (visited[i]) count++;

	if (count != H.size()) return false;
	else return true;
}

void delnode(vector<vector<edge>> &H, int q)
{
	H.erase(H.begin() + q);
	for (int i = 0; i<H.size(); i++) H[i].erase(H[i].begin() + q);
}

void deledge(vector<vector<edge>> &H, int q, int w)
{
	edge F;
	F.ex = false;
	H[q][w] = F;
	H[w][q] = F;
}

void contractedge(vector<vector<edge>> &H, int q, int w)
{
	for (int i = 0; i<H[q].size(); i++) if (H[q][i].ex && i != w) { // do not consider H[q][w]
		if (H[w][i].ex && H[q][i].power == 1 && H[w][i].power == 1) { //  power==1 is falg of simple edge
			H[w][i].simple += H[q][i].simple + 1;
			H[i][w].simple += H[q][i].simple + 1;
		}
		if (H[w][i].ex && (H[q][i].power != 1 || H[w][i].power != 1)) { // arise in chainreduction
			H[w][i] = H[q][i] + H[w][i] - H[q][i] * H[w][i];
			H[i][w] = H[w][i]; // for symmetry and less computing
		}
		if (!H[w][i].ex) {
			H[w][i] = H[q][i]; // so easy, no need to compute
			H[i][w] = H[q][i];
		}
	}
	delnode(H, q);
}

void renumerate(vector<vector<edge>> &H, int s, int t)
{
	if (s != t) {
		vector<edge> F, G;
		F = H[s];
		G = H[t];
		for (int i = 0; i<H.size(); i++) if (i != s && i != t) swap(F[i], G[i]);

		if (!visited.empty() && max(s, t) < visited.size()) {
			bool r = visited[s];
			visited[s] = visited[t];
			visited[t] = r;
		}

		for (int i = 0; i<H.size(); i++) { //clear strings and columns for nodes s,t
			if (H[t][i].ex) deledge(H, t, i); //take into account the symmetry
			if (H[s][i].ex) deledge(H, s, i);
		}

		for (int i = 0; i<H.size(); i++) { // filling
			if (F[i].ex && i != s) {
				H[s][i] = F[i];
				if (i != t)  H[i][s] = F[i]; // if do not fill another string 
			}
			if (G[i].ex && i != t) {
				H[t][i] = G[i];
				if (i != s) H[i][t] = G[i];
			}
		}
	}
}

vector<vector<edge>> get_info()
{
	vector<vector<edge>> S;

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

			S.resize(n);
			table(S, E);

			output << "Adjacency matrix:" << endl;
			for (int i = 0; i<S.size(); i++) { for (int j = 0; j<S[i].size(); j++) output << S[i][j].ex << " "; output << endl; }

			bool r = gconnected(S);
			if (!r) cout << "Unconnected graph on input!" << endl;

			int s = 0, t = 2;

			if (s < t) {
				if (s != 0 || t != 1) { // after this 0,1
					if (s != 0 && t != 1) {
						renumerate(S, s, 0);
						renumerate(S, t, 1);
					}
					if (s == 0 && t != 1) renumerate(S, t, 1);
					if (s != 0 && t == 1) renumerate(S, s, 0);
				}
			}
			output << "Adjacency matrix after renum:" << endl;
			for (int i = 0; i<S.size(); i++) { for (int j = 0; j<S[i].size(); j++) output << S[i][j].ex << " "; output << endl; }

			str H;
			H.G = S;
			H.F.push_back(Bin.front());
			H.outprintstr();

			return S;
		}
		return S;
	}
	return S;
}

edge penduntreduction(vector<vector<edge>> &H, edge F, int q, bool find)
{
	vector<int> nodepower = gnodepower(H);
	if (find) nodepower.erase(nodepower.begin() + q); // in penduntreduction we not consider pendunt node 0 || 1

	vector <int>::iterator it, smallest = nodepower.end() - 1;
	for (it = nodepower.begin(); it<nodepower.end(); ++it) if (*it == 1) smallest = it;
	int r = smallest - nodepower.begin(); // find pendunt node, can be 0,1

	if (*smallest == 1 && H.size() > 2) { //otherwise all will be deleted
		for (int i = H[r].size() - 1; i >= 0; i--)
			if (H[r][i].ex) { //find incident edge to pendunt node, can't be 0,1 || 1,0

				if ((r == 1 && i == 0) || (r == 0 && i == 1)) { // that do not have loop
					q = r;
					find = true;
				}
				if (r != 0 && r != 1) delnode(H, r);
				if ((r == 1 && i != 0) || (r == 0 && i != 1)) {
					F = F*H[r][i];
					delnode(H, r);
					i--; // after delenode not forget about i
					renumerate(H, i, 1); // always must be 1
				}
				break;// arise the eror if abort this
			}
		return penduntreduction(H, F, q, find);
	}
	return F;

}

bool bridgereduction(vector<vector<edge>> &H)
{
	for (int i = 0; i<H.size(); i++) if (!visited[i]) cout << i + 1 << " ";
	cout << endl;

	if (visited[0] && visited[1]) // only one of this cases exist
		for (int i = H.size() - 1; i>0; i--) if (!visited[i]) { // if end is 0,0 make mistake
																//cout << i + 1 << " ";
			delnode(H, i);
			visited.erase(visited.begin() + i); // not forget about visited
		}

	if (!visited[0] && !visited[1])
		for (int i = H.size() - 1; i>0; i--) if (visited[i]) {
			delnode(H, i);
			visited.erase(visited.begin() + i); // not forget about visited
		}

	if ((visited[0] && !visited[1]) || (!visited[0] && visited[1])) return false;

	return true;
}

edge fedge(vector<vector<edge>> &H)
{
	edge F;
	vector<int> nodepower = gnodepower(H);
	vector<int>::iterator it, smallest = nodepower.end() - 1;

	for (it = nodepower.begin(); it<nodepower.end(); ++it)
		if (*it <= *smallest && (it - nodepower.begin() != 0) && (it - nodepower.begin() != 1)) smallest = it;
	int r = smallest - nodepower.begin(); //can't be 0,1

	for (int i = H[r].size() - 1; i >= 0; i--) if (H[r][i].ex) {  //find incident edge selected node, can be 0,1
		F = H[r][i];
		F.node1 = r; // Becouse we use only matrix coefficients
		F.node2 = i;
		break;
	}
	return F;
}

bool exis(vector<int> P, int x)
{ // existence of element in the vector
	if (P.empty()) return false;
	else for (int i = 0; i<P.size(); i++) if (x == P[i]) return true;
	return false;
}

vector<int> fchain(vector<vector<edge>> &H, vector<int> &V)
{
	vector<int> nodepower = gnodepower(H);
	vector<int> P; // chain nodes vector

	bool find = false;
	int i = 0;
	while (!find && i<H.size()) {
		for (int j = 0; j<H[i].size(); j++) {
			if (H[i][j].ex && !(exis(V, i) && exis(V, j)) && (nodepower[j] == 2 || nodepower[i] == 2)) {
				P.push_back(i);
				P.push_back(j);
				find = true;
				break;
			}
		}
		i++;
	}

	if (P.size() > 0) { // expand vector in both sides if it is not empty
		i = P.front();
		while (nodepower[i] == 2 && i != P.back())
			for (int j = 0; j<H[i].size(); j++) if (H[i][j].ex && j != P[1]) {
				if (j != P.back()) P.insert(P.begin(), j);
				i = j;
				break;
			}
		i = P.back();
		while (nodepower[i] == 2 && i != P.front())
			for (int j = 0; j<H[i].size(); j++) if (H[i][j].ex && j != P[P.size() - 2]) {
				if (j != P.front()) P.push_back(j);
				i = j;
				break;
			}

		return P;
	}
	return P;
}

vector<int> chainreduction(vector<vector<edge>> &H, vector<int> Ch, vector<int> V, bool found)
{
	vector<int> P = fchain(H, V);
	int count = 0;
	if (!P.empty()) {
		for (int j = 0; j<P.size(); j++) if (P[j] == 0 || P[j] == 1) count++; // count selected nodes in chain

		vector<int>::iterator it, r = P.begin();
		if (count == 1) {
			for (it = P.begin(); it<P.end(); ++it) if (*it == 0 || *it == 1) r = it;
			if ((r == P.begin() || r == P.end() - 1) && !Ch.empty()) count = 0;
		}

		if ((count == 1 || count == 2) && !found) {
			if (count == 2) { // have only chain with count=2
				found = true;
				V = P;
			}
			if (count == 1) {
				if (!Ch.empty()) {
					for (it = Ch.begin(); it<Ch.end() - 1; ++it) if (*it == 0 || *it == 1) r = it; // count at the end
					if (r == Ch.begin() || r == Ch.end() - 2) V.clear(); // next recursive call reduct Ch
				}
				for (int j = 0; j<P.size(); j++) if (!exis(V, P[j])) V.push_back(P[j]);
			}

			P.push_back(count);
			Ch = P;

			return chainreduction(H, Ch, V, found);
		}
		else count = 0; // to reduct chain(s) when we find count=2

		if (count == 0) { // Replace chain by edge
			chr++;
			edge newedge;
			newedge.C.push_back(1);
			for (int j = 0; j<P.size() - 1; j++) newedge = newedge*H[P[j]][P[j + 1]];
			for (int j = 1; j<P.size() - 1; j++) {
				delnode(H, P[j]); // delete only inner nodes of chain
				for (int k = 0; k<P.size(); k++) if (P[j] < P[k]) P[k]--; // not forget about P
				for (int k = 0; k<Ch.size(); k++) if (P[j] < Ch[k]) Ch[k]--; // not forget about Ch
				for (int k = 0; k<V.size(); k++) if (P[j] < V[k]) V[k]--; // not forget about V
			}
			int x = P.front(), y = P.back();

			if (H[x][y].ex) { // Becouse newedge.power can't be 1
				H[x][y] = H[x][y] + newedge - H[x][y] * newedge;
				H[y][x] = H[x][y];
			}
			else {
				H[x][y] = newedge;
				H[y][x] = newedge;
			}

			if (!Ch.empty()) { // expand vector in both sides if it is not empty
				Ch.pop_back();
				vector<int> nodepower = gnodepower(H);

				int i = Ch.front();
				while (nodepower[i] == 2 && i != Ch.back())
					for (int j = 0; j<H[i].size(); j++) if (H[i][j].ex && j != Ch[1]) {
						if (j != Ch.back()) {
							Ch.insert(Ch.begin(), j);
							if (!exis(V, j)) V.push_back(j);
						}
						i = j;
						break;
					}
				i = Ch.back();
				while (nodepower[i] == 2 && i != Ch.front())
					for (int j = 0; j<H[i].size(); j++) if (H[i][j].ex && j != Ch[Ch.size() - 2]) {
						if (j != Ch.front()) {
							Ch.push_back(j);
							if (!exis(V, j)) V.push_back(j);
						}
						i = j;
						break;
					}

				count = 0;
				for (int i = 0; i<Ch.size(); i++) if (Ch[i] == 0 || Ch[i] == 1) count++; // count selected nodes in chain
				Ch.push_back(count);
			}
			return chainreduction(H, Ch, V, found);
		}
	}
	return Ch;
}

void procedure(vector<vector<edge>> &H, edge F, bool connected, int buf_size, int vector_size, int rank)
{
	//cout << "slave " << rank << " start factoring" << endl;
	if (!connected) {
		if (!bridgereduction(H)) return;
		else connected = true;
	}

	if (H.size() < 6) {
		//cout << "slave " << rank << " end factoring" << endl;
		num++;
		if (H.size() == 2) {
			num2++;
			edge e = F*H[0][1];
			sum = sum + e;
			//e.printedge();
		}
		if (H.size() == 3) {
			num3++;
			edge e = F*(H[0][1] + H[1][2] * H[0][2] - H[0][1] * H[1][2] * H[0][2]);
			sum = sum + e;
			//e.printedge();
		}
		if (H.size() == 4) {
			num4++;
			edge e = F*(H[0][1] + ~H[0][1] * (H[1][2] * H[0][2] + H[0][3] * H[1][3] - H[1][2] * H[0][2] * H[0][3] * H[1][3] +
				H[1][2] * H[2][3] * H[0][3] * ~H[1][3] * ~H[0][2] + ~H[1][2] * H[2][3] * ~H[0][3] * H[1][3] * H[0][2]));
			sum = sum + e;
			//e.printedge();
		}

		if (H.size() == 5) {
			num5++;
			edge e = F*(~(~H[0][1] * ~H[0][2] * (H[1][2] * ~H[1][4] * (H[0][4] * ~H[0][3] * ~H[2][4] * (~H[3][4] + ~H[1][3] * ~H[2][3] * H[3][4]) +
				H[0][3] * ~H[1][3] * ~H[2][3] * (~H[2][4] + ~H[0][4] * ~H[3][4] * H[2][4])) + ~H[0][3] * ~H[0][4] * ~(~H[1][2] * ~H[1][3] * ~H[1][4])) +
				~H[0][1] * ~H[1][3] * (H[1][4] * ~H[0][4] * (H[0][3] * ~H[0][2] * ~H[3][4] * (~H[2][3] + ~H[1][2] * ~H[2][4] * H[2][3]) +
					H[0][2] * ~H[1][2] * ~H[2][4] * (~H[3][4] + ~H[0][3] * ~H[2][3] * H[3][4])) + ~H[1][2] * ~H[1][4]) +
				H[1][3] * ~H[0][1] * ~H[0][3] * ~H[1][2] * (H[0][2] * ~H[0][4] * ~H[2][3] * (~H[2][4] + ~H[1][4] * ~H[3][4] * H[2][4]) +
					H[0][4] * ~H[1][4] * ~H[3][4] * (~H[2][3] + ~H[0][2] * ~H[2][4] * H[2][3]))));
			sum = sum + e;
			//e.printedge();
		}
		return;
	}

	F = penduntreduction(H, F, 0, false);

	vector<int> Chain, V;
	Chain = chainreduction(H, Chain, V, false);
	int count = 0;
	if (!Chain.empty()) {
		count = Chain.back();
		Chain.pop_back();
	}
	//count = 0;

	if (H.size() < 6) {
		//cout << "slave " << rank << " end factoring" << endl;
		num++;
		if (H.size() == 2) {
			num2++;
			edge e = F*H[0][1];
			sum = sum + e;
			//e.printedge();
		}
		if (H.size() == 3) {
			num3++;
			edge e = F*(H[0][1] + H[1][2] * H[0][2] - H[0][1] * H[1][2] * H[0][2]);
			sum = sum + e;
			//e.printedge();
		}
		if (H.size() == 4) {
			num4++;
			edge e = F*(H[0][1] + ~H[0][1] * (H[1][2] * H[0][2] + H[0][3] * H[1][3] - H[1][2] * H[0][2] * H[0][3] * H[1][3] +
				H[1][2] * H[2][3] * H[0][3] * ~H[1][3] * ~H[0][2] + ~H[1][2] * H[2][3] * ~H[0][3] * H[1][3] * H[0][2]));
			sum = sum + e;
			//e.printedge();
		}

		if (H.size() == 5) {
			num5++;
			edge e = F*(~(~H[0][1] * ~H[0][2] * (H[1][2] * ~H[1][4] * (H[0][4] * ~H[0][3] * ~H[2][4] * (~H[3][4] + ~H[1][3] * ~H[2][3] * H[3][4]) +
				H[0][3] * ~H[1][3] * ~H[2][3] * (~H[2][4] + ~H[0][4] * ~H[3][4] * H[2][4])) + ~H[0][3] * ~H[0][4] * ~(~H[1][2] * ~H[1][3] * ~H[1][4])) +
				~H[0][1] * ~H[1][3] * (H[1][4] * ~H[0][4] * (H[0][3] * ~H[0][2] * ~H[3][4] * (~H[2][3] + ~H[1][2] * ~H[2][4] * H[2][3]) +
					H[0][2] * ~H[1][2] * ~H[2][4] * (~H[3][4] + ~H[0][3] * ~H[2][3] * H[3][4])) + ~H[1][2] * ~H[1][4]) +
				H[1][3] * ~H[0][1] * ~H[0][3] * ~H[1][2] * (H[0][2] * ~H[0][4] * ~H[2][3] * (~H[2][4] + ~H[1][4] * ~H[3][4] * H[2][4]) +
					H[0][4] * ~H[1][4] * ~H[3][4] * (~H[2][3] + ~H[0][2] * ~H[2][4] * H[2][3]))));
			sum = sum + e;
			//e.printedge();
		}
		return;
	}

	if (count == 1) {
		//cout << "slave " << rank << " get hain with 1 pivote" << endl;
		ch1++;
		int s, t;
		bool terminal = false;
		vector<int> Ch(Chain);
		vector<int>::iterator it, r;
		int coun = 0;
		for (it = Ch.begin(); it < Ch.end(); ++it) if (*it == 0 || *it == 1) r = it;
		int q = r - Ch.begin(); // place of pivote node t=(0 or 1) into vector Ch
		if (r == Ch.begin() || r == Ch.end() - 1) terminal = true;
		s = *r;
		s == 0 ? t = 1 : t = 0; // need for case terminal

		edge P, L;
		P.C.push_back(1);
		L.C.push_back(1);
		vector<vector<edge>> G(H);
		for (int i = 0; i<q; i++) L = L*G[Ch[i]][Ch[i + 1]];
		for (int i = q; i<Ch.size() - 1; i++) P = P*G[Ch[i]][Ch[i + 1]];

		for (int i = 1; i<Ch.size() - 1; i++) { // after this we get 2 nodes from chain, one of them can be pivote
			delnode(G, Ch[i]);
			for (int j = 0; j<Ch.size(); j++) if (Ch[i] < Ch[j]) Ch[j]--; // not forget about Ch
		}
		int x = Ch.front(), y = Ch.back(); // matter x<=>y

		vector<vector<edge>> G1(G);
		bool connec = gconnected(G);
		vector<bool> visited1(visited);
		if (!connec) cout << "Get unconnected in chr1" << endl;

		(terminal) ? renumerate(G, y, t) : renumerate(G, y, 1);  // s=0||1, becouse we don't delete pivote node, after delete: s - node out of chain always 0, so t=1

		//cout << "slave " << rank << " ask for help" << endl;
		int just_value;
		MPI_Send(&just_value, 0, MPI_INT, HOST_PROCESSOR, I_NEED_HELP_TAG, MPI_COMM_WORLD); // ask for help
		int help_processor = 0;
		MPI_Status status;
		MPI_Recv(&help_processor, 1, MPI_INT, HOST_PROCESSOR, I_NEED_HELP_TAG, MPI_COMM_WORLD, &status); // get answer

		if (help_processor != 0) {
			//cout << "slave " << rank << " have help from " << help_processor << endl;
			str Gp;
			Gp.G = G;
			Gp.F.push_back(F*(P - P*L));
			Gp.ex = connec;
			int str_buf_size = Gp.F.size()*buf_size + sizeof(int) + sizeof(bool);
			for (int i = 1; i < Gp.G.size(); i++) str_buf_size += i*buf_size + sizeof(int);
			char *buf_str = Pack_data(Gp, buf_size, vector_size);
			MPI_Send(buf_str, str_buf_size, MPI_PACKED, help_processor, GPINFO_TAG, MPI_COMM_WORLD);
			delete[] buf_str;
		}
		else procedure(G, F*(P - P*L), connec, buf_size, vector_size, rank); // Rsy = k

		visited = visited1; // same size so all right
		G = G1;
		(terminal) ? renumerate(G, x, t) : renumerate(G, x, 1);

		//cout << "slave " << rank << " ask for help" << endl;
		just_value;
		MPI_Send(&just_value, 0, MPI_INT, HOST_PROCESSOR, I_NEED_HELP_TAG, MPI_COMM_WORLD); // ask for help
		help_processor = 0;
		MPI_Recv(&help_processor, 1, MPI_INT, HOST_PROCESSOR, I_NEED_HELP_TAG, MPI_COMM_WORLD, &status); // get answer

		if (help_processor != 0) {
			//cout << "slave " << rank << " have help from " << help_processor << endl;
			str Gp;
			Gp.G = G;
			Gp.F.push_back(F*(L - P*L));
			Gp.ex = connec;
			int str_buf_size = Gp.F.size()*buf_size + sizeof(int) + sizeof(bool);
			for (int i = 1; i < Gp.G.size(); i++) str_buf_size += i*buf_size + sizeof(int);
			char *buf_str = Pack_data(Gp, buf_size, vector_size);
			MPI_Send(buf_str, str_buf_size, MPI_PACKED, help_processor, GPINFO_TAG, MPI_COMM_WORLD);
			delete[] buf_str;
		}
		else procedure(G, F*(L - P*L), connec, buf_size, vector_size, rank); // Rsx = w

		G = G1;
		if (x == 0 || x == 1 || y == 0 || y == 1) // only 1 case can be
			(x == 0 || x == 1) ? contractedge(G, y, x) : contractedge(G, x, y); // don't delete pivote node, get t
		else {
			int xy = x;
			if (y < xy) xy--; // not forget about xy
			contractedge(G, y, x);
			renumerate(G, xy, 1); // if terminal x or y is pivote
		}

		edge m;

		if (!connec) {
			visited = visited1;
			int component1_size = 0;
			for (int i = 0; i < visited.size(); i++) if (visited[i]) component1_size++;
			int component2_size = visited.size() - component1_size;

			if (component1_size != 1 && component2_size != 1) {
				cout << "Decomposition chr1:" << component1_size << " & " << component2_size << endl;
				vector<vector<edge>> J(G1);

				if (!terminal) s = 0; // else s = s 
				if ((visited[s] && visited[x]) || (!visited[s] && !visited[x]))
					(terminal) ? renumerate(G, x, t) : renumerate(G, x, 1);
				if ((visited[s] && visited[y]) || (!visited[s] && !visited[y]))
					(terminal) ? renumerate(G, y, t) : renumerate(G, y, 1);

				procedure(J, F, connec, buf_size, vector_size, rank); // dont *F twice
			}
			else procedure(G, F*P*L, connected, buf_size, vector_size, rank);
		}
		else procedure(G, F*P*L, connected, buf_size, vector_size, rank);

		//procedure(G, F*P*L, connected, buf_size, vector_size, rank); // Rs<x,y> = m, no renumerate becouse s,t always pivote nodes

		return;
		//return (P - P*L)*k + (L - P*L)*w + P*L*m
	}
	if (count == 2) {
		//cout << "slave " << rank << " get hain with 2 pivotes" << endl;
		ch2++;
		vector<int> Ch(Chain);
		vector<int>::iterator it, r;
		for (it = Ch.begin(); it<Ch.end(); ++it) if (*it == 0) r = it;
		int q = r - Ch.begin(); // place of pivote node s=0 into vector Ch 
		for (it = Ch.begin(); it<Ch.end(); ++it) if (*it == 1) r = it;
		int w = r - Ch.begin(); // place of pivote node t=1 into vector Ch 
		if (q > w) { // that * edges in chain, q<w always
			int u = q;
			q = w;
			w = u;
		}

		edge P, L;
		P.C.push_back(1);
		L.C.push_back(1);
		vector<vector<edge>> G(H);
		for (int i = 0; i<q; i++) L = L*G[Ch[i]][Ch[i + 1]];
		for (int i = q; i<w; i++) P = P*G[Ch[i]][Ch[i + 1]];
		for (int i = w; i<Ch.size() - 1; i++) L = L*G[Ch[i]][Ch[i + 1]];
		for (int i = 1; i<Ch.size() - 1; i++) {
			delnode(G, Ch[i]);
			for (int j = 0; j<Ch.size(); j++) if (Ch[i] < Ch[j]) Ch[j]--; // not forget about Ch
		}
		int x = Ch.front(), y = Ch.back(); // doesn't matter x<=>y

		bool connec = gconnected(G);
		if (!connec) cout << "Get unconnected in chr2" << endl;

		if (x != 0 && x != 1 && y != 0 && y != 1) {
			renumerate(G, x, 0);
			renumerate(G, y, 1);
		}
		if (x == 0 && y != 1) renumerate(G, y, 1);
		if (x == 1 && y != 0) renumerate(G, y, 0);
		if (y == 0 && x != 1) renumerate(G, x, 1);
		if (y == 1 && x != 0) renumerate(G, x, 0);

		sum = sum + F*P;
		procedure(G, F*(L - P*L), connec, buf_size, vector_size, rank); // Rxy = n

		return;
		//return F*P + (L - P*L)*n
	}

	edge W = fedge(H);
	if ((W.node1 == 1 && W.node2 == 0) || (W.node1 == 0 && W.node2 == 1)) cout << "Eror in Allowing edge" << endl;
	if (W.node1 == 1 || W.node1 == 0) cout << "Eror in Allowing edge" << endl;

	vector<vector<edge>> H1(H.size());
	vector<vector<edge>> H2(H.size());
	for (int i = 0; i<H.size(); i++) { // Copy H
		H1[i] = H[i];
		H2[i] = H[i];
	}
	edge F1, F2;
	if (W.simple == 0 && W.power == 1) {
		F1 = F;
		F1.power++;
		F2 = F;
		F2.power++;
		F2.C.insert(F2.C.begin(), 0);
		F2.C.pop_back();
	}
	else {
		F1 = F*W;
		F2 = F*~W;
	}

	deledge(H2, W.node1, W.node2);
	bool connec = gconnected(H2);
	contractedge(H1, W.node1, W.node2);

	//cout << "slave " << rank << " ask for help" << endl;
	int just_value;
	MPI_Send(&just_value, 0, MPI_INT, HOST_PROCESSOR, I_NEED_HELP_TAG, MPI_COMM_WORLD); // ask for help
	int help_processor = 0;
	MPI_Status status;
	MPI_Recv(&help_processor, 1, MPI_INT, HOST_PROCESSOR, I_NEED_HELP_TAG, MPI_COMM_WORLD, &status); // get answer

	if (help_processor != 0) { // send half work
		//cout << "slave " << rank << " have help from " << help_processor << endl;
		str Gp;
		Gp.G = H1;
		Gp.F.push_back(F1);
		Gp.ex = connected;
		int str_buf_size = Gp.F.size()*buf_size + sizeof(int) + sizeof(bool);
		for (int i = 1; i < Gp.G.size(); i++) str_buf_size += i*buf_size + sizeof(int);
		char *buf_str = Pack_data(Gp, buf_size, vector_size);
		MPI_Send(buf_str, str_buf_size, MPI_PACKED, help_processor, GPINFO_TAG, MPI_COMM_WORLD);
		delete[] buf_str;
	}
	else procedure(H1, F1, connected, buf_size, vector_size, rank);

	procedure(H2, F2, connec, buf_size, vector_size, rank); // continue with graph wehere deleted
}

void Bcast_data(int buf_size, int vector_size)
{
	MPI_Bcast(&buf_size, 1, MPI_INT, HOST_PROCESSOR, MPI_COMM_WORLD);
	MPI_Bcast(&vector_size, 1, MPI_INT, HOST_PROCESSOR, MPI_COMM_WORLD);
	int vector_buf_size = Bin.size()*buf_size + sizeof(int);
	MPI_Bcast(&vector_buf_size, 1, MPI_INT, HOST_PROCESSOR, MPI_COMM_WORLD);
	char* buf_vector = Pack_vector(Bin, buf_size, vector_size);
	MPI_Bcast(buf_vector, vector_buf_size, MPI_PACKED, HOST_PROCESSOR, MPI_COMM_WORLD);
	delete[] buf_vector;
}

void master(int size) 
{
	str Gp;
	Gp.G = get_info();
	if (!Gp.G.empty()) {
		Gp.F.push_back(Bin.front());
		int vector_size = Bin.front().C.size();
		int buf_size = vector_size * sizeof(double) + 2 * sizeof(int) + sizeof(bool);

		Bcast_data(buf_size, vector_size);

		stack<int> free_processors; // free proc
		//cout << "free_processors : ";
		for (int i = 1; i < size; ++i) {
			//cout << i << " ";
			free_processors.push(i);
		}
		//cout << endl;

		int str_buf_size = Gp.F.size()*buf_size + sizeof(int) + sizeof(bool);
		for (int i = 1; i < Gp.G.size(); i++) str_buf_size += i*buf_size + sizeof(int);
		char* buf_str = Pack_data(Gp, buf_size, vector_size);
		MPI_Send(buf_str, str_buf_size, MPI_PACKED, free_processors.top(), GPINFO_TAG, MPI_COMM_WORLD);
		delete[] buf_str;

		//cout << "Out free " << free_processors.top() << endl;
		free_processors.pop();

		int just_value;
		MPI_Status status;
		while (free_processors.size() != size - 1)
		{
			MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status); // проверяем, что нам пришло

			if (status.MPI_TAG == I_AM_FREE_TAG) // если кто-то освободился
			{
				//cout << "In free " << status.MPI_SOURCE << endl;
				MPI_Recv(&just_value, 0, MPI_INT, status.MPI_SOURCE, I_AM_FREE_TAG, MPI_COMM_WORLD, &status); // добавляем свободный процессор обратно в стек/очередь/etc
				free_processors.push(status.MPI_SOURCE);
			}
			else if (status.MPI_TAG == I_NEED_HELP_TAG) // если кому-то нужна помощь
			{
				MPI_Recv(&just_value, 0, MPI_INT, status.MPI_SOURCE, I_NEED_HELP_TAG, MPI_COMM_WORLD, &status);

				int help_processor = 0;
				if (free_processors.size() > 0) // если есть свободный процессор
				{
					//cout << "Out free " << free_processors.top() << endl;
					help_processor = free_processors.top(); // вытаскиваем его из стека
					free_processors.pop();
				}

				MPI_Send(&help_processor, 1, MPI_INT, status.MPI_SOURCE, I_NEED_HELP_TAG, MPI_COMM_WORLD); // отправляем ноль или номер процесса
			}
		}

		//cout << "send stop" << endl;
		for (int i = 1; i < size; i++) MPI_Send(&i, 0, MPI_INT, i, STOP_TAG, MPI_COMM_WORLD);

		//cout << "get result" << endl;
		int n, n2, n3, n4, n5, cr, c1, c2;
		for (int i = 1; i < size; i++) 
		{
			//cout << "get from " << i << endl;
			char* buf = new char[buf_size];
			MPI_Recv(buf, buf_size, MPI_PACKED, MPI_ANY_SOURCE, SUM_TAG, MPI_COMM_WORLD, &status);
			edge s = Unpack(buf, buf_size, vector_size, 0);
			delete[] buf;
			//s.printedge();
			sum = sum + s;
			MPI_Recv(&n, 1, MPI_INT, MPI_ANY_SOURCE, NUM_TAG, MPI_COMM_WORLD, &status);
			num += n;
			MPI_Recv(&n2, 1, MPI_INT, MPI_ANY_SOURCE, NUM2_TAG, MPI_COMM_WORLD, &status);
			num2 += n2;
			MPI_Recv(&n3, 1, MPI_INT, MPI_ANY_SOURCE, NUM3_TAG, MPI_COMM_WORLD, &status);
			num3 += n3;
			MPI_Recv(&n4, 1, MPI_INT, MPI_ANY_SOURCE, NUM4_TAG, MPI_COMM_WORLD, &status);
			num4 += n4;
			MPI_Recv(&n5, 1, MPI_INT, MPI_ANY_SOURCE, NUM5_TAG, MPI_COMM_WORLD, &status);
			num5 += n5;
			MPI_Recv(&cr, 1, MPI_INT, MPI_ANY_SOURCE, CHR_TAG, MPI_COMM_WORLD, &status);
			chr += cr;
			MPI_Recv(&c1, 1, MPI_INT, MPI_ANY_SOURCE, CH1_TAG, MPI_COMM_WORLD, &status);
			ch1 += c1;
			MPI_Recv(&c2, 1, MPI_INT, MPI_ANY_SOURCE, CH2_TAG, MPI_COMM_WORLD, &status);
			ch2 += c2;
		}

		cout << "Solution:" << endl;
		sum.printedge();
		cout << "Were ends of recursion:" << num << endl;
		cout << " 2-dimension graph:" << num2 << endl;
		cout << " 3-dimension graph:" << num3 << endl;
		cout << " 4-dimension graph:" << num4 << endl;
		cout << " 5-dimension graph:" << num5 << endl;
		cout << "Were chains:" << chr + ch1 + ch2 << endl;
		cout << " chains reduced:" << chr << endl;
		cout << " 1-st type:" << ch1 << endl;
		cout << " 2-d type:" << ch2 << endl;

		double value = 0, p = 0.9, z = 0.1;
		int q = sum.power;
		for (int i = 0; i < q; i++) value += sum.C[i] * pow(p, q - i)*pow(z, i);
		cout << "Value at point " << p << ": " << fixed << setprecision(15) << value << endl;
	}

}

void slaves(int rank) 
{
	//cout << "slave " << rank << endl;
	int buf_size;
	MPI_Bcast(&buf_size, 1, MPI_INT, HOST_PROCESSOR, MPI_COMM_WORLD);

	int vector_size;
	MPI_Bcast(&vector_size, 1, MPI_INT, HOST_PROCESSOR, MPI_COMM_WORLD);

	int vector_buf_size;
	MPI_Bcast(&vector_buf_size, 1, MPI_INT, HOST_PROCESSOR, MPI_COMM_WORLD);

	char *buf_vector = new char[vector_buf_size];
	MPI_Bcast(buf_vector, vector_buf_size, MPI_PACKED, HOST_PROCESSOR, MPI_COMM_WORLD);

	Bin = Unpack_vector(buf_vector, buf_size, vector_size, 0);
	delete[] buf_vector;

	//cout << "Bcast get for slave " << rank << endl; // work

	MPI_Status status;
	int just_value;
	do // have 2 tags
	{
		MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

		if (status.MPI_TAG == GPINFO_TAG) {
			//cout << "slave " << rank << " get work" << endl;
			int get_size;
			MPI_Get_count(&status, MPI_PACKED, &get_size);
			char *buf_str = new char[get_size];
			MPI_Recv(buf_str, get_size, MPI_PACKED, MPI_ANY_SOURCE, GPINFO_TAG, MPI_COMM_WORLD, &status);
			str Gp = Unpack_data(buf_str, buf_size, vector_size);
			delete[] buf_str;

			procedure(Gp.G, Gp.F.front(), Gp.ex, buf_size, vector_size, rank);

			//cout << "slave " << rank << " get free" << endl;
			MPI_Send(&just_value, 0, MPI_INT, HOST_PROCESSOR, I_AM_FREE_TAG, MPI_COMM_WORLD);
		}
	} while (status.MPI_TAG != STOP_TAG);

	//cout << "slave " << rank << " stop working and send result" << endl;
	char *buf_edge = Pack(sum, buf_size, vector_size);
	MPI_Send(buf_edge, buf_size, MPI_PACKED, HOST_PROCESSOR, SUM_TAG, MPI_COMM_WORLD);
	delete[] buf_edge;
	MPI_Send(&num, 1, MPI_INT, HOST_PROCESSOR, NUM_TAG, MPI_COMM_WORLD);
	MPI_Send(&num2, 1, MPI_INT, HOST_PROCESSOR, NUM2_TAG, MPI_COMM_WORLD);
	MPI_Send(&num3, 1, MPI_INT, HOST_PROCESSOR, NUM3_TAG, MPI_COMM_WORLD);
	MPI_Send(&num4, 1, MPI_INT, HOST_PROCESSOR, NUM4_TAG, MPI_COMM_WORLD);
	MPI_Send(&num5, 1, MPI_INT, HOST_PROCESSOR, NUM5_TAG, MPI_COMM_WORLD);
	MPI_Send(&chr, 1, MPI_INT, HOST_PROCESSOR, CHR_TAG, MPI_COMM_WORLD);
	MPI_Send(&ch1, 1, MPI_INT, HOST_PROCESSOR, CH1_TAG, MPI_COMM_WORLD);
	MPI_Send(&ch2, 1, MPI_INT, HOST_PROCESSOR, CH2_TAG, MPI_COMM_WORLD);
}

int main(int argc, char **argv)
{
	int rank, size;
	MPI_Init(&argc, &argv); // выполняет инициализацию библиотеки MPI, работает с начала main
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // возвращает идентификатор (номер, ранг) процесса в рамках заданного коммуникатора
	MPI_Comm_size(MPI_COMM_WORLD, &size); // OUT size

	if (rank == 0) {
		double startTime = MPI_Wtime();
		master(size);
		double endTime = MPI_Wtime();
		cout << "Time of programm : " << endTime - startTime << endl;
	}
	else slaves(rank);

	MPI_Finalize();

	input.close();
	output.close();
	return 0;
}