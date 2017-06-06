#include "stdafx.h"
#include "funcs.h"
#include "globals.h"
#include "Cclass.h"

void chainreduction1(vector<vector<edge>> &H, vector<int> V, bool connected, int buf_size, int vector_size, int size)
{
	vector<int> Chain = fchain(H, V);
	for (int i = 0; i<Chain.size(); i++) if (!exis(V, Chain[i])) V.push_back(Chain[i]);

	if (!Chain.empty()) {
		chrs++;
		vector<int>::iterator largest = max_element(Chain.begin(), Chain.end());

		for (int i = 0; i<*largest; i++) if (!exis(Chain, i)) {
			edge T;
			T.C.push_back(1);
			vector<int> Ch(Chain);
			vector<vector<edge>> G(H);
			for (int j = 0; j<Ch.size() - 1; j++) T = T*G[Ch[j]][Ch[j + 1]];

			edge F1, F2;
			int F3 = 0;
			for (int j = 0; j<Ch.size() - 1; j++) if (Ch[j] > i && mask1[i][Ch[j]]) { // j=Ch.size() - 1 doesn't make anything
				edge L; // save power,simple, but lost P.C
				L.C.push_back(1);
				for (int k = 0; k<j; k++) L = L*G[Ch[k]][Ch[k + 1]];
				F1 = F1 + (L - T);
			}
			for (int j = 1; j<Ch.size(); j++) if (Ch[j] > i && mask1[i][Ch[j]]) { // j=0 doesn't make anything
				edge P;
				P.C.push_back(1);
				for (int k = j; k<Ch.size() - 1; k++) P = P*G[Ch[k]][Ch[k + 1]];
				F2 = F2 + (P - T);
			}

			for (int j = 0; j<Ch.size(); j++) if (Ch[j] > i && mask1[i][Ch[j]]) F3++;

			for (int j = 0; j<Ch.size(); j++) if (Ch[j] > i && mask1[i][Ch[j]]) {
				mask1[i][Ch[j]] = false;
				mask1[Ch[j]][i] = false;
			}

			int s = i;
			for (int j = 1; j<Ch.size() - 1; j++) { // after this we get 2 nodes from chain, one of them can be pivote
				delnode(G, Ch[j]);
				if (Ch[j] < s) s--; // not forget about s
				for (int k = 0; k<Ch.size(); k++) if (Ch[j] < Ch[k]) Ch[k]--; // not forget about Ch
			}
			int x = Ch.front(), y = Ch.back(); // matter x<=>y

			vector<vector<edge>> G1(G);
			int s1 = s;
			bool connec = gconnected(G);
			vector<bool> visited1(visited);
			//if (!connec) cout << "Get unconnected in chainreduction1" << endl;

			if (!F2.C.empty()) {
				if (s != 0 && s != 1 && y != 0 && y != 1) {
					renumerate(G, s, 0);
					renumerate(G, y, 1);
				}
				if (s == 0 && y != 1) renumerate(G, y, 1);
				if (s == 1 && y != 0) renumerate(G, y, 0);
				if (y == 0 && s != 1) renumerate(G, s, 1);
				if (y == 1 && s != 0) renumerate(G, s, 0);

				str Gp;
				Gp.G = G;
				Gp.F.push_back(F2);
				Gp.ex = connec;
				Gp.V = visited;
				send_control(Gp, buf_size, vector_size, size); // Rsy
			}

			if (!F1.C.empty()) {
				visited = visited1;
				G = G1;

				if (s != 0 && s != 1 && x != 0 && x != 1) {
					renumerate(G, s, 0);
					renumerate(G, x, 1);
				}
				if (s == 0 && x != 1) renumerate(G, x, 1);
				if (s == 1 && x != 0) renumerate(G, x, 0);
				if (x == 0 && s != 1) renumerate(G, s, 1);
				if (x == 1 && s != 0) renumerate(G, s, 0);

				str Gp;
				Gp.G = G;
				Gp.F.push_back(F1);
				Gp.ex = connec;
				Gp.V = visited;
				send_control(Gp, buf_size, vector_size, size); // Rsx
			}

			G = G1;
			int xy = x;
			if (y < xy) xy--; // not forget
			if (y < s) s--; // not forget
			contractedge(G, y, x); // x or y can't be s
			if (s != 0 && s != 1 && xy != 0 && xy != 1) {
				renumerate(G, s, 0);
				renumerate(G, xy, 1);
			}
			if (s == 0 && xy != 1) renumerate(G, xy, 1);
			if (s == 1 && xy != 0) renumerate(G, xy, 0);
			if (xy == 0 && s != 1) renumerate(G, s, 1);
			if (xy == 1 && s != 0) renumerate(G, s, 0);
			edge m;

			if (!connec) {
				visited = visited1;
				int component1_size = 0;
				for (int i = 0; i < visited.size(); i++) if (visited[i]) component1_size++;
				int component2_size = visited.size() - component1_size;

				if (component1_size != 1 && component2_size != 1) {
					decomp1++;
					//cout << "Decomposition chainredaction1 :" << component1_size << " & " << component2_size << endl;
					if ((visited[x] && visited[y]) || (!visited[x] && !visited[y])) cout << "Eror in decomposition chainreductiom" << endl;
					vector<vector<edge>> J(G1);

					s = s1;
					if ((visited[s] && visited[x]) || (!visited[s] && !visited[x])) {
						if (s != 0 && s != 1 && x != 0 && x != 1) {
							renumerate(J, s, 0);
							renumerate(J, x, 1);
						}
						if (s == 0 && x != 1) renumerate(J, x, 1);
						if (s == 1 && x != 0) renumerate(J, x, 0);
						if (x == 0 && s != 1) renumerate(J, s, 1);
						if (x == 1 && s != 0) renumerate(J, s, 0);
					}
					else {
						if (s != 0 && s != 1 && y != 0 && y != 1) {
							renumerate(J, s, 0);
							renumerate(J, y, 1);
						}
						if (s == 0 && y != 1) renumerate(J, y, 1);
						if (s == 1 && y != 0) renumerate(J, y, 0);
						if (y == 0 && s != 1) renumerate(J, s, 1);
						if (y == 1 && s != 0) renumerate(J, s, 0);
					}

					str Gp;
					Gp.G = J;
					Gp.F.push_back(F3*T); // can't be 0, F3>=1
					Gp.ex = connec;
					Gp.V = visited;
					send_control(Gp, buf_size, vector_size, size); // Rs,xy
				}
				else {
					str Gp;
					Gp.G = G;
					Gp.F.push_back(F3*T); // can't be 0, F3>=1
					Gp.ex = connected;
					Gp.V = visited;
					send_control(Gp, buf_size, vector_size, size); // Rs,xy
				}
			}
			else {
				str Gp;
				Gp.G = G;
				Gp.F.push_back(F3*T); // can't be 0, F3>=1
				Gp.ex = connected;
				Gp.V = visited;
				send_control(Gp, buf_size, vector_size, size); // Rs,xy
			}
		}
		else {
			vector<int> Ch(Chain);
			vector<int>::iterator it, r;
			for (it = Ch.begin(); it<Ch.end(); ++it) if (*it == i) r = it;
			int q = r - Ch.begin(); // place of pivote node s=i into vector Ch 

			edge T;
			T.C.push_back(1);
			vector<vector<edge>> G(H);
			for (int j = 0; j<Ch.size() - 1; j++) T = T*G[Ch[j]][Ch[j + 1]];

			edge F1, F2;
			int F3 = 0;
			for (int j = 0; j<Ch.size(); j++) if (Ch[j] > i && mask1[i][Ch[j]]) {
				edge P; // save power,simple, but lost P.C
				P.C.push_back(1);
				if (j < q) for (int k = j; k<q; k++) P = P*G[Ch[k]][Ch[k + 1]];
				else for (int k = q; k<j; k++) P = P*G[Ch[k]][Ch[k + 1]];
				F1 = F1 + P;
			}
			for (int j = 0; j<Ch.size(); j++) if (Ch[j] > i && mask1[i][Ch[j]]) {
				edge L;
				L.C.push_back(1);
				if (j < q) {
					for (int k = 0; k<j; k++) L = L*G[Ch[k]][Ch[k + 1]];
					for (int k = q; k<Ch.size() - 1; k++) L = L*G[Ch[k]][Ch[k + 1]];
				}
				else {
					for (int k = 0; k<q; k++) L = L*G[Ch[k]][Ch[k + 1]];
					for (int k = j; k<Ch.size() - 1; k++) L = L*G[Ch[k]][Ch[k + 1]];
				}
				F2 = F2 + L;
			}

			for (int j = 0; j<Ch.size(); j++) if (Ch[j] > i && mask1[i][Ch[j]]) F3++;

			for (int j = 0; j<Ch.size(); j++) if (Ch[j] > i && mask1[i][Ch[j]]) {
				mask1[i][Ch[j]] = false;
				mask1[Ch[j]][i] = false;
			}

			for (int j = 1; j<Ch.size() - 1; j++) {
				delnode(G, Ch[j]);
				for (int k = 0; k<Ch.size(); k++) if (Ch[j] < Ch[k]) Ch[k]--; // not forget about Ch
			}
			int x = Ch.front(), y = Ch.back(); // doesn't matter x<=>y

			bool connec = gconnected(G);
			//if (!connec) cout << "Get unconnected in chainreduction2" << endl;

			if (x != 0 && x != 1 && y != 0 && y != 1) {
				renumerate(G, x, 0);
				renumerate(G, y, 1);
			}
			if (x == 0 && y != 1) renumerate(G, y, 1);
			if (x == 1 && y != 0) renumerate(G, y, 0);
			if (y == 0 && x != 1) renumerate(G, x, 1);
			if (y == 1 && x != 0) renumerate(G, x, 0);

			str Gp;
			Gp.G = G;
			Gp.F.push_back(F2 - F3*T); // can't be 0 or 1, F3>=1
			Gp.ex = connec;
			Gp.V = visited;
			send_control(Gp, buf_size, vector_size, size); // Rxy

			sum = sum + F1;
		}
		chainreduction1(H, V, connected, buf_size, vector_size, size);
	}
}