#include "stdafx.h"
#include "funcs.h"
#include "globals.h"
#include "Cclass.h"


edge chainreduction2(vector<vector<edge>> &H, edge sum, vector<int> weight, edge F, bool connected)
{ 
	vector<int> V; // considered nodes
	for (int i=0; i<mask.size(); i++) if (!mask[i]) V.push_back(i);
    vector<int> C = fchain(H, V);

	if (!C.empty()) {
		for (int i=0; i<C.size(); i++) if (!exis(V, C[i])) mask[C[i]] = false;

	    if (!exis(C, 0)) {
		    edge T;
	        T.C.push_back(1);
		    vector<int> Ch = C;
	        vector<vector<edge>> G(H.size());
	        for (int i=0; i<H.size(); i++) G[i] = H[i];
	        for (int i=0; i<Ch.size() - 1; i++) T = T*G[Ch[i]][Ch[i + 1]];

	        edge F1, F2;
			int F3 = 0;
			for (int i=0; i<Ch.size() - 1; i++) if (!exis(V, Ch[i])) { // i=Ch.size() - 1 doesn't make anything
			     edge L; // save power,simple, but lost P.C
		         L.C.push_back(1);
		         for (int j=0; j<i; j++) L = L*G[Ch[j]][Ch[j + 1]];
				 F1 = F1 + weight[Ch[i]]*(L - T);
	        }
			for (int i=1; i<Ch.size(); i++) if (!exis(V, Ch[i])) { // i=0 doesn't make anything
			     edge P;
			     P.C.push_back(1);
		         for (int j=i; j<Ch.size() - 1; j++) P = P*G[Ch[j]][Ch[j + 1]]; // ?
		         F2 = F2 + weight[Ch[i]]*(P - T);
	        }

			for (int i=0; i<Ch.size(); i++) if (!exis(V, Ch[i])) F3 += weight[Ch[i]];
        
	        for (int i=1; i<Ch.size() - 1; i++) { // after this we get 2 nodes from chain, one of them can be pivote
		         delnode(G, Ch[i]); 
				 for (int j=0; j<Ch.size(); j++) if (Ch[i] < Ch[j]) Ch[j]--; // not forget about Ch
	        }
	        vector<vector<edge>> G1(G.size());
	        for (int i=0; i<G.size(); i++) G1[i] = G[i]; // after reduction G changed

	        int x = Ch.front(), y = Ch.back(); // matter x<=>y
            renumerate(G, y, 1);  // s - node out of chain always 0, so t=1
	        edge k = procedure(G, F, connected); // R1y
	        renumerate(G1, x, 1);   
	        edge w = procedure(G1, F, connected);// R1x
        
	        Ch = C;
	        G.resize(H.size());
	        for (int i=0; i<H.size(); i++) G[i] = H[i];

	        for (int i=0; i<Ch.size() - 1; i++) { 
	             contractedge(G, Ch[i], Ch[i + 1]);
				 for (int j=0; j<Ch.size(); j++) { // usually delete Ch[i], but not always
			          int w = Ch[i];
			          if (Ch[i] == 1) w = Ch[i + 1]; // no 0 in Ch
			          if (w < Ch[j]) Ch[j]--; // not forget about Ch
	             }
		         if (Ch[i] == 1) {
		             Ch.erase(Ch.begin() + i + 1);
		             i--;
                 }
				 else { // need to get <x,y> 
					 Ch.erase(Ch.begin() + i); 
					 i--;
				 }
            }

		    int xy = Ch.front();
			if (xy != 1) renumerate(G, xy, 1); // 1 can not be in Ch
	        edge m = procedure(G, F, connected);
            
	        sum = sum + (F2*k + F1*w + F3*T*m); 

	        return chainreduction2(H, sum, weight, F, connected);
	    }
	    else {
		    vector<int> Ch = C;
		    vector<int>::iterator it, r;
	        for (it=Ch.begin(); it<Ch.end(); ++it) if (*it == 0) r = it; 
		    int q = r - Ch.begin(); // place of pivote node s=0 into vector Ch 

		    edge T;
	        T.C.push_back(1);
		    vector<vector<edge>> G(H.size());
		    for (int i=0; i<H.size(); i++) G[i] = H[i];
	        for (int i=0; i<Ch.size() - 1; i++) T = T*G[Ch[i]][Ch[i + 1]];
               
	        edge F1, F2;
			int F3 = 0;
			for (int i=0; i<Ch.size(); i++) if (i != q && !exis(V, Ch[i])) {
			     edge P; // save power,simple, but lost P.C
				 P.C.push_back(1);
			     if (i < q) for (int j=i; j<q; j++) P = P*G[Ch[j]][Ch[j + 1]];
			     else for (int j=q; j<i; j++) P = P*G[Ch[j]][Ch[j + 1]];           
			     F1 = F1 + weight[Ch[i]]*P;
		    }
			for (int i=0; i<Ch.size(); i++) if (i != q && !exis(V, Ch[i])) {
			     edge L;
			     L.C.push_back(1);
			     if (i < q) for (int j=0; j<i; j++) L = L*G[Ch[j]][Ch[j + 1]];
			     else for (int j=0; j<q; j++) L = L*G[Ch[j]][Ch[j + 1]];
			     if (i < q) for (int j=q; j<Ch.size() - 1; j++) L = L*G[Ch[j]][Ch[j + 1]];
			     else for (int j=i; j<Ch.size() - 1; j++) L = L*G[Ch[j]][Ch[j + 1]];
			     F2 = F2 + weight[Ch[i]]*L;
		    }

			for (int i=0; i<Ch.size(); i++) if (!exis(V, Ch[i])) F3 += weight[Ch[i]];

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

		    edge n = procedure(G, F, connected); // Rxy
            
		    sum = sum + (F1 + F2*n - F3*T*n); 

	        return chainreduction2(H, sum, weight, F, connected);
	    }
	}
	return sum;
}