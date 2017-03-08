#include "stdafx.h"
#include "funcs.h"
#include "globals.h"
#include "Cclass.h"

vector<bool> visited;
int num = 0, num2 = 0, num3 = 0, num4 = 0, num5 = 0;

vector<int> gnodepower( vector<vector<edge>> &H)
{   vector <int> nodepower(H.size());
	int i = 0; 
    while( i < H.size() ) {
		for (int j=0; j<H[i].size(); j++) if ( H[i][j].ex ) nodepower[i]++;
		i++;
	}
	return nodepower;
}

void DFS(vector<vector<edge>> &H, int q)
{
     visited[q] = true;
	 for (int i=0; i<H.size(); i++) for (int j=i+1; j<H[i].size(); j++) if ( H[i][j].ex && ( !visited[i] || !visited[j] ) ) {
		 if (!visited[i]) DFS(H, i);
		 else DFS(H, j);
	 }
}

bool gconnected (vector<vector<edge>> &H)
{
	 int count = 0;
	 visited.resize(H.size());
	 for (int i=0; i<visited.size(); i++) visited[i] = false;

     DFS(H, 0);
	 for (int i=0; i<visited.size(); i++) if (visited[i]) count++;

	 if (count!=H.size()) return false;
     else return true;
}

void delnode ( vector<vector<edge>> &H,int q)
{
	H.erase(H.begin() + q);
	for (int i=0; i<H.size(); i++) H[i].erase(H[i].begin() + q);
}

void deledge (vector<vector<edge>> &H, int q, int w) 
{  
	edge F;
	F.ex = false;
    H[q][w] = F;
    H[w][q] = F;
}

void contractedge ( vector<vector<edge>> &H,int q, int w)
{
	if (q==0 || q==1) { // that do not delete pivote node, this case arise in chainreduction
		int r = w;
		w = q;
		q = r;
	}

    for (int i=0; i<H[q].size(); i++) if (H[q][i].ex && i!=w) { // do not consider H[q][w]
		if (H[w][i].ex && H[q][i].power == 1 && H[w][i].power == 1) { //  power==1 is falg of simple edge
		    H[w][i].simple += H[q][i].simple + 1;
		    H[i][w].simple += H[q][i].simple + 1;
		}
		if (H[w][i].ex && (H[q][i].power != 1 || H[w][i].power != 1)) { // arise in chainreduction
			H[w][i] = H[q][i] + H[w][i] - H[q][i]*H[w][i];
			H[i][w] = H[w][i]; // for symmetry and less computing
		}
		if (!H[w][i].ex)  {
			H[w][i] = H[q][i]; // so easy, no need to compute
		    H[i][w] = H[q][i];
		}
	} 
	delnode (H, q);
}

void renumerate (vector<vector<edge>> &H, int s, int t) 
{
	if (s!=t) {
		vector<edge> F, G;
		F = H[s];
		G = H[t];
		for (int i=0; i<H.size(); i++) if (i!=s && i!=t) swap(F[i], G[i]);
		
		for (int i=0; i<H.size(); i++) { //clear strings and columns for nodes s,t
			if (H[t][i].ex) deledge(H, t, i); //take into account the symmetry
            if (H[s][i].ex) deledge(H, s, i);
		}
	    
		for (int i=0; i<H.size(); i++) { // filling
	         if ( F[i].ex && i!=s) {
			     H[s][i] = F[i];
				 if (i!=t)  H[i][s] = F[i]; // if do not fill another string 
			 }
			 if ( G[i].ex && i!=t) {
			     H[t][i] = G[i];
				 if (i!=s) H[i][t] = G[i];
			 }
	    }
	}
}

edge penduntreduction( vector<vector<edge>> &H, edge F , int q, bool w)
{
	vector<int> nodepower = gnodepower(H);
	if (w) nodepower.erase(nodepower.begin() + q); // in one penduntreduction we not consider pendunt node 0 || 1
    
	vector <int>::iterator it, smallest = nodepower.end() - 1;
    for(it=nodepower.begin();it<nodepower.end();++it) if ( *it == 1 ) smallest = it; 
    int r = smallest - nodepower.begin(); // find pendunt node, can be 0,1

	if ( *smallest==1 && H.size()>2 ) { //otherwise all will be deleted
       for (int i = H[r].size() - 1; i >= 0; i--) 
		   if (H[r][i].ex) { //find incident edge to pendunt node, can't be 0,1 || 1,0
		       
	           if (r!=0 && r!=1) delnode(H,r);
			   if ( (r == 1 && i !=0) || (r == 0 && i != 1) ) {
					F = F*H[r][i];
				    delnode(H,r);
				    i--; // after delenode not forget about i
			        if (i != 1) renumerate (H, i, 1); // always must be 1
	           }
			   if ( (r == 1 && i == 0) || (r == 0 && i == 1) ) { // that do not have loop
				   q = r;
				   w = true;
			   }
		   break;// arise the eror if abort this
	       }
	return penduntreduction(H, F, q, w);
	}
	return F;

}
void bridgereduction( vector<vector<edge>> &H)
{
	cout<<"Bridge reduction!"<<endl;
	if(visited[0] && visited[1]) for (int i=0; i<H.size(); i++) 
		if (!visited[i]) {
			cout<<"Deleted unconnected node:"<<i<<endl; 
			delnode(H,i); 
		}
	if(!visited[0] && !visited[1]) for (int i=0; i<H.size(); i++) 
		if (visited[i]) {
			cout<<"Deleted unconnected node:"<<i<<endl; 
			delnode(H,i); 
		}
}

edge fedge(vector<vector<edge>> &H)
{
	edge F;
	vector<int> nodepower = gnodepower(H);
	vector<int>::iterator it, smallest = nodepower.end() - 1;
    
	for (it=nodepower.begin();it<nodepower.end();++it) 
		if (*it <= *smallest && (it - nodepower.begin() != 0) && (it - nodepower.begin() != 1) ) smallest = it; 
	       //becouse after deledge we can get unconnected graph in case nodepower=1
    int r = smallest - nodepower.begin(); //can't be 0,1
	
	for (int i = H[r].size() - 1; i >= 0; i--) if ( H[r][i].ex ) {  //find incident edge selected node, can be 0,1
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
	else for (int i=0; i<P.size(); i++) if (x == P[i]) return true;
	return false;
}

vector<int> fchain(vector<vector<edge>> &H, vector<int> &V) 
{
	vector<int> nodepower = gnodepower(H);
	vector<int> P; // chain nodes vector
    
	bool find = false;
	int i = 0;
	while (!find && i<H.size()) {
	       for (int j=0; j<H[i].size(); j++) { 
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
			   for (int j=0; j<H[i].size(); j++) if (H[i][j].ex && j != P[1]) {
					if (j != P.back()) P.insert(P.begin(), j);
			        i = j;
					break; 
			   }
        i = P.back();
		while (nodepower[i] == 2 && i != P.front()) 
			   for (int j=0; j<H[i].size(); j++) if (H[i][j].ex && j != P[P.size() - 2]) {
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
		for (int i=0; i<P.size(); i++) if (P[i] == 0 || P[i] == 1) count++; // count selected nodes in chain
        
		vector<int>::iterator it, r = P.begin();
		if (count == 1) {
	        for (it=P.begin(); it<P.end(); ++it) if (*it == 0 || *it == 1 ) r = it; 
		    if ((r == P.begin() || r == P.end()) && !Ch.empty()) count = 0; 
		}

		if ((count == 1 || count == 2) && !found) {
			if (count == 2) { // have only chain with count=2
				found = true;
				V = P;
			}
			if (count == 1) {
				if (!Ch.empty()) {
				    for (it=Ch.begin(); it<Ch.end() - 1; ++it) if (*it == 0 || *it == 1 ) r = it;  // Ch.back()=count
				    if (r == Ch.begin() || r == Ch.end() - 2) V.clear(); // next recursive call reduct Ch
				}
				for (int j=0; j<P.size(); j++) if (!exis(V, P[j])) V.push_back(P[j]);
			}

			P.push_back(count);
			Ch = P;

			return chainreduction(H, Ch, V, found);
		}
		else count = 0; // to reduct chain(s) when we find count=2

		if (count == 0) { // Replace chain by edge
			edge newedge;
	        newedge.C.push_back(1);
		    for (int j=0; j<P.size() - 1; j++) newedge = newedge*H[P[j]][P[j + 1]];
			for (int j=1; j<P.size() - 1; j++) {
				 delnode(H, P[j]); // delete only inner nodes of chain
				 for (int k=0; k<P.size(); k++) if (P[j] < P[k] ) P[k]--; // not forget about P
				 for (int k=0; k<Ch.size(); k++) if (P[j] < Ch[k] ) Ch[k]--; // not forget about Ch
				 for (int k=0; k<V.size(); k++) if (P[j] < V[k] ) V[k]--; // not forget about V
			}
			int x = P.front(), y = P.back();

		    if (H[x][y].ex) { // Becouse newedge.power can't be 1
			H[x][y] = H[x][y] + newedge - H[x][y]*newedge;
			H[y][x] = H[x][y]; 
		    }
			else {
			H[x][y] = newedge; 
		    H[y][x] = newedge;
		    }
            
	        return chainreduction(H, Ch, V, found);
		}
	}
	return Ch;
}

edge procedure(vector<vector<edge>> &H, edge F, bool connected) 
{

	if (H.size() < 5) {
		num++;
	    
		if (H.size() == 2) {
			num2++;
			return F*H[0][1];
		}
		if (H.size() == 3) {
			num3++;
			return F*( H[0][1] + H[1][2]*H[0][2] - H[0][1]*H[1][2]*H[0][2] );
		}
		if (H.size() == 4) { 
			num4++;
			return F*( H[0][1] + ~H[0][1]* ( H[1][2]*H[0][2] + H[0][3]*H[1][3] - H[1][2]*H[0][2]*H[0][3]*H[1][3] +
			       H[1][2]*H[2][3]*H[0][3]*~H[1][3]*~H[0][2] + ~H[1][2]*H[2][3]*~H[0][3]*H[1][3]*H[0][2] ) );
		}
		
		if (H.size() == 5) {
			num5++;
            return F*( ~( H[0][1]*H[0][2]* ( ~H[1][2]*H[1][4]*( ~H[0][4]*H[0][3]*H[2][4]*(H[3][4] + H[1][3]*H[2][3]*~H[3][4]) +
		           ~H[0][3]*H[1][3]*H[2][3]*(H[2][4] + H[0][4]*H[3][4]*~H[2][4])) + H[0][3]*H[0][4]*~(H[1][2]*H[1][3]*H[1][4]) ) +
		           H[0][1]*H[1][3]* ( ~H[1][4]*H[0][4]*(~H[0][3]*H[0][2]*H[3][4]*(H[2][3] + H[1][2]*H[2][4]*~H[2][3]) +
		           ~H[0][2]*H[1][2]*H[2][4]*(H[3][4] + H[0][3]*H[2][3]*~H[3][4])) + H[1][2]*H[1][4] ) +
		           ~H[1][3]*H[0][1]*H[0][3]*H[1][2]* ( ~H[0][2]*H[0][4]*H[2][3]*(H[2][4] + H[1][4]*H[3][4]*~H[2][4]) +
		           ~H[0][4]*H[1][4]*H[3][4]*(H[2][3] + H[1][4]*H[2][4]*~H[2][3]) ) ) );
		}
	}

	if (!connected) {
		bridgereduction(H);
		connected = true;
	}

	F = penduntreduction(H, F, 0, false);

	vector<int> Chain, V;
    Chain = chainreduction(H, Chain, V, false);
	int count = 0;
	if (!Chain.empty()) {
	    count = Chain.back();
	    Chain.pop_back();
	}

	if (H.size() < 5) {
		num++;
	    
		if (H.size() == 2) {
			num2++;
			return F*H[0][1];
		}
		if (H.size() == 3) {
			num3++;
			return F*( H[0][1] + H[1][2]*H[0][2] - H[0][1]*H[1][2]*H[0][2] );
		}
		if (H.size() == 4) { 
			num4++;
			return F*( H[0][1] + ~H[0][1]* ( H[1][2]*H[0][2] + H[0][3]*H[1][3] - H[1][2]*H[0][2]*H[0][3]*H[1][3] +
			       H[1][2]*H[2][3]*H[0][3]*~H[1][3]*~H[0][2] + ~H[1][2]*H[2][3]*~H[0][3]*H[1][3]*H[0][2] ) );
		}
		
		if (H.size() == 5) {
			num5++;
            return F*( ~( H[0][1]*H[0][2]* ( ~H[1][2]*H[1][4]*( ~H[0][4]*H[0][3]*H[2][4]*(H[3][4] + H[1][3]*H[2][3]*~H[3][4]) +
		           ~H[0][3]*H[1][3]*H[2][3]*(H[2][4] + H[0][4]*H[3][4]*~H[2][4])) + H[0][3]*H[0][4]*~(H[1][2]*H[1][3]*H[1][4]) ) +
		           H[0][1]*H[1][3]* ( ~H[1][4]*H[0][4]*(~H[0][3]*H[0][2]*H[3][4]*(H[2][3] + H[1][2]*H[2][4]*~H[2][3]) +
		           ~H[0][2]*H[1][2]*H[2][4]*(H[3][4] + H[0][3]*H[2][3]*~H[3][4])) + H[1][2]*H[1][4] ) +
		           ~H[1][3]*H[0][1]*H[0][3]*H[1][2]* ( ~H[0][2]*H[0][4]*H[2][3]*(H[2][4] + H[1][4]*H[3][4]*~H[2][4]) +
		           ~H[0][4]*H[1][4]*H[3][4]*(H[2][3] + H[1][4]*H[2][4]*~H[2][3]) ) ) );
		}
	}

	if (count == 1) {
		vector<int> Ch = Chain;
		vector<int>::iterator it, r;
	    for (it=Ch.begin(); it<Ch.end(); ++it) if (*it == 0 || *it == 1 ) r = it; 
		int q = r - Ch.begin(); // place of pivote node t=(0 or 1) into vector Ch 

		edge P, L;
		P.C.push_back(1);
		L.C.push_back(1);
		vector<vector<edge>> G(H.size());
		for (int i=0; i<H.size(); i++) G[i] = H[i];
		for (int i=0; i<q; i++) L = L*G[Ch[i]][Ch[i + 1]];
		for (int i=q; i<Ch.size() - 1; i++) P = P*G[Ch[i]][Ch[i + 1]];
        
		for (int i=1; i<Ch.size() - 1; i++) { // after this we get 2 nodes from chain, one of them can be pivote
		     delnode(G, Ch[i]); 
			 for (int j=0; j<Ch.size(); j++) if (Ch[i] < Ch[j]) Ch[j]--; // not forget about Ch
	    }
		vector<vector<edge>> G1(G.size());
		for (int i=0; i<G.size(); i++) G1[i] = G[i]; // after reduction G changed
		int x = Ch.front(), y = Ch.back(); // matter x<=>y
		if (r == Ch.begin()) renumerate(G, y, *r); // s=0||1 
		if (r != Ch.begin() && r != Ch.end() - 1) renumerate(G, y, 1);  // after delete: s - node out of chain always 0, so t=1
		edge k = procedure(G, F, connected); // Rsy
		if (r == Ch.end() - 1) renumerate(G1, x, *r); // s=0||1 
		if (r != Ch.begin() && r != Ch.end() - 1) renumerate(G1, x, 1);   
		edge w = procedure(G1, F, connected); // Rsx
        
		Ch = Chain;
		G.resize(H.size());
		for (int i=0; i<H.size(); i++) G[i] = H[i];
		for (int i=0; i<Ch.size() - 1; i++) { // after we get 1 pivote node from chain
			contractedge(G, Ch[i], Ch[i + 1]);
			for (int j=0; j<Ch.size(); j++) { // usually delete Ch[i], but not always
				int w = Ch[i];
				if (Ch[i] == 0 || Ch[i] == 1) w = Ch[i + 1];
				if (w < Ch[j]) Ch[j]--; // not forget about Ch
			}
			if (Ch[i] == 0 || Ch[i] == 1) {
					Ch.erase(Ch.begin() + i + 1);
					i--;
			}
		}
		edge m = procedure(G, F, connected); // Rs<x,y>
		// no renumerate becouse s,t always pivote nodes
        
		return P*k + L*w - P*L*(k + w - m);
	}
	if (count == 2) {
		vector<int> Ch = Chain;
		vector<int>::iterator it, r;
	    for (it=Ch.begin(); it<Ch.end(); ++it) if (*it == 0) r = it; 
		int q = r - Ch.begin(); // place of pivote node s=0 into vector Ch 
		for (it=Ch.begin(); it<Ch.end(); ++it) if (*it == 1 ) r = it; 
		int w = r - Ch.begin(); // place of pivote node t=1 into vector Ch 
		if (q > w) { // that * edges in chain, q<w always
			int u = q;
			q = w;
			w = u;
		}

		edge P, L;
		P.C.push_back(1);
		L.C.push_back(1);
		vector<vector<edge>> G(H.size());
		for (int i=0; i<H.size(); i++) G[i] = H[i];
		for (int i=0; i<q; i++) L = L*G[Ch[i]][Ch[i + 1]];
		for (int i=q; i<w; i++) P = P*G[Ch[i]][Ch[i + 1]];
		for (int i=w; i<Ch.size() - 1; i++) L = L*G[Ch[i]][Ch[i + 1]];
		for (int i=1; i<Ch.size() - 1; i++) {
		     delnode(G, Ch[i]); 
			 for (int j=0; j<Ch.size(); j++) if (Ch[i] < Ch[j]) Ch[j]--; // not forget about Ch
	    }
		int x = Ch.front(), y = Ch.back(); // doesn't matter x<=>y
		if (x != 0 && x != 1 && y != 0 && y != 1) {
			renumerate(G, x, 0); 
		    renumerate(G, y, 1); 
		}
		if (x == 0 && y != 1) renumerate(G, y, 1); 
		if (x == 1 && y != 0) renumerate(G, y, 0); 
		if (y == 0 && x != 1) renumerate(G, x, 1); 
		if (y == 1 && x != 0) renumerate(G, x, 0); 
		edge n = procedure(G, F, connected);

		return F*P + L*n - P*L*n; // F* becouse we don't make recursive call
	}

    edge W = fedge(H);
	if (W.node1 == 0 || W.node1 == 1) cout<<"Eror in fedge"<<endl;
    if ((W.node1==1 && W.node2==0) || (W.node1==0 && W.node2==1)) cout<<"Eror in Allowing edge"<<endl;
		 
    vector<vector<edge>> H1(H.size());
	vector<vector<edge>> H2(H.size());
    for (int i=0; i<H.size(); i++)  { // Copy H
		 H1[i] = H[i];
	     H2[i] = H[i];
	}
	edge F1, F2;
    if (W.simple == 0 && W.power == 1) {
		if (F.C.size() == 1) {
			F.C.resize(H.size());
			F.power = 0;
			F.simple = 0;
		}
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
       
    contractedge (H1, W.node1, W.node2);
    deledge (H2, W.node1, W.node2);
 
    bool r = gconnected(H2);
    if (!r) cout<<"Get unconnected graph into the recursion"<<endl;

	return  procedure ( H1, F1, connected ) + procedure ( H2, F2, r ) ;
}