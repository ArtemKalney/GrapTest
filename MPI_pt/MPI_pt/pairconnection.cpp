#include "stdafx.h"
#include "funcs.h"
#include "globals.h"
#include "Cclass.h"

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

inline void delnode(vector<vector<edge>> &H, int q)
{
	H.erase(H.begin() + q);
	for (int i = 0; i<H.size(); i++) H[i].erase(H[i].begin() + q);
}

inline void deledge(vector<vector<edge>> &H, int q, int w)
{
	edge F;
	F.ex = false;
	H[q][w] = F;
	H[w][q] = F;
}

void contractedge(vector<vector<edge>> &H, int q, int w)
{
	for (int i = 0; i<H[q].size(); i++) if (H[q][i].ex && i != w) { // do not consider H[q][w]
		if (H[w][i].ex && H[q][i].power == 1 && H[w][i].power == 1) { // power==1 is falg of simple edge
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

edge penduntreduction(vector<vector<edge>> &H, edge F, int q, bool find)
{
	pendunt++;
	vector<int> nodepower = gnodepower(H);
	if (find) nodepower.erase(nodepower.begin() + q); // in penduntreduction we not consider pendunt node 0 || 1

	vector <int>::iterator it, smallest = nodepower.end() - 1;
	for (it = nodepower.begin(); it<nodepower.end(); ++it) if (*it == 1) smallest = it;
	int r = smallest - nodepower.begin(); // find pendunt node, can be 0,1

	if (*smallest == 1 && H.size() > 2) { //otherwise all will be deleted
		for (int i = H[r].size() - 1; i >= 0; i--) if (H[r][i].ex) {
			//find incident edge to pendunt node, can't be 0,1 || 1,0
			if ((r == 1 && i == 0) || (r == 0 && i == 1)) { // that do not have loop
				q = r;
				find = true;
			}
			if (r != 0 && r != 1) delnode(H, r);
			if ((r == 1 && i != 0) || (r == 0 && i != 1)) {
				F = F*H[r][i];
				delnode(H, r);
				i--; // after delenode not forget about i
				if (i != 1) renumerate(H, i, 1); // always must be 1
			}
			break; // arise the eror if abort this
		}
		return penduntreduction(H, F, q, find);
	}
	return F;

}

bool bridgereduction(vector<vector<edge>> &H)
{
	bridges++;
	if (visited.size() != H.size()) cout << "Eror bridgereduction!" << endl;
	//cout << "Bridge reduction: ";

	if (visited[0] && visited[1]) { // only one of this cases exist
		for (int i = H.size() - 1; i >= 0; i--) if (!visited[i]) {  // when end is 0,0 make mistake if start from 0
			//cout << i + 1 << " ";
			delnode(H, i);
		}
		//cout << endl;
	}

	if (!visited[0] && !visited[1]) {
		for (int i = H.size() - 1; i >= 0; i--) if (visited[i]) {
			//cout << i + 1 << " ";
			delnode(H, i);
		}
		//cout << endl;
	}

	if ((visited[0] && !visited[1]) || (!visited[0] && visited[1])) {
		//cout << "Return 0" << endl;
		return false;
	}

	return true;
}

edge fedge(vector<vector<edge>> &H)
{
	edge F;
	vector<int> nodepower = gnodepower(H);
	vector<int>::iterator it, smallest = nodepower.end() - 1;

	for (it = nodepower.begin(); it<nodepower.end(); ++it)
		if (*it <= *smallest && (it - nodepower.begin() != 0) && (it - nodepower.begin() != 1)) smallest = it;
	//becouse after deledge we can get unconnected graph in case nodepower=1
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
				// find node power = 2 and add nodes of edge where is selected node
				P.push_back(i);
				P.push_back(j);
				find = true;
				break;
			}
		}
		i++;
	}

	if (!P.empty()) { // expand vector in both sides if it is not empty
		i = P.front();
		while (nodepower[i] == 2 && i != P.back())
			// find node incident to left node of our chain and add it
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

vector<int> chainreduction(vector<vector<edge>> &H, vector<int> Ch, vector<int> V, bool find)
{
	vector<int> P = fchain(H, V);
	int count = 0;
	if (!P.empty()) {
		for (int i = 0; i<P.size(); i++) if (P[i] == 0 || P[i] == 1) count++; // count selected nodes in chain

		vector<int>::iterator it, r = P.begin();
		if (count == 1) {
			for (it = P.begin(); it<P.end(); ++it) if (*it == 0 || *it == 1) r = it;
			if ((r == P.begin() || r == P.end() - 1) && !Ch.empty()) count = 0; // reduct special chains
		}

		if ((count == 1 || count == 2) && !find) {
			if (count == 2) { // have only chain with count=2
				find = true;
				V = P;
			}
			if (count == 1) {
				if (!Ch.empty()) {
					for (it = Ch.begin(); it<Ch.end() - 1; ++it) if (*it == 0 || *it == 1) r = it;  // Ch.back()=count
					if (r == Ch.begin() || r == Ch.end() - 2) V.clear(); // next recursive call reduct Ch 
				}
				for (int j = 0; j<P.size(); j++) if (!exis(V, P[j])) V.push_back(P[j]);
			}

			P.push_back(count);
			Ch = P;

			return chainreduction(H, Ch, V, find);
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

			if (H[x][y].ex) { // Becouse newedge.power != 1
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
					// find node incident to left node of our chain and add it
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
			return chainreduction(H, Ch, V, find);
		}
	}
	return Ch;
}

edge procedure(vector<vector<edge>> &H, edge F, bool connected, int buf_size, int vector_size)
{
	factors++;
	if (!connected) {
		edge N;
		N.ex = false;
		if (!bridgereduction(H)) {
			num0++;
			return N;
		}
		connected = true;
	}

	if (H.size() < 6) {
		if (H.size() == 2) {
			num2++;
			return F*H[0][1];
		}
		if (H.size() == 3) {
			num3++;
			return F*(H[0][1] + H[1][2] * H[0][2] - H[0][1] * H[1][2] * H[0][2]);
		}
		if (H.size() == 4) {
			num4++;
			return F*(H[0][1] + ~H[0][1] * (H[1][2] * H[0][2] + H[0][3] * H[1][3] - H[1][2] * H[0][2] * H[0][3] * H[1][3] +
				H[1][2] * H[2][3] * H[0][3] * ~H[1][3] * ~H[0][2] + ~H[1][2] * H[2][3] * ~H[0][3] * H[1][3] * H[0][2]));
		}

		if (H.size() == 5) {
			num5++;
			return F*(~(~H[0][1] * ~H[0][2] * (H[1][2] * ~H[1][4] * (H[0][4] * ~H[0][3] * ~H[2][4] * (~H[3][4] + ~H[1][3] * ~H[2][3] * H[3][4]) +
				H[0][3] * ~H[1][3] * ~H[2][3] * (~H[2][4] + ~H[0][4] * ~H[3][4] * H[2][4])) + ~H[0][3] * ~H[0][4] * ~(~H[1][2] * ~H[1][3] * ~H[1][4])) +
				~H[0][1] * ~H[1][3] * (H[1][4] * ~H[0][4] * (H[0][3] * ~H[0][2] * ~H[3][4] * (~H[2][3] + ~H[1][2] * ~H[2][4] * H[2][3]) +
					H[0][2] * ~H[1][2] * ~H[2][4] * (~H[3][4] + ~H[0][3] * ~H[2][3] * H[3][4])) + ~H[1][2] * ~H[1][4]) +
				H[1][3] * ~H[0][1] * ~H[0][3] * ~H[1][2] * (H[0][2] * ~H[0][4] * ~H[2][3] * (~H[2][4] + ~H[1][4] * ~H[3][4] * H[2][4]) +
					H[0][4] * ~H[1][4] * ~H[3][4] * (~H[2][3] + ~H[0][2] * ~H[2][4] * H[2][3]))));
		}
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
		if (H.size() == 2) {
			num2++;
			return F*H[0][1];
		}
		if (H.size() == 3) {
			num3++;
			return F*(H[0][1] + H[1][2] * H[0][2] - H[0][1] * H[1][2] * H[0][2]);
		}
		if (H.size() == 4) {
			num4++;
			return F*(H[0][1] + ~H[0][1] * (H[1][2] * H[0][2] + H[0][3] * H[1][3] - H[1][2] * H[0][2] * H[0][3] * H[1][3] +
				H[1][2] * H[2][3] * H[0][3] * ~H[1][3] * ~H[0][2] + ~H[1][2] * H[2][3] * ~H[0][3] * H[1][3] * H[0][2]));
		}

		if (H.size() == 5) {
			num5++;
			return F*(~(~H[0][1] * ~H[0][2] * (H[1][2] * ~H[1][4] * (H[0][4] * ~H[0][3] * ~H[2][4] * (~H[3][4] + ~H[1][3] * ~H[2][3] * H[3][4]) +
				H[0][3] * ~H[1][3] * ~H[2][3] * (~H[2][4] + ~H[0][4] * ~H[3][4] * H[2][4])) + ~H[0][3] * ~H[0][4] * ~(~H[1][2] * ~H[1][3] * ~H[1][4])) +
				~H[0][1] * ~H[1][3] * (H[1][4] * ~H[0][4] * (H[0][3] * ~H[0][2] * ~H[3][4] * (~H[2][3] + ~H[1][2] * ~H[2][4] * H[2][3]) +
					H[0][2] * ~H[1][2] * ~H[2][4] * (~H[3][4] + ~H[0][3] * ~H[2][3] * H[3][4])) + ~H[1][2] * ~H[1][4]) +
				H[1][3] * ~H[0][1] * ~H[0][3] * ~H[1][2] * (H[0][2] * ~H[0][4] * ~H[2][3] * (~H[2][4] + ~H[1][4] * ~H[3][4] * H[2][4]) +
					H[0][4] * ~H[1][4] * ~H[3][4] * (~H[2][3] + ~H[0][2] * ~H[2][4] * H[2][3]))));
		}
	}

	if (count == 1) {
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
		for (int i = 0; i < q; i++) L = L*G[Ch[i]][Ch[i + 1]];
		for (int i = q; i < Ch.size() - 1; i++) P = P*G[Ch[i]][Ch[i + 1]];
		edge T = P*L;

		for (int i = 1; i < Ch.size() - 1; i++) { // after this we get 2 nodes from chain, one of them can be pivote
			delnode(G, Ch[i]);
			for (int j = 0; j < Ch.size(); j++) if (Ch[i] < Ch[j]) Ch[j]--; // not forget about Ch
		}
		int x = Ch.front(), y = Ch.back(); // matter x<=>y

		vector<vector<edge>> G1(G);
		bool connec = gconnected(G);
		vector<bool> visited1(visited);
		//if (!connec) cout << "Get unconnected in chr1" << endl;

		edge k;
		if (q != 0) {
			(terminal) ? renumerate(G, y, t) : renumerate(G, y, 1);  // s=0||1, becouse we don't delete pivote node, after delete: s - node out of chain always 0, so t=1

			int just_value, help_processor = 0;
			if (H.size() >= MAX_DIMENSIONAL) {
				MPI_Status status;
				MPI_Send(&just_value, 0, MPI_INT, HOST_PROCESSOR, I_NEED_HELP_TAG, MPI_COMM_WORLD); // ask for help
				MPI_Recv(&help_processor, 1, MPI_INT, HOST_PROCESSOR, I_NEED_HELP_TAG, MPI_COMM_WORLD, &status); // get answer
			}

			if (help_processor != 0) {
				str Gp;
				Gp.G = G;
				Gp.F.push_back(F*(P - T)); // can't be 0 or 1
				Gp.ex = connec;
				Gp.V = visited;
				int str_buf_size = Gp.F.size()*buf_size + sizeof(int) + sizeof(bool);
				if (!Gp.ex) str_buf_size += Gp.G.size() * sizeof(bool);
				for (int i = 1; i < Gp.G.size(); i++) str_buf_size += i*buf_size + sizeof(int);
				char *buf_str = Pack_data(Gp, str_buf_size, buf_size, vector_size);
				MPI_Send(buf_str, str_buf_size, MPI_PACKED, help_processor, GPINFO_TAG, MPI_COMM_WORLD);
				delete[] buf_str;
			}
			else k = procedure(G, F*(P - T), connec, buf_size, vector_size); // Rsy = k
		}

		edge w;
		if (q != Ch.size() - 1) {
			visited = visited1; // same size so all right
			G = G1;
			(terminal) ? renumerate(G, x, t) : renumerate(G, x, 1);

			int just_value, help_processor = 0;
			if (H.size() >= MAX_DIMENSIONAL) {
				MPI_Status status;
				MPI_Send(&just_value, 0, MPI_INT, HOST_PROCESSOR, I_NEED_HELP_TAG, MPI_COMM_WORLD); // ask for help
				MPI_Recv(&help_processor, 1, MPI_INT, HOST_PROCESSOR, I_NEED_HELP_TAG, MPI_COMM_WORLD, &status); // get answer
			}

			if (help_processor != 0) {
				str Gp;
				Gp.G = G;
				Gp.F.push_back(F*(L - T)); // can't be 0 or 1
				Gp.ex = connec;
				Gp.V = visited;
				int str_buf_size = Gp.F.size()*buf_size + sizeof(int) + sizeof(bool);
				if (!Gp.ex) str_buf_size += Gp.G.size() * sizeof(bool);
				for (int i = 1; i < Gp.G.size(); i++) str_buf_size += i*buf_size + sizeof(int);
				char *buf_str = Pack_data(Gp, str_buf_size, buf_size, vector_size);
				MPI_Send(buf_str, str_buf_size, MPI_PACKED, help_processor, GPINFO_TAG, MPI_COMM_WORLD);
				delete[] buf_str;
			}
			else w = procedure(G, F*(L - T), connec, buf_size, vector_size); // Rsx = w
		}

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
				//cout << "Decomposition chr1:" << component1_size << " & " << component2_size << endl;
				vector<vector<edge>> J(G1);

				if (!terminal) s = 0; // else s = s 
				if ((visited[s] && visited[x]) || (!visited[s] && !visited[x]))
					(terminal) ? renumerate(G, x, t) : renumerate(G, x, 1);
				else
					(terminal) ? renumerate(G, y, t) : renumerate(G, y, 1);

				m = procedure(J, F*T, connec, buf_size, vector_size); // dont *F twice
			}
			else m = procedure(G, F*T, connected, buf_size, vector_size);
		}
		else m = procedure(G, F*T, connected, buf_size, vector_size);

		return k + w + m;
	}
	if (count == 2) {
		ch2++;
		vector<int> Ch(Chain);
		vector<int>::iterator it, r;
		for (it = Ch.begin(); it < Ch.end(); ++it) if (*it == 0) r = it;
		int q = r - Ch.begin(); // place of pivote node s=0 into vector Ch 
		for (it = Ch.begin(); it < Ch.end(); ++it) if (*it == 1) r = it;
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
		for (int i = 0; i < q; i++) L = L*G[Ch[i]][Ch[i + 1]];
		for (int i = q; i < w; i++) P = P*G[Ch[i]][Ch[i + 1]];
		for (int i = w; i < Ch.size() - 1; i++) L = L*G[Ch[i]][Ch[i + 1]];
		for (int i = 1; i < Ch.size() - 1; i++) {
			delnode(G, Ch[i]);
			for (int j = 0; j < Ch.size(); j++) if (Ch[i] < Ch[j]) Ch[j]--; // not forget about Ch
		}
		int x = Ch.front(), y = Ch.back(); // doesn't matter x<=>y

		bool connec = gconnected(G);
		//if (!connec) cout << "Get unconnected in chr2" << endl;

		if (x != 0 && x != 1 && y != 0 && y != 1) {
			renumerate(G, x, 0);
			renumerate(G, y, 1);
		}
		if (x == 0 && y != 1) renumerate(G, y, 1);
		if (x == 1 && y != 0) renumerate(G, y, 0);
		if (y == 0 && x != 1) renumerate(G, x, 1);
		if (y == 1 && x != 0) renumerate(G, x, 0);

		edge n = F*P;
		n = n + procedure(G, F*(L - P*L), connec, buf_size, vector_size); // Rxy = n

		return n;
	}

	edge W = fedge(H);
	if ((W.node1 == 1 && W.node2 == 0) || (W.node1 == 0 && W.node2 == 1)) cout << "Eror in Allowing edge" << endl;
	if (W.node1 == 1 || W.node1 == 0) cout << "Eror in Allowing edge" << endl;

	vector<vector<edge>> H1(H.size());
	vector<vector<edge>> H2(H.size());
	for (int i = 0; i < H.size(); i++) { // Copy H
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
	contractedge(H1, W.node1, W.node2);

	bool connec = gconnected(H2);
	vector<bool> visited1(visited);
	//if (!connec) cout << "Get unconnected graph into the factoring" << endl;

	if (!connec) {
		visited = visited1;
		int component1_size = 0;
		for (int i = 0; i < visited.size(); i++) if (visited[i]) component1_size++;
		int component2_size = visited.size() - component1_size;

		if (component1_size != 1 && component2_size != 1) {
			//cout << "Decomposition :" << component1_size << " & " << component2_size << endl;
			vector<vector<edge>> J1(H2);
			vector<vector<edge>> J2(H2);
			edge F3 = Bin.front();

			edge m = procedure(H2, F2, connec, buf_size, vector_size); // use visited

			if (W.node2 != 0 && W.node2 != 1 && ((visited[0] && !visited[1]) || (!visited[0] && visited[1]))) {
				//cout << "case 1" << endl;
				visited = visited1;
				if ((visited[W.node1] && visited[0]) || (!visited[W.node1] && !visited[0])) renumerate(J1, 1, W.node1);
				else renumerate(J1, 1, W.node2);
				edge k = procedure(J1, F3, connec, buf_size, vector_size);

				visited = visited1;
				if ((visited[W.node1] && visited[1]) || (!visited[W.node1] && !visited[1])) renumerate(J2, 0, W.node1);
				else renumerate(J2, 0, W.node2);
				edge w = procedure(J2, F3, connec, buf_size, vector_size);

				return F1*k*w + m; // dont *F1 twice
			}
			if (W.node2 != 0 && W.node2 != 1 && ((visited[0] && visited[1]) || (!visited[0] && !visited[1]))) {
				//cout << "case 2" << endl;
				visited = visited1;
				edge k = procedure(J1, F1, connec, buf_size, vector_size);

				return k + m;
			}
			if (W.node2 == 0 || W.node2 == 1) { // if 0 and 1 in one component nothing renum
				//cout << "case 3" << endl;
				visited = visited1;
				if (W.node2 == 0 && ((visited[0] && !visited[1]) || (!visited[0] && visited[1]))) renumerate(J1, 0, W.node1); // becouse can't return 0
				if (W.node2 == 1 && ((visited[0] && !visited[1]) || (!visited[0] && visited[1]))) renumerate(J1, 1, W.node1);
				edge k = procedure(J1, F1, connec, buf_size, vector_size);

				return k + m;
			}
		}
		else return procedure(H1, F1, connected, buf_size, vector_size) + procedure(H2, F2, connec, buf_size, vector_size);
	}
	else {
		int just_value, help_processor = 0;
		if (H.size() >= MAX_DIMENSIONAL) {
			MPI_Status status;
			MPI_Send(&just_value, 0, MPI_INT, HOST_PROCESSOR, I_NEED_HELP_TAG, MPI_COMM_WORLD); // ask for help
			MPI_Recv(&help_processor, 1, MPI_INT, HOST_PROCESSOR, I_NEED_HELP_TAG, MPI_COMM_WORLD, &status); // get answer
		}

		if (help_processor != 0) { // send half work
			str Gp;
			Gp.G = H1;
			Gp.F.push_back(F1); // can't be 0 or 1
			Gp.ex = connected;
			Gp.V = visited;
			int str_buf_size = Gp.F.size()*buf_size + sizeof(int) + sizeof(bool);
			if (!Gp.ex) str_buf_size += Gp.G.size() * sizeof(bool);
			for (int i = 1; i < Gp.G.size(); i++) str_buf_size += i*buf_size + sizeof(int);
			char *buf_str = Pack_data(Gp, str_buf_size, buf_size, vector_size);
			MPI_Send(buf_str, str_buf_size, MPI_PACKED, help_processor, GPINFO_TAG, MPI_COMM_WORLD);
			delete[] buf_str;
		}
		else return procedure(H1, F1, connected, buf_size, vector_size) + procedure(H2, F2, connec, buf_size, vector_size);

		return procedure(H2, F2, connec, buf_size, vector_size); // continue with graph wehere deleted
	}
}