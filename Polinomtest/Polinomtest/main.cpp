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
/*
edge chainreduction1(vector<vector<edge>> &H, edge sum, vector<int> V, edge F, bool connected)
{
	output << "chainreduction1:" << endl;

	output << "H:" << endl;
	for (int i = 0; i < H.size(); i++) { for (int j = 0; j < H[i].size(); j++) output << H[i][j].ex << " "; output << endl; }

	output << "V before:" << endl;
	copy(V.begin(), V.end(), ostream_iterator<int>(output, " "));
	output << endl;

	vector<int> C = fchain(H, V);

	for (int i = 0; i < C.size(); i++) if (!exis(V, C[i])) V.push_back(C[i]);

	output << "Chain:" << endl;
	copy(C.begin(), C.end(), ostream_iterator<int>(output, " "));
	output << endl;

	output << "V after:" << endl;
	copy(V.begin(), V.end(), ostream_iterator<int>(output, " "));
	output << endl;

	if (!C.empty()) {
		chrs++;
		vector<int>::iterator largest = max_element(C.begin(), C.end());

		for (int i = 0; i < *largest; i++) if (!exis(C, i)) {
			output << "Chain has not:" << i << endl;

			output << "mask1 before:" << endl;
			for (int i = 0; i < mask1.size(); i++) { for (int j = 0; j < mask1[i].size(); j++) output << mask1[i][j] << " "; output << endl; }

			edge T;
			T.C.push_back(1);
			vector<int> Ch = C;
			vector<vector<edge>> G(H.size());
			for (int j = 0; j < H.size(); j++) G[j] = H[j];
			for (int j = 0; j < Ch.size() - 1; j++) T = T*G[Ch[j]][Ch[j + 1]];

			output << "T:" << endl;
			copy(T.C.begin(), T.C.end(), ostream_iterator<int>(output, " "));
			output << endl << T.power << " " << T.simple << endl;

			edge F1, F2;
			int F3 = 0;

			for (int j = 0; j < Ch.size() - 1; j++) if (Ch[j] > i && mask1[i][Ch[j]]) { // j=Ch.size() - 1 doesn't make anything
				edge L; // save power,simple, but lost P.C
				L.C.push_back(1);

				output << "L befor:" << endl;
				copy(L.C.begin(), L.C.end(), ostream_iterator<int>(output, " "));
				output << endl << L.power << " " << L.simple << endl;


				for (int k = 0; k < j; k++) L = L*G[Ch[k]][Ch[k + 1]];

				output << "L after:" << endl;
				copy(L.C.begin(), L.C.end(), ostream_iterator<int>(output, " "));
				output << endl << L.power << " " << L.simple << endl;

				output << "F1 before:" << endl;
				copy(F1.C.begin(), F1.C.end(), ostream_iterator<int>(output, " "));
				output << endl << F1.power << " " << F1.simple << endl;

				F1 = F1 + (L - T);

				output << "F1 after:" << endl;
				copy(F1.C.begin(), F1.C.end(), ostream_iterator<int>(output, " "));
				output << endl << F1.power << " " << F1.simple << endl;
			}

			output << "F2 before:" << endl;
			copy(F2.C.begin(), F2.C.end(), ostream_iterator<int>(output, " "));
			output << endl << F2.power << " " << F2.simple << endl;

			for (int j = 1; j < Ch.size(); j++) if (Ch[j] > i && mask1[i][Ch[j]]) { // j=0 doesn't make anything
				edge P;
				P.C.push_back(1);
				for (int k = j; k < Ch.size() - 1; k++) P = P*G[Ch[k]][Ch[k + 1]];
				F2 = F2 + (P - T);
			}

			output << "F2 after:" << endl;
			copy(F2.C.begin(), F2.C.end(), ostream_iterator<int>(output, " "));
			output << endl << F2.power << " " << F2.simple << endl;

			for (int j = 0; j < Ch.size(); j++) if (Ch[j] > i && mask1[i][Ch[j]]) F3++;

			output << "F3:" << F3 << endl;

			for (int j = 0; j < Ch.size(); j++) if (Ch[j] > i && mask1[i][Ch[j]]) {
				mask1[i][Ch[j]] = false;
				mask1[Ch[j]][i] = false;
			}

			output << "mask1 after:" << endl;
			for (int i = 0; i < mask1.size(); i++) { for (int j = 0; j < mask1[i].size(); j++) output << mask1[i][j] << " "; output << endl; }

			int s = i;
			for (int j = 1; j < Ch.size() - 1; j++) { // after this we get 2 nodes from chain, one of them can be pivote
				delnode(G, Ch[j]);
				if (Ch[j] < s) s--; // not forget about s
				for (int k = 0; k < Ch.size(); k++) if (Ch[j] < Ch[k]) Ch[k]--; // not forget about Ch
			}
			int x = Ch.front(), y = Ch.back(); // matter x<=>y

			output << "x = " << x << ", y = " << y << endl;

			vector<vector<edge>> G1(G);
			int s1 = s;
			bool connec = gconnected(G);
			vector<bool> visited1(visited);
			if (!connec) cout << "Get unconnected in chainreduction1" << endl;

			output << "visited1 : ";
			copy(visited1.begin(), visited1.end(), ostream_iterator<int>(output, " "));
			output << endl;

			if (s != 0 && s != 1 && y != 0 && y != 1) {
				output << "Renumerate:" << s << " 0" << endl;
				renumerate(G, s, 0);
				output << "Renumerate:" << y << " 1" << endl;
				renumerate(G, y, 1);
			}
			if (s == 0 && y != 1) {
				output << "Renumerate:" << y << " 1" << endl;
				renumerate(G, y, 1);
			}
			if (s == 1 && y != 0) {
				output << "Renumerate:" << y << " 0" << endl;
				renumerate(G, y, 0);
			}
			if (y == 0 && s != 1) {
				output << "Renumerate:" << s << " 1" << endl;
				renumerate(G, s, 1);
			}
			if (y == 1 && s != 0) {
				output << "Renumerate:" << s << " 0" << endl;
				renumerate(G, s, 0);
			}

			output << "visited : ";
			copy(visited.begin(), visited.end(), ostream_iterator<int>(output, " "));
			output << endl;

			edge k = procedure(G, F, connec); // Rsy

			output << "k:" << endl;
			copy(k.C.begin(), k.C.end(), ostream_iterator<int>(output, " "));
			output << endl << k.power << " " << k.simple << endl;

			visited = visited1;
			G = G1;
			if (s != 0 && s != 1 && x != 0 && x != 1) {
				output << "Renumerate:" << s << " 0" << endl;
				renumerate(G, s, 0);
				output << "Renumerate:" << x << " 1" << endl;
				renumerate(G, x, 1);
			}
			if (s == 0 && x != 1) {
				output << "Renumerate:" << x << " 1" << endl;
				renumerate(G, x, 1);
			}
			if (s == 1 && x != 0) {
				output << "Renumerate:" << x << " 0" << endl;
				renumerate(G, x, 0);
			}
			if (x == 0 && s != 1) {
				output << "Renumerate:" << s << " 1" << endl;
				renumerate(G, s, 1);
			}
			if (x == 1 && s != 0) {
				output << "Renumerate:" << s << " 0" << endl;
				renumerate(G, s, 0);
			}

			output << "visited : ";
			copy(visited.begin(), visited.end(), ostream_iterator<int>(output, " "));
			output << endl;

			edge w = procedure(G, F, connec);// Rsx

			output << "w:" << endl;
			copy(w.C.begin(), w.C.end(), ostream_iterator<int>(output, " "));
			output << endl << w.power << " " << w.simple << endl;

			G = G1;
			int xy = x;
			if (y < xy) xy--; // not forget
			if (y < s) s--; // not forget
			contractedge(G, y, x); // x or y can't be s
			output << "<x,y>:" << xy << endl;
			if (s != 0 && s != 1 && xy != 0 && xy != 1) {
				output << "Renumerate:" << s << " 0" << endl;
				renumerate(G, s, 0);
				output << "Renumerate:" << xy << " 1" << endl;
				renumerate(G, xy, 1);
			}
			if (s == 0 && xy != 1) {
				output << "Renumerate:" << xy << " 1" << endl;
				renumerate(G, xy, 1);
			}
			if (s == 1 && xy != 0) {
				output << "Renumerate:" << xy << " 0" << endl;
				renumerate(G, xy, 0);
			}
			if (xy == 0 && s != 1) {
				output << "Renumerate:" << s << " 1" << endl;
				renumerate(G, s, 1);
			}
			if (xy == 1 && s != 0) {
				output << "Renumerate:" << s << " 0" << endl;
				renumerate(G, s, 0);
			}
			edge m;

			if (!connec) {
				visited = visited1;
				int component1_size = 0;
				for (int i = 0; i < visited.size(); i++) if (visited[i]) component1_size++;
				int component2_size = visited.size() - component1_size;

				if (component1_size != 1 && component2_size != 1) {
					decomp1++;
					cout << "Decomposition chainreduction1:";
					cout << component1_size << " & " << component2_size << endl;
					if ((visited[x] && visited[y]) || (!visited[x] && !visited[y])) cout << "Eror in decomposition chainreductiom" << endl;
					output << "---------------------------------------------------------------------------------------------------------------------->Decomposition chainreduction1" << endl;

					vector<vector<edge>> J(G1.size());
					for (int i = 0; i < G1.size(); i++) J[i] = G1[i];

					output << "G1:" << endl; // not changed
					for (int i = 0; i < G1.size(); i++) { for (int j = 0; j < G1[i].size(); j++) output << G1[i][j].ex << " "; output << endl; }
					output << "Get visited : ";
					copy(visited.begin(), visited.end(), ostream_iterator<int>(output, " "));
					output << endl;
					output << "x = " << x << ", y = " << y << endl;
					output << "s before = " << s << endl;

					s = s1;

					output << "s after = " << s << endl;

					if ((visited[s] && visited[x]) || (!visited[s] && !visited[x])) {
						output << "s & x" << endl;
						if (s != 0 && s != 1 && x != 0 && x != 1) {
							output << "Renumerate:" << s << " 0" << endl;
							renumerate(J, s, 0);
							output << "Renumerate:" << x << " 1" << endl;
							renumerate(J, x, 1);
						}
						if (s == 0 && x != 1) {
							output << "Renumerate:" << x << " 1" << endl;
							renumerate(J, x, 1);
						}
						if (s == 1 && x != 0) {
							output << "Renumerate:" << x << " 0" << endl;
							renumerate(J, x, 0);
						}
						if (x == 0 && s != 1) {
							output << "Renumerate:" << s << " 1" << endl;
							renumerate(J, s, 1);
						}
						if (x == 1 && s != 0) {
							output << "Renumerate:" << s << " 0" << endl;
							renumerate(J, s, 0);
						}
					}
					else {
						output << "s & y" << endl;
						if (s != 0 && s != 1 && y != 0 && y != 1) {
							output << "Renumerate:" << s << " 0" << endl;
							renumerate(J, s, 0);
							output << "Renumerate:" << y << " 1" << endl;
							renumerate(J, y, 1);
						}
						if (s == 0 && y != 1) {
							output << "Renumerate:" << y << " 1" << endl;
							renumerate(J, y, 1);
						}
						if (s == 1 && y != 0) {
							output << "Renumerate:" << y << " 0" << endl;
							renumerate(J, y, 0);
						}
						if (y == 0 && s != 1) {
							output << "Renumerate:" << s << " 1" << endl;
							renumerate(J, s, 1);
						}
						if (y == 1 && s != 0) {
							output << "Renumerate:" << s << " 0" << endl;
							renumerate(J, s, 0);
						}
					}

					output << "J1:" << endl;
					for (int i = 0; i < J.size(); i++) { for (int j = 0; j < J[i].size(); j++) output << J[i][j].ex << " "; output << endl; }
					output << "visited : ";
					copy(visited.begin(), visited.end(), ostream_iterator<int>(output, " "));
					output << endl;

					m = procedure(J, F, connec); // dont *F twice
				}
				else m = procedure(G, F, connected); // Rs,xy
			}
			else m = procedure(G, F, connected); // Rs,xy

			//m = procedure(G, F, connected); // Rs,xy

			output << "m:" << endl;
			copy(m.C.begin(), m.C.end(), ostream_iterator<int>(output, " "));
			output << endl << m.power << " " << m.simple << endl;

			output << "sum before:" << endl;
			copy(sum.C.begin(), sum.C.end(), ostream_iterator<int>(output, " "));
			output << endl << sum.power << " " << sum.simple << endl;

			output << "Start computing sum + F2*k + F1*w + F3*T*m:" << endl;
			sum = sum + (F2*k + F1*w + F3*T*m);

			output << "sum after:" << endl;
			copy(sum.C.begin(), sum.C.end(), ostream_iterator<int>(output, " "));
			output << endl << sum.power << " " << sum.simple << endl;
		}
		else {
			output << "Chain has:" << i << endl;

			vector<int> Ch = C;
			vector<int>::iterator it, r;
			for (it = Ch.begin(); it < Ch.end(); ++it) if (*it == i) r = it;
			int q = r - Ch.begin(); // place of pivote node s=i into vector Ch
			output << "Place " << i << " in vector Ch:" << q << endl;

			output << "mask1 before:" << endl;
			for (int i = 0; i < mask1.size(); i++) { for (int j = 0; j < mask1[i].size(); j++) output << mask1[i][j] << " "; output << endl; }

			edge T;
			T.C.push_back(1);
			vector<vector<edge>> G(H.size());
			for (int j = 0; j < H.size(); j++) G[j] = H[j];
			for (int j = 0; j < Ch.size() - 1; j++) T = T*G[Ch[j]][Ch[j + 1]];

			output << "T:" << endl;
			copy(T.C.begin(), T.C.end(), ostream_iterator<int>(output, " "));
			output << endl << T.power << " " << T.simple << endl;

			edge F1, F2;
			int F3 = 0;
			for (int j = 0; j < Ch.size(); j++) if (Ch[j] > i && mask1[i][Ch[j]]) {
				edge P; // save power,simple, but lost P.C
				P.C.push_back(1);

				output << "P befor:" << endl;
				copy(P.C.begin(), P.C.end(), ostream_iterator<int>(output, " "));
				output << endl << P.power << " " << P.simple << endl;

				if (j < q) for (int k = j; k < q; k++) P = P*G[Ch[k]][Ch[k + 1]];
				else for (int k = q; k < j; k++) P = P*G[Ch[k]][Ch[k + 1]];

				output << "P after:" << endl;
				copy(P.C.begin(), P.C.end(), ostream_iterator<int>(output, " "));
				output << endl << P.power << " " << P.simple << endl;

				output << "F1 before:" << endl;
				copy(F1.C.begin(), F1.C.end(), ostream_iterator<int>(output, " "));
				output << endl << F1.power << " " << F1.simple << endl;

				F1 = F1 + P;

				output << "F1 after:" << endl;
				copy(F1.C.begin(), F1.C.end(), ostream_iterator<int>(output, " "));
				output << endl << F1.power << " " << F1.simple << endl;
			}

			output << "F2 before:" << endl;
			copy(F2.C.begin(), F2.C.end(), ostream_iterator<int>(output, " "));
			output << endl << F2.power << " " << F2.simple << endl;

			for (int j = 0; j < Ch.size(); j++) if (Ch[j] > i && mask1[i][Ch[j]]) {
				edge L;
				L.C.push_back(1);
				if (j < q) {
					for (int k = 0; k < j; k++) L = L*G[Ch[k]][Ch[k + 1]];
					for (int k = q; k < Ch.size() - 1; k++) L = L*G[Ch[k]][Ch[k + 1]];
				}
				else {
					for (int k = 0; k < q; k++) L = L*G[Ch[k]][Ch[k + 1]];
					for (int k = j; k < Ch.size() - 1; k++) L = L*G[Ch[k]][Ch[k + 1]];
				}
				F2 = F2 + L;
			}

			output << "F2 after:" << endl;
			copy(F2.C.begin(), F2.C.end(), ostream_iterator<int>(output, " "));
			output << endl << F2.power << " " << F2.simple << endl;

			for (int j = 0; j < Ch.size(); j++) if (Ch[j] > i && mask1[i][Ch[j]]) F3++;

			output << "F3:" << F3 << endl;

			for (int j = 0; j < Ch.size(); j++) if (Ch[j] > i && mask1[i][Ch[j]]) {
				mask1[i][Ch[j]] = false;
				mask1[Ch[j]][i] = false;
			}

			output << "mask1 after:" << endl;
			for (int i = 0; i < mask1.size(); i++) { for (int j = 0; j < mask1[i].size(); j++) output << mask1[i][j] << " "; output << endl; }

			for (int j = 1; j < Ch.size() - 1; j++) {
				delnode(G, Ch[j]);
				for (int k = 0; k < Ch.size(); k++) if (Ch[j] < Ch[k]) Ch[k]--; // not forget about Ch
			}
			int x = Ch.front(), y = Ch.back(); // doesn't matter x<=>y

			bool connec = gconnected(G);
			if (!connec) cout << "Get unconnected in chainreduction2" << endl;

			if (x != 0 && x != 1 && y != 0 && y != 1) {
				output << "Renumerate:" << x << " 0" << endl;
				renumerate(G, x, 0);
				output << "Renumerate:" << y << " 1" << endl;
				renumerate(G, y, 1);
			}
			if (x == 0 && y != 1) {
				output << "Renumerate:" << y << " 1" << endl;
				renumerate(G, y, 1);
			}
			if (x == 1 && y != 0) {
				output << "Renumerate:" << y << " 0" << endl;
				renumerate(G, y, 0);
			}
			if (y == 0 && x != 1) {
				output << "Renumerate:" << x << " 1" << endl;
				renumerate(G, x, 1);
			}
			if (y == 1 && x != 0) {
				output << "Renumerate:" << x << " 0" << endl;
				renumerate(G, x, 0);
			}

			edge n = procedure(G, F, connec); // Rxy

			output << "n:" << endl;
			copy(n.C.begin(), n.C.end(), ostream_iterator<int>(output, " "));
			output << endl << n.power << " " << n.simple << endl;

			output << "sum before:" << endl;
			copy(sum.C.begin(), sum.C.end(), ostream_iterator<int>(output, " "));
			output << endl << sum.power << " " << sum.simple << endl;

			output << "Start computing sum + F1 + F2*n - F3*T*n:" << endl;
			sum = sum + (F1 + F2*n - F3*T*n);

			output << "sum after:" << endl;
			copy(sum.C.begin(), sum.C.end(), ostream_iterator<int>(output, " "));
			output << endl << sum.power << " " << sum.simple << endl;
		}
		return chainreduction1(H, sum, V, F, connected);
	}
	return sum;
}
*/
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

int main()
{
	setlocale(LC_ALL, "");
	if (!input.is_open()) cout << "File can not be opened!\n";
	else {
		char str[50];
		int n = 0, m = 0;
		vector<edge> E;
		input.getline(str, 50); output << str << endl;
		int buf, count = 0;
		input >> buf; output << "Number of nodes n=" << buf << endl;
		n = buf;
		input >> buf; output << "Number of edges m=" << buf << endl;
		m = buf;

		while (count != m) {
			bool reduct = false;
			edge q; // filling data for new edge
			q.simple = 0;
			q.ex = true;
			q.power = 1;
			q.C.push_back(1);
			q.C.resize(m + 1); // numerate start from 0
			input >> buf; output << buf - 1 << " ";
			q.node1 = buf - 1;
			input >> buf; output << buf - 1 << endl;
			q.node2 = buf - 1;
			for (int i = 0; i<E.size(); i++) // work with multi edges
				if ((E[i].node1 == q.node1 && E[i].node2 == q.node2) || (E[i].node1 == q.node2 && E[i].node2 == q.node1)) {
					E[i].simple++;
					output << "E[" << i << "].simple=" << E[i].simple << endl;
					reduct = true;
				}
			if (!reduct) E.push_back(q);
			count++;
		}

		input >> str; output << str << endl; // end of input
		if (strcmp(str, "$$$") != 0) cout << "Uncorrect entry" << endl; // 0 = ; 1 < ; -1 >
		else {
			cout << "Correct entry" << endl;

			Bin.resize(m + 1); // max power is m-1
			for (int i = 0; i<Bin.size(); i++) {
				Bin[i].C.resize(m + 1);
				Bin[i].power = i;
				Bin[i].simple = 0;
				Bin[i].C.front() = 1;
				if (i != 0) for (int j = 1; j<m + 1; j++) Bin[i].C[j] = Bin[i - 1].C[j - 1] + Bin[i - 1].C[j];
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
			vector<bool> visited1 = visited;
			if (!r) cout << "Unconnected graph on input!" << endl;

			edge F = Bin.front(); // create pseudo edge for computing

			unsigned int start_time = clock();
			edge sum;

			mask1.resize(S.size());
			for (int i = 0; i<mask1.size(); i++) {
				mask1[i].resize(S[i].size());
				for (int j = 0; j<mask1[i].size(); j++) i != j ? mask1[i][j] = true : mask1[i][j] = false;
			}

			vector<int> V;
			sum = chainreduction1(S, sum, V, r);

			//cout << "chainreduction:" << endl;
			//sum.printedge();

			double value = 0, p = 0.9, z = 0.1;
			int q = sum.power;
			//for (int i = 0; i < q; i++) value += sum.C[i] * pow(p, q - i)*pow(z, i);
			//cout << "Value at point " << p << ": " << value / Bin[S.size()].C[2] << endl;

			for (int i = 0; i<S.size(); i++) for (int j = i + 1; j<S[i].size(); j++) if (mask1[i][j]) {
				//cout << "+R" << i + 1 << j + 1 << endl;
				vector<vector<edge>> G(S);
				if (i != 0 || j != 1) { // after this 0,1
					if (i != 0 && j != 1) {
						renumerate(G, i, 0);
						renumerate(G, j, 1);
					}
					if (i == 0 && j != 1) renumerate(G, j, 1);
					if (i != 0 && j == 1) renumerate(G, i, 0);
				}
				sum = sum + procedure(G, F, true); // G changed
			}

			for (int i = 0; i < sum.C.size(); i++)  sum.C[i] = sum.C[i] / Bin[S.size()].C[2];

/*
			mask.resize(S.size());
			for (int i = 0; i<mask.size(); i++) i == 0 ? mask[i] = false : mask[i] = true;

			vector<int> weight(S.size());
			for (int i = 0; i<weight.size(); i++) weight[i] = 1;

			sum = chainreduction2(S, sum, weight, r);

			//cout << "chainreduction:" << endl;
			//sum.printedge();

			double value = 0, p = 0.9, z = 0.1;
			int q = sum.power;
			//for (int i = 0; i<q; i++) value += sum.C[i] * pow(p, q - i)*pow(z, i);
			//cout << "Value at point " << p << ": " << value << endl;

			for (int i = 0; i < S.size(); i++) if (mask[i]) {
				//cout << "+R1" << i + 1 << endl;
				vector<vector<edge>> G(S);
				if (i != 1) renumerate(G, i, 1); // after this i=1
				sum = sum + procedure(G, F, true); // G changed
			}

			sum = sum + Bin.front();
*/
			unsigned int end_time = clock();
			int search_time = end_time - start_time;
			cout << "Time of programm " << search_time << " msec" << endl;
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

			cout << "Solution:" << endl;
			sum.printedge();

			value = 0;
			q = sum.power;
			for (int i = 0; i < q; i++) value += sum.C[i] * pow(p, q - i)*pow(z, i);
			cout << "Value at point " << p << ": " << fixed << setprecision(15) << value << endl;

/*
			int s, t;
			cout<<"Entry nodes:"<<endl;
			cout<<"s=";
			cin>>s; s--; // numerate start from 0
			cout<<"t=";
			cin>>t; t--;
			if (s != t && s < t) {
				if (s != 0 || t != 1) { // after this 0,1
					if (s != 0 && t != 1) {
						renumerate(S, s, 0);
						renumerate(S, t, 1);
					}
					if (s == 0 && t != 1) renumerate(S, t, 1);
					if (s != 0 && t == 1) renumerate(S, s, 0);
				}
				r = gconnected(S);

				unsigned int start_time = clock();
				edge sum = procedure(S, F, true);
				unsigned int end_time = clock();
				int search_time = end_time - start_time;
				cout << "Time of programm:" << search_time << " msec" << endl;
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

				cout << "Solution:" << endl;
				sum.printedge();

				double value = 0, p = 0.9, z = 0.1;
				int q = sum.power;
				for (int i = 0; i < q; i++) value += sum.C[i] * pow(p, q - i)*pow(z, i);
				cout << "Value at point " << p << ": " << fixed << setprecision(19) << value << endl;
			}
*/
		}
	}

	input.close();
	output.close();

	system("pause>>void");
	return 0;
}