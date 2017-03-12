#include "stdafx.h"
#include "funcs.h"
#include "globals.h"
#include "Cclass.h"


edge chainreduction1(vector<vector<edge>> &H, edge sum, vector<int> V, edge F, bool connected)
{ 
    vector<int> C = fchain(H, V);

	for (int i=0; i<C.size(); i++) if (!exis(V, C[i])) V.push_back(C[i]);

	if (!C.empty()) {
		vector<int>::iterator largest = max_element(C.begin(), C.end());

		for (int i=0; i<*largest; i++) if (!exis(C, i)) {
		    edge T;
	        T.C.push_back(1);
		    vector<int> Ch = C;
	        vector<vector<edge>> G(H.size());
	        for (int j=0; j<H.size(); j++) G[j] = H[j];
	        for (int j=0; j<Ch.size() - 1; j++) T = T*G[Ch[j]][Ch[j + 1]];

	        edge F1, F2;
			int F3 = 0;

			for (int j=0; j<Ch.size() - 1; j++) if (Ch[j] > i && mask1[i][Ch[j]]) { // j=Ch.size() - 1 doesn't make anything
			     edge L; // save power,simple, but lost P.C
		         L.C.push_back(1);
		         for (int k=0; k<j; k++) L = L*G[Ch[k]][Ch[k + 1]];
				 F1 = F1 +(L - T);
	        }
			for (int j=1; j<Ch.size(); j++) if (Ch[j] > i && mask1[i][Ch[j]]) { // j=0 doesn't make anything
			     edge P;
			     P.C.push_back(1);
		         for (int k=j; k<Ch.size() - 1; k++) P = P*G[Ch[k]][Ch[k + 1]];
		         F2 = F2 + (P - T);
	        }

			for (int j=0; j<Ch.size(); j++) if (Ch[j] > i && mask1[i][Ch[j]]) F3++;

			for (int j=0; j<Ch.size(); j++) if (Ch[j] > i && mask1[i][Ch[j]]) {
				mask1[i][Ch[j]] = false;
				mask1[Ch[j]][i] = false;
			}
            
			int s = i;
	        for (int j=1; j<Ch.size() - 1; j++) { // after this we get 2 nodes from chain, one of them can be pivote
		         delnode(G, Ch[j]); 
				 if (Ch[j] < s) s--; // not forget about s
				 for (int k=0; k<Ch.size(); k++) if (Ch[j] < Ch[k]) Ch[k]--; // not forget about Ch
	        }
	        vector<vector<edge>> G1(G.size());
	        for (int j=0; j<G.size(); j++) G1[j] = G[j]; // after reduction G changed

	        int x = Ch.front(), y = Ch.back(); // matter x<=>y
			if (s != 0 && s != 1 && y != 0 && y != 1) {
			    renumerate(G, s, 0); 
		        renumerate(G, y, 1); 
		    }
			if (s == 0 && y != 1) renumerate(G, y, 1); 
			if (s == 1 && y != 0) renumerate(G, y, 0);
			if (y == 0 && s != 1) renumerate(G, s, 1);
			if (y == 1 && s != 0) renumerate(G, s, 0);
	        edge k = procedure(G, F, connected); // Rsy

			if (s != 0 && s != 1 && x != 0 && x != 1) {
			    renumerate(G1, s, 0); 
		        renumerate(G1, x, 1); 
		    }
			if (s == 0 && x != 1) renumerate(G1, x, 1); 
			if (s == 1 && x != 0) renumerate(G1, x, 0);
			if (x == 0 && s != 1) renumerate(G1, s, 1);
			if (x == 1 && s != 0) renumerate(G1, s, 0);
	        edge w = procedure(G1, F, connected);// Rsx
        
	        Ch = C;
	        G.resize(H.size());
	        for (int j=0; j<H.size(); j++) G[j] = H[j];

			s = i;
	        for (int j=0; j<Ch.size() - 1; j++) { // can contract 0,1 but it is not problem
	             contractedge(G, Ch[j], Ch[j + 1]);
				 int w = Ch[j];
			     if (Ch[j] == 1 || Ch[j] == 0) w = Ch[j + 1]; 
				 if (w < s) s--; // not forget about s
				 for (int k=0; k<Ch.size(); k++) if (w < Ch[k]) Ch[k]--; // not forget about Ch
				      // usually delete Ch[i], but not always 
				 if (Ch[j] == 1 || Ch[j] == 0) {
		             Ch.erase(Ch.begin() + j + 1);
		             j--;
                 }
				 else { // need to get <x,y> 
					 Ch.erase(Ch.begin() + j); 
					 j--;
				 }
            }

		    int xy = Ch.front();
			if (s != 0 && s != 1 && xy != 0 && xy != 1) {
			    renumerate(G, s, 0); 
		        renumerate(G, xy, 1); 
		    }
			if (s == 0 && xy != 1) renumerate(G, xy, 1); 
			if (s == 1 && xy != 0) renumerate(G, xy, 0);
			if (xy == 0 && s != 1) renumerate(G, s, 1);
			if (xy == 1 && s != 0) renumerate(G, s, 0);
	        edge m = procedure(G, F, connected); // Rs,xy
           
	        sum = sum + (F2*k + F1*w + F3*T*m); 
	    }
	    else {
		    vector<int> Ch = C;
		    vector<int>::iterator it, r;
	        for (it=Ch.begin(); it<Ch.end(); ++it) if (*it == i) r = it; 
		    int q = r - Ch.begin(); // place of pivote node s=i into vector Ch 

		    edge T;
	        T.C.push_back(1);
		    vector<vector<edge>> G(H.size());
		    for (int j=0; j<H.size(); j++) G[j] = H[j];
	        for (int j=0; j<Ch.size() - 1; j++) T = T*G[Ch[j]][Ch[j + 1]];
               
	        edge F1, F2;
			int F3 = 0;
			for (int j=0; j<Ch.size(); j++) if (Ch[j] > i && mask1[i][Ch[j]]) {
			     edge P; // save power,simple, but lost P.C
				 P.C.push_back(1);
				 if (j < q) for (int k=j; k<q; k++) P = P*G[Ch[k]][Ch[k + 1]];
			     else for (int k=q; k<j; k++) P = P*G[Ch[k]][Ch[k + 1]];            
			     F1 = F1 + P;
		    }
			for (int j=0; j<Ch.size(); j++) if (Ch[j] > i && mask1[i][Ch[j]]) {
			     edge L;
			     L.C.push_back(1);
				 if (j < q) {
					 for (int k=0; k<j; k++) L = L*G[Ch[k]][Ch[k + 1]];
					 for (int k=q; k<Ch.size() - 1; k++) L = L*G[Ch[k]][Ch[k + 1]];
				 }
				 else {
					 for (int k=0; k<q; k++) L = L*G[Ch[k]][Ch[k + 1]];
					 for (int k=j; k<Ch.size() - 1; k++) L = L*G[Ch[k]][Ch[k + 1]];
				 }
			     F2 = F2 + L;
		    }

			for (int j=0; j<Ch.size(); j++) if (Ch[j] > i && mask1[i][Ch[j]]) F3++;

			for (int j=0; j<Ch.size(); j++) if (Ch[j] > i && mask1[i][Ch[j]]) {
				mask1[i][Ch[j]] = false;
				mask1[Ch[j]][i] = false;
			}

		    for (int j=1; j<Ch.size() - 1; j++) {
		         delnode(G, Ch[j]); 
				 for (int k=0; k<Ch.size(); k++) if (Ch[j] < Ch[k]) Ch[k]--; // not forget about Ch
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

		    edge n = procedure(G, F, connected); // Rxy
            
		    sum = sum + (F1 + F2*n - F3*T*n); 
		}
		return chainreduction1(H, sum, V, F, connected);
	}
	return sum;
}
