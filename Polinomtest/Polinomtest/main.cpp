#include "stdafx.h"
#include "funcs.h"
#include "globals.h"
#include "Cclass.h"

ofstream output("C://output.txt");
vector<vector<bool>> mask1;
vector<bool> mask;

void table(vector<vector<edge>> &H, vector<edge> &E) 
{
    for (int i=0; i<H.size(); i++) H[i].resize(H.size()); 
	     //initialize all strings, so as array is associative, not fill by nulls, or it is not global
    for (int i=0; i<H.size(); i++) for (int j=0; j<H[i].size(); j++) {
		 H[i][j].ex = false;
		 for (int w=0; w<E.size(); w++) 
		      if ((i==E[w].node1 && j==E[w].node2) || (i==E[w].node2 && j==E[w].node1)) H[i][j] = E[w];
	}
}

edge chainreduction1(vector<vector<edge>> &H, edge sum, vector<int> V, edge F, bool connected)
{ 
	output<<"chainreduction1:"<<endl;

	output<<"H:"<<endl;
    for (int i=0; i<H.size(); i++) { for (int j=0; j<H[i].size(); j++) output<<H[i][j].ex<<" "; output<<endl;}

	output<<"V before:"<<endl;
	copy( V.begin(), V.end(), ostream_iterator<int>(output," ") );
	output<<endl;

    vector<int> C = fchain(H, V);

	for (int i=0; i<C.size(); i++) if (!exis(V, C[i])) V.push_back(C[i]);

	output<<"Chain:"<<endl;
	copy( C.begin(), C.end(), ostream_iterator<int>(output," ") );
	output<<endl;

	output<<"V after:"<<endl;
	copy( V.begin(), V.end(), ostream_iterator<int>(output," ") );
	output<<endl;

	if (!C.empty()) {
		vector<int>::iterator largest = max_element(C.begin(), C.end());

		for (int i=0; i<*largest; i++) if (!exis(C, i)) {
		    output<<"Chain has not:"<<i<<endl;

			output<<"mask1 before:"<<endl;
            for (int i=0; i<mask1.size(); i++) { for (int j=0; j<mask1[i].size(); j++) output<<mask1[i][j]<<" "; output<<endl;}

		    edge T;
	        T.C.push_back(1);
		    vector<int> Ch = C;
	        vector<vector<edge>> G(H.size());
	        for (int j=0; j<H.size(); j++) G[j] = H[j];
	        for (int j=0; j<Ch.size() - 1; j++) T = T*G[Ch[j]][Ch[j + 1]];

			output<<"T:"<<endl;
	        copy( T.C.begin(), T.C.end(), ostream_iterator<int>(output," ") );
	        output<<endl<<T.power<<" "<<T.simple<<endl;

	        edge F1, F2;
			int F3 = 0;

			for (int j=0; j<Ch.size() - 1; j++) if (Ch[j] > i && mask1[i][Ch[j]]) { // j=Ch.size() - 1 doesn't make anything
			     edge L; // save power,simple, but lost P.C
		         L.C.push_back(1);

				 output<<"L befor:"<<endl;
	             copy( L.C.begin(), L.C.end(), ostream_iterator<int>(output," ") );
	             output<<endl<<L.power<<" "<<L.simple<<endl;


		         for (int k=0; k<j; k++) L = L*G[Ch[k]][Ch[k + 1]];

				 output<<"L after:"<<endl;
	             copy( L.C.begin(), L.C.end(), ostream_iterator<int>(output," ") );
	             output<<endl<<L.power<<" "<<L.simple<<endl;

				 if (F1.C.empty()) output<<"empty F1"<<endl;
				 else {
				      output<<"F1 before:"<<endl;
	                  copy( F1.C.begin(), F1.C.end(), ostream_iterator<int>(output," ") );
	                  output<<endl<<F1.power<<" "<<F1.simple<<endl;
				 }

				 F1 = F1 +(L - T);

				 output<<"F1 after:"<<endl;
	             copy( F1.C.begin(), F1.C.end(), ostream_iterator<int>(output," ") );
	             output<<endl<<F1.power<<" "<<F1.simple<<endl;
	        }
			for (int j=1; j<Ch.size(); j++) if (Ch[j] > i && mask1[i][Ch[j]]) { // j=0 doesn't make anything
			     edge P;
			     P.C.push_back(1);
		         for (int k=j; k<Ch.size() - 1; k++) P = P*G[Ch[k]][Ch[k + 1]];
		         F2 = F2 + (P - T);
	        }

			output<<"F2:"<<endl;
	        copy( F2.C.begin(), F2.C.end(), ostream_iterator<int>(output," ") );
	        output<<endl<<F2.power<<" "<<F2.simple<<endl;

			for (int j=0; j<Ch.size(); j++) if (Ch[j] > i && mask1[i][Ch[j]]) F3++;

			output<<"F3:"<<F3<<endl;

			for (int j=0; j<Ch.size(); j++) if (Ch[j] > i && mask1[i][Ch[j]]) {
				mask1[i][Ch[j]] = false;
				mask1[Ch[j]][i] = false;
			}

			output<<"mask1 after:"<<endl;
            for (int i=0; i<mask1.size(); i++) { for (int j=0; j<mask1[i].size(); j++) output<<mask1[i][j]<<" "; output<<endl;}
            
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
				output<<"Renumerate:"<<s<<" 0"<<endl;
			    renumerate(G, s, 0); 
				output<<"Renumerate:"<<y<<" 1"<<endl;
		        renumerate(G, y, 1); 
		    }
			if (s == 0 && y != 1) {
				output<<"Renumerate:"<<y<<" 1"<<endl;
				renumerate(G, y, 1); 
			}
			if (s == 1 && y != 0) {
				output<<"Renumerate:"<<y<<" 0"<<endl;
				renumerate(G, y, 0);
			}
			if (y == 0 && s != 1) {
				output<<"Renumerate:"<<s<<" 1"<<endl;
				renumerate(G, s, 1);
			}
			if (y == 1 && s != 0) {
				output<<"Renumerate:"<<s<<" 0"<<endl;
				renumerate(G, s, 0);
			}
	        edge k = procedure(G, F, connected); // Rsy

			output<<"k:"<<endl;
	        copy( k.C.begin(), k.C.end(), ostream_iterator<int>(output," ") );
	        output<<endl<<k.power<<" "<<k.simple<<endl;

			if (s != 0 && s != 1 && x != 0 && x != 1) {
				output<<"Renumerate:"<<s<<" 0"<<endl;
			    renumerate(G1, s, 0); 
				output<<"Renumerate:"<<x<<" 1"<<endl;
		        renumerate(G1, x, 1); 
		    }
			if (s == 0 && x != 1) {
				output<<"Renumerate:"<<x<<" 1"<<endl;
				renumerate(G1, x, 1); 
			}
			if (s == 1 && x != 0) {
				output<<"Renumerate:"<<x<<" 0"<<endl;
				renumerate(G1, x, 0);
			}
			if (x == 0 && s != 1) {
				output<<"Renumerate:"<<s<<" 1"<<endl;
				renumerate(G1, s, 1);
			}
			if (x == 1 && s != 0) {
				output<<"Renumerate:"<<s<<" 0"<<endl;
				renumerate(G1, s, 0);
			}  
	        edge w = procedure(G1, F, connected);// Rsx

			output<<"w:"<<endl;
	        copy( w.C.begin(), w.C.end(), ostream_iterator<int>(output," ") );
	        output<<endl<<w.power<<" "<<w.simple<<endl;
        
	        Ch = C;
	        G.resize(H.size());
	        for (int j=0; j<H.size(); j++) G[j] = H[j];

			output<<"G:"<<endl;
            for (int j=0; j<G.size(); j++) { for (int k=0; k<G[j].size(); k++) output<<G[j][k].ex<<" "; output<<endl;}

			output<<"Start contracting:"<<endl;
			s = i;
	        for (int j=0; j<Ch.size() - 1; j++) { // can contract 0,1 but it is not problem
				 output<<"contract:"<<Ch[j]<<" "<<Ch[j + 1]<<endl;
	             contractedge(G, Ch[j], Ch[j + 1]);
                 
				 int w = Ch[j];
			     if (Ch[j] == 1 || Ch[j] == 0) w = Ch[j + 1]; 
				 if (w < s) s--; // not forget about s
				 for (int k=0; k<Ch.size(); k++) if (w < Ch[k]) Ch[k]--; // not forget about Ch
				      // usually delete Ch[i], but not always 

                 output<<"G:"<<endl;
                 for (int j=0; j<G.size(); j++) { for (int k=0; k<G[j].size(); k++) output<<G[j][k].ex<<" "; output<<endl;}

				 if (Ch[j] == 1 || Ch[j] == 0) {
		             Ch.erase(Ch.begin() + j + 1);
		             j--;
                 }
				 else { // need to get <x,y> 
					 Ch.erase(Ch.begin() + j); 
					 j--;
				 }

				 output<<"Ch:"<<endl;
	             copy( Ch.begin(), Ch.end(), ostream_iterator<int>(output," ") );
				 output<<endl;
            }

		    int xy = Ch.front();
			output<<"<x,y>:"<<xy<<endl;
			if (s != 0 && s != 1 && xy != 0 && xy != 1) {
				output<<"Renumerate:"<<s<<" 0"<<endl;
			    renumerate(G, s, 0); 
				output<<"Renumerate:"<<xy<<" 1"<<endl;
		        renumerate(G, xy, 1); 
		    }
			if (s == 0 && xy != 1) {
				output<<"Renumerate:"<<xy<<" 1"<<endl;
				renumerate(G, xy, 1); 
			}
			if (s == 1 && xy != 0) {
				output<<"Renumerate:"<<xy<<" 0"<<endl;
				renumerate(G, xy, 0);
			}
			if (xy == 0 && s != 1) {
				output<<"Renumerate:"<<s<<" 1"<<endl;
				renumerate(G, s, 1);
			}
			if (xy == 1 && s != 0) {
				output<<"Renumerate:"<<s<<" 0"<<endl;
				renumerate(G, s, 0);
			}
	        edge m = procedure(G, F, connected); // Rs,xy

			output<<"m:"<<endl;
	        copy( m.C.begin(), m.C.end(), ostream_iterator<int>(output," ") );
	        output<<endl<<m.power<<" "<<m.simple<<endl;
            
			if (!sum.C.empty()) {
			    output<<"sum before:"<<endl;
	            copy( sum.C.begin(), sum.C.end(), ostream_iterator<int>(output," ") );
	            output<<endl<<sum.power<<" "<<sum.simple<<endl;
			}
			else output<<"sum empty"<<endl;
            
			output<<"Start computing sum + F2*k + F1*w + F3*T*m:"<<endl;
	        sum = sum + (F2*k + F1*w + F3*T*m); 

			output<<"sum after:"<<endl;
	        copy( sum.C.begin(), sum.C.end(), ostream_iterator<int>(output," ") );
	        output<<endl<<sum.power<<" "<<sum.simple<<endl;
	    }
	    else {
			output<<"Chain has:"<<i<<endl;

		    vector<int> Ch = C;
		    vector<int>::iterator it, r;
	        for (it=Ch.begin(); it<Ch.end(); ++it) if (*it == i) r = it; 
		    int q = r - Ch.begin(); // place of pivote node s=i into vector Ch 
			output<<"Place "<<i<<" in vector Ch:"<<q<<endl;

			output<<"mask1 before:"<<endl;
            for (int i=0; i<mask1.size(); i++) { for (int j=0; j<mask1[i].size(); j++) output<<mask1[i][j]<<" "; output<<endl;}

		    edge T;
	        T.C.push_back(1);
		    vector<vector<edge>> G(H.size());
		    for (int j=0; j<H.size(); j++) G[j] = H[j];
	        for (int j=0; j<Ch.size() - 1; j++) T = T*G[Ch[j]][Ch[j + 1]];

			output<<"T:"<<endl;
	        copy( T.C.begin(), T.C.end(), ostream_iterator<int>(output," ") );
	        output<<endl<<T.power<<" "<<T.simple<<endl;
               
	        edge F1, F2;
			int F3 = 0;
			for (int j=0; j<Ch.size(); j++) if (Ch[j] > i && mask1[i][Ch[j]]) {
			     edge P; // save power,simple, but lost P.C
				 P.C.push_back(1);

				 output<<"P befor:"<<endl;
	             copy( P.C.begin(), P.C.end(), ostream_iterator<int>(output," ") );
	             output<<endl<<P.power<<" "<<P.simple<<endl;

				 if (j < q) for (int k=j; k<q; k++) P = P*G[Ch[k]][Ch[k + 1]];
			     else for (int k=q; k<j; k++) P = P*G[Ch[k]][Ch[k + 1]];

				 output<<"P after:"<<endl;
	             copy( P.C.begin(), P.C.end(), ostream_iterator<int>(output," ") );
	             output<<endl<<P.power<<" "<<P.simple<<endl;
                 
				 if (F1.C.empty()) output<<"empty F1"<<endl;
				 else {
				      output<<"F1 before:"<<endl;
	                  copy( F1.C.begin(), F1.C.end(), ostream_iterator<int>(output," ") );
	                  output<<endl<<F1.power<<" "<<F1.simple<<endl;
				 }

			     F1 = F1 + P;

				 output<<"F1 after:"<<endl;
	             copy( F1.C.begin(), F1.C.end(), ostream_iterator<int>(output," ") );
	             output<<endl<<F1.power<<" "<<F1.simple<<endl;
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

			output<<"F2:"<<endl;
	        copy( F2.C.begin(), F2.C.end(), ostream_iterator<int>(output," ") );
	        output<<endl<<F2.power<<" "<<F2.simple<<endl;

			for (int j=0; j<Ch.size(); j++) if (Ch[j] > i && mask1[i][Ch[j]]) F3++;

			output<<"F3:"<<F3<<endl;

			for (int j=0; j<Ch.size(); j++) if (Ch[j] > i && mask1[i][Ch[j]]) {
				mask1[i][Ch[j]] = false;
				mask1[Ch[j]][i] = false;
			}

			output<<"mask1 after:"<<endl;
            for (int i=0; i<mask1.size(); i++) { for (int j=0; j<mask1[i].size(); j++) output<<mask1[i][j]<<" "; output<<endl;}

		    for (int j=1; j<Ch.size() - 1; j++) {
		         delnode(G, Ch[j]); 
				 for (int k=0; k<Ch.size(); k++) if (Ch[j] < Ch[k]) Ch[k]--; // not forget about Ch
	        }
		    int x = Ch.front(), y = Ch.back(); // doesn't matter x<=>y

		    if (x != 0 && x != 1 && y != 0 && y != 1) {
				output<<"Renumerate:"<<x<<" 0"<<endl;
			    renumerate(G, x, 0); 
				output<<"Renumerate:"<<y<<" 1"<<endl;
		        renumerate(G, y, 1); 
		    }
			if (x == 0 && y != 1) {
				output<<"Renumerate:"<<y<<" 1"<<endl;
				renumerate(G, y, 1); 
			}
			if (x == 1 && y != 0) {
				output<<"Renumerate:"<<y<<" 0"<<endl;
				renumerate(G, y, 0);
			}
			if (y == 0 && x != 1) {
				output<<"Renumerate:"<<x<<" 1"<<endl;
				renumerate(G, x, 1);
			}
			if (y == 1 && x != 0) {
				output<<"Renumerate:"<<x<<" 0"<<endl;
				renumerate(G, x, 0);
			}

		    edge n = procedure(G, F, connected); // Rxy

			output<<"n:"<<endl;
	        copy( n.C.begin(), n.C.end(), ostream_iterator<int>(output," ") );
	        output<<endl<<n.power<<" "<<n.simple<<endl;

			if (!sum.C.empty()) {
			    output<<"sum before:"<<endl;
	            copy( sum.C.begin(), sum.C.end(), ostream_iterator<int>(output," ") );
	            output<<endl<<sum.power<<" "<<sum.simple<<endl;
			}
			else output<<"sum empty"<<endl;
            
			output<<"Start computing sum + F1 + F2*n - F3*T*n:"<<endl;
		    sum = sum + (F1 + F2*n - F3*T*n); 

			output<<"sum after:"<<endl;
	        copy( sum.C.begin(), sum.C.end(), ostream_iterator<int>(output," ") );
	        output<<endl<<sum.power<<" "<<sum.simple<<endl;
		}
		return chainreduction1(H, sum, V, F, connected);
	}
	return sum;
}

edge chainreduction2(vector<vector<edge>> &H, edge sum, vector<int> weight, edge F, bool connected)
{ 
	output<<"chainreduction2:"<<endl;

	output<<"H:"<<endl;
    for (int i=0; i<H.size(); i++) { for (int j=0; j<H[i].size(); j++) output<<H[i][j].ex<<" "; output<<endl;}

	vector<int> V; // considered nodes

	for (int i=0; i<mask.size(); i++) if (!mask[i]) V.push_back(i);

	output<<"V:"<<endl;
	copy( V.begin(), V.end(), ostream_iterator<int>(output," ") );
	output<<endl;

    vector<int> C = fchain(H, V);

	output<<"Chain:"<<endl;
	copy( C.begin(), C.end(), ostream_iterator<int>(output," ") );
	output<<endl;

	if (!C.empty()) {
		for (int i=0; i<C.size(); i++) if (!exis(V, C[i])) mask[C[i]] = false;

	    if (!exis(C, 0)) {
			output<<"Chain has not 0:"<<endl;
		    edge T;
	        T.C.push_back(1);
		    vector<int> Ch = C;
	        vector<vector<edge>> G(H.size());
	        for (int i=0; i<H.size(); i++) G[i] = H[i];
	        for (int i=0; i<Ch.size() - 1; i++) T = T*G[Ch[i]][Ch[i + 1]];

			output<<"T:"<<endl;
	        copy( T.C.begin(), T.C.end(), ostream_iterator<int>(output," ") );
	        output<<endl<<T.power<<" "<<T.simple<<endl;

	        edge F1, F2;
			int F3 = 0;

			for (int i=0; i<Ch.size() - 1; i++) if (!exis(V, Ch[i])) { // i=Ch.size() - 1 doesn't make anything
			     edge L; // save power,simple, but lost P.C
		         L.C.push_back(1);

				 output<<"L befor:"<<endl;
	             copy( L.C.begin(), L.C.end(), ostream_iterator<int>(output," ") );
	             output<<endl<<L.power<<" "<<L.simple<<endl;


		         for (int j=0; j<i; j++) L = L*G[Ch[j]][Ch[j + 1]];

				 output<<"L after:"<<endl;
	             copy( L.C.begin(), L.C.end(), ostream_iterator<int>(output," ") );
	             output<<endl<<L.power<<" "<<L.simple<<endl;

				 if (F1.C.empty()) output<<"empty F1"<<endl;
				 else {
				      output<<"F1 before:"<<endl;
	                  copy( F1.C.begin(), F1.C.end(), ostream_iterator<int>(output," ") );
	                  output<<endl<<F1.power<<" "<<F1.simple<<endl;
				 }

				 F1 = F1 + weight[Ch[i]]*(L - T);

				 output<<"F1 after:"<<endl;
	             copy( F1.C.begin(), F1.C.end(), ostream_iterator<int>(output," ") );
	             output<<endl<<F1.power<<" "<<F1.simple<<endl;
	        }
			for (int i=1; i<Ch.size(); i++) if (!exis(V, Ch[i])) { // i=0 doesn't make anything
			     edge P;
			     P.C.push_back(1);
		         for (int j=i; j<Ch.size() - 1; j++) P = P*G[Ch[j]][Ch[j + 1]]; // ?
		         F2 = F2 + weight[Ch[i]]*(P - T);
	        }

			output<<"F2:"<<endl;
	        copy( F2.C.begin(), F2.C.end(), ostream_iterator<int>(output," ") );
	        output<<endl<<F2.power<<" "<<F2.simple<<endl;

			for (int i=0; i<Ch.size(); i++) if (!exis(V, Ch[i])) F3 += weight[Ch[i]];

			output<<"F3:"<<F3<<endl;
        
	        for (int i=1; i<Ch.size() - 1; i++) { // after this we get 2 nodes from chain, one of them can be pivote
		         delnode(G, Ch[i]); 
				 for (int j=0; j<Ch.size(); j++) if (Ch[i] < Ch[j]) Ch[j]--; // not forget about Ch
	        }
	        vector<vector<edge>> G1(G.size());
	        for (int i=0; i<G.size(); i++) G1[i] = G[i]; // after reduction G changed

	        int x = Ch.front(), y = Ch.back(); // matter x<=>y
			output<<"Renumerate:"<<y<<" 1"<<endl;
            renumerate(G, y, 1);  // s - node out of chain always 0, so t=1
	        edge k = procedure(G, F, connected); // R1y

			output<<"k:"<<endl;
	        copy( k.C.begin(), k.C.end(), ostream_iterator<int>(output," ") );
	        output<<endl<<k.power<<" "<<k.simple<<endl;

			output<<"Renumerate:"<<x<<" 1"<<endl;
	        renumerate(G1, x, 1);   
	        edge w = procedure(G1, F, connected);// R1x

			output<<"w:"<<endl;
	        copy( w.C.begin(), w.C.end(), ostream_iterator<int>(output," ") );
	        output<<endl<<w.power<<" "<<w.simple<<endl;
        
	        Ch = C;
	        G.resize(H.size());
	        for (int i=0; i<H.size(); i++) G[i] = H[i];

			output<<"G:"<<endl;
            for (int i=0; i<G.size(); i++) { for (int j=0; j<G[i].size(); j++) output<<G[i][j].ex<<" "; output<<endl;}

			output<<"Start contracting:"<<endl;
	        for (int i=0; i<Ch.size() - 1; i++) { 
				 output<<"contract:"<<Ch[i]<<" "<<Ch[i + 1]<<endl;
	             contractedge(G, Ch[i], Ch[i + 1]);

				 for (int j=0; j<Ch.size(); j++) { // usually delete Ch[i], but not always
			          int w = Ch[i];
			          if (Ch[i] == 1) w = Ch[i + 1]; // no 0 in Ch
			          if (w < Ch[j]) Ch[j]--; // not forget about Ch
	             }

                 output<<"G:"<<endl;
                 for (int i=0; i<G.size(); i++) { for (int j=0; j<G[i].size(); j++) output<<G[i][j].ex<<" "; output<<endl;}

		         if (Ch[i] == 1) {
		             Ch.erase(Ch.begin() + i + 1);
		             i--;
                 }
				 else { // need to get <x,y> 
					 Ch.erase(Ch.begin() + i); 
					 i--;
				 }

				 output<<"Ch:"<<endl;
	             copy( Ch.begin(), Ch.end(), ostream_iterator<int>(output," ") );
				 output<<endl;
            }

		    int xy = Ch.front();
			output<<"<x,y>:"<<xy<<endl;
			if (xy != 1) { // 1 can not be in Ch
				output<<"Renumerate:"<<xy<<" 1"<<endl;
				renumerate(G, xy, 1);
			}
	        edge m = procedure(G, F, connected);

			output<<"m:"<<endl;
	        copy( m.C.begin(), m.C.end(), ostream_iterator<int>(output," ") );
	        output<<endl<<m.power<<" "<<m.simple<<endl;
            
			if (!sum.C.empty()) {
			    output<<"sum before:"<<endl;
	            copy( sum.C.begin(), sum.C.end(), ostream_iterator<int>(output," ") );
	            output<<endl<<sum.power<<" "<<sum.simple<<endl;
			}
			else output<<"sum empty"<<endl;
            
			output<<"Start computing sum + F2*k + F1*w + F3*T*m:"<<endl;
	        sum = sum + (F2*k + F1*w + F3*T*m); 

			output<<"sum after:"<<endl;
	        copy( sum.C.begin(), sum.C.end(), ostream_iterator<int>(output," ") );
	        output<<endl<<sum.power<<" "<<sum.simple<<endl;

	        return chainreduction2(H, sum, weight, F, connected);
	    }
	    else {
			output<<"Chain has 0:"<<endl;
		    vector<int> Ch = C;
		    vector<int>::iterator it, r;
	        for (it=Ch.begin(); it<Ch.end(); ++it) if (*it == 0) r = it; 
		    int q = r - Ch.begin(); // place of pivote node s=0 into vector Ch 
			output<<"Place 0 in vector Ch:"<<q<<endl;

		    edge T;
	        T.C.push_back(1);
		    vector<vector<edge>> G(H.size());
		    for (int i=0; i<H.size(); i++) G[i] = H[i];
	        for (int i=0; i<Ch.size() - 1; i++) T = T*G[Ch[i]][Ch[i + 1]];

			output<<"T:"<<endl;
	        copy( T.C.begin(), T.C.end(), ostream_iterator<int>(output," ") );
	        output<<endl<<T.power<<" "<<T.simple<<endl;
               
	        edge F1, F2;
			int F3 = 0;
			for (int i=0; i<Ch.size(); i++) if (i != q && !exis(V, Ch[i])) {
			     edge P; // save power,simple, but lost P.C
				 P.C.push_back(1);

				 output<<"P befor:"<<endl;
	             copy( P.C.begin(), P.C.end(), ostream_iterator<int>(output," ") );
	             output<<endl<<P.power<<" "<<P.simple<<endl;

			     if (i < q) for (int j=i; j<q; j++) P = P*G[Ch[j]][Ch[j + 1]];
			     else for (int j=q; j<i; j++) P = P*G[Ch[j]][Ch[j + 1]];

				 output<<"P after:"<<endl;
	             copy( P.C.begin(), P.C.end(), ostream_iterator<int>(output," ") );
	             output<<endl<<P.power<<" "<<P.simple<<endl;
                 
				 if (F1.C.empty()) output<<"empty F1"<<endl;
				 else {
				      output<<"F1 before:"<<endl;
	                  copy( F1.C.begin(), F1.C.end(), ostream_iterator<int>(output," ") );
	                  output<<endl<<F1.power<<" "<<F1.simple<<endl;
				 }

			     F1 = F1 + weight[Ch[i]]*P;

				 output<<"F1 after:"<<endl;
	             copy( F1.C.begin(), F1.C.end(), ostream_iterator<int>(output," ") );
	             output<<endl<<F1.power<<" "<<F1.simple<<endl;
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

			output<<"F2:"<<endl;
	        copy( F2.C.begin(), F2.C.end(), ostream_iterator<int>(output," ") );
	        output<<endl<<F2.power<<" "<<F2.simple<<endl;

			for (int i=0; i<Ch.size(); i++) if (!exis(V, Ch[i])) F3 += weight[Ch[i]];

			output<<"F3:"<<F3<<endl;

		    for (int i=1; i<Ch.size() - 1; i++) {
		         delnode(G, Ch[i]); 
				 for (int j=0; j<Ch.size(); j++) if (Ch[i] < Ch[j]) Ch[j]--; // not forget about Ch
	        }
		    int x = Ch.front(), y = Ch.back(); // doesn't matter x<=>y

		    if (x != 0 && x != 1 && y != 0 && y != 1) {
				output<<"Renumerate:"<<x<<" 0"<<endl;
			    renumerate(G, x, 0); 
				output<<"Renumerate:"<<y<<" 1"<<endl;
		        renumerate(G, y, 1); 
		    }
			if (x == 0 && y != 1) {
				output<<"Renumerate:"<<y<<" 1"<<endl;
				renumerate(G, y, 1); 
			}
			if (x == 1 && y != 0) {
				output<<"Renumerate:"<<y<<" 0"<<endl;
				renumerate(G, y, 0);
			}
			if (y == 0 && x != 1) {
				output<<"Renumerate:"<<x<<" 1"<<endl;
				renumerate(G, x, 1);
			}
			if (y == 1 && x != 0) {
				output<<"Renumerate:"<<x<<" 0"<<endl;
				renumerate(G, x, 0);
			}

		    edge n = procedure(G, F, connected); // Rxy

			output<<"n:"<<endl;
	        copy( n.C.begin(), n.C.end(), ostream_iterator<int>(output," ") );
	        output<<endl<<n.power<<" "<<n.simple<<endl;

			if (!sum.C.empty()) {
			    output<<"sum before:"<<endl;
	            copy( sum.C.begin(), sum.C.end(), ostream_iterator<int>(output," ") );
	            output<<endl<<sum.power<<" "<<sum.simple<<endl;
			}
			else output<<"sum empty"<<endl;
            
			output<<"Start computing sum + F1 + F2*n - F3*T*n:"<<endl;
		    sum = sum + (F1 + F2*n - F3*T*n); 

			output<<"sum after:"<<endl;
	        copy( sum.C.begin(), sum.C.end(), ostream_iterator<int>(output," ") );
	        output<<endl<<sum.power<<" "<<sum.simple<<endl;

	        return chainreduction2(H, sum, weight, F, connected);
	    }
	}
	return sum;
}

int main()
{
setlocale(LC_ALL, "");
ifstream input("C://input.txt");
if (!input.is_open()) cout << "File can not be opened!\n"; 
else {
     char str[50];
	 int n = 0, m = 0;
	 vector<edge> E;
     input.getline(str,50); output<<str<<endl;
     int buf, count = 0;
     input>>buf; output<<"Number of nodes n="<<buf<<endl;
     n = buf;
     input>>buf; output<<"Number of edges m="<<buf<<endl;
     m = buf;

     while (count != m) {
	       bool reduct = false;
	       edge q; // filling data for new edge
	       q.simple = 0; 
	       q.ex = true;
	       q.power = 1;
	       q.C.push_back(1);
	       q.C.resize(m + 1); // numerate start from 0
           input>>buf; output<<buf - 1<<" "; 
	       q.node1 = buf - 1; 
	       input>>buf; output<<buf - 1<<endl;
	       q.node2 = buf - 1;
	       for (int i=0; i<E.size(); i++) // work with multi edges
			   if ( (E[i].node1 == q.node1 && E[i].node2 == q.node2) || (E[i].node1 == q.node2 && E[i].node2 == q.node1) ) {
		          E[i].simple++;
		          output<<"E["<<i<<"].simple="<<E[i].simple<<endl;
		          reduct = true;
	           }
	       if (!reduct) E.push_back(q);
	       count++;
    }

    input>>str; output<<str<<endl; // end of input
    if (strcmp(str,"$$$")!=0) cout<<"Uncorrect entry"<<endl; // 0 = ; 1 < ; -1 >
    else {
	     cout<<"Correct entry"<<endl;
		 output<<"Simple edges:"<<endl;
	     for (int i=0; i<E.size(); i++) output<<E[i].simple<<" ";
         output<<endl;

         vector<vector<edge>> S(n); 
         table(S, E);
	     output<<"Adjacency matrix:"<<endl;
         for (int i=0; i<S.size(); i++) { for (int j=0; j<S[i].size(); j++) output<<S[i][j].ex<<" "; output<<endl;}

		 bool r = gconnected(S);
         if (!r) cout<<"Unconnected graph on input!"<<endl;

         edge F; // create pseudo edge for computing
	     F.C.push_back(1);

         vector<int> weight(S.size()), V;
		 for (int i=0; i<weight.size(); i++) weight[i] = 1;
/*
		 edge sum;
		 unsigned int start_time = clock();
		 mask1.resize(S.size()); 
		 for (int i=0; i<mask1.size(); i++) { 
			  mask1[i].resize(S[i].size());
			  for (int j=0; j<mask1[i].size(); j++) i != j ? mask1[i][j] = true : mask1[i][j] = false;
		 }
		 sum = chainreduction1(S, sum, V, F, r);

		 output<<"mask1:"<<endl;
         for (int i=0; i<mask1.size(); i++) { for (int j=0; j<mask1[i].size(); j++) output<<mask1[i][j]<<" "; output<<endl;}
         
		 cout<<"chainreduction:"<<endl;
		 if (!sum.C.empty()) {
	         copy( sum.C.begin(), sum.C.end(), ostream_iterator<int>(cout," ") );
	         cout<<endl<<sum.power<<" "<<sum.simple<<endl;
		 }
		 
		 double value = 0, p = 0.9, z = 0.1;
		 int q = sum.power;
		 for (int i=0; i<=q; i++) {
			  value += sum.C[i]*pow(p, q - i)*pow(z, i);
		 }
		 cout<<"Value at point "<<p<<": "<<value/Bin(S.size(), 2)<<endl;

		 for (int i=0; i<S.size(); i++) for (int j=i + 1; j<S[i].size(); j++) if (mask1[i][j]) {
			 cout<<"+R"<<i + 1<<j + 1<<endl;
		     vector<vector<edge>> G(S.size()); 
			 for (int i=0; i<S.size(); i++) G[i] = S[i];
			 if (i!=0 || j!=1) { // after this 0,1
                 if (i!=0 && j!=1) {
                 renumerate (G, i, 0);
                 renumerate (G, j, 1);
                 }
                 if (i == 0 && j != 1) renumerate (G, j, 1);
                 if (i != 0 &&j == 1) renumerate (G, i, 0);
             } 
			 sum = sum + procedure(G, F, r); // G changed
		 }
		 unsigned int end_time = clock();
		 double search_time = end_time - start_time;
	     cout<<"Time of programm:"<<search_time/1000<<" sec"<<endl;

		 cout<<"R:"<<endl;
         copy( sum.C.begin(), sum.C.end(), ostream_iterator<int>(cout," ") );
         cout<<endl<<"Power:"<<sum.power<<" "<<"Simple:"<<sum.simple<<endl;
		 value = 0;
		 q = sum.power;
		 for (int i=0; i<=q; i++) {
			  value += sum.C[i]*pow(p, q - i)*pow(z, i);
		 }
		 cout<<"Value at point "<<p<<": "<<value/Bin(S.size(), 2)<<endl;
*/

		 edge sum;
		 unsigned int start_time = clock();
		 mask.resize(S.size()); 
		 for (int i=0; i<mask.size(); i++) i == 0 ? mask[i] = false : mask[i] = true;
		 sum = chainreduction2(S, sum, weight, F, r);

		 output<<"mask:"<<endl;
	     copy( mask.begin(), mask.end(), ostream_iterator<int>(output," ") );
	     output<<endl;
         
		 cout<<"chainreduction:"<<endl;
		 if (!sum.C.empty()) {
	         copy( sum.C.begin(), sum.C.end(), ostream_iterator<int>(cout," ") );
	         cout<<endl<<sum.power<<" "<<sum.simple<<endl;
		 }
		 
		 double value = 0, p = 0.9, z = 0.1;
		 int q = sum.power;
		 for (int i=0; i<=q; i++) {
			  value += sum.C[i]*pow(p, q - i)*pow(z, i);
		 }
		 cout<<"Value at point "<<p<<": "<<value<<endl;
        
		 for (int i=0; i<S.size(); i++) if (mask[i]) {
			 cout<<"+R1"<<i + 1<<endl;
			 vector<vector<edge>> G(S.size()); 
			 for (int j=0; j<S.size(); j++) G[j] = S[j];
             if (i != 1) renumerate (G, i, 1); // after this i=1
			 sum = sum + procedure(G, F, r); // G changed
		 }
		 unsigned int end_time = clock();
		 double search_time = end_time - start_time;
	     cout<<"Time of programm:"<<search_time/1000<<" sec"<<endl;

		 cout<<"C:"<<endl;
		 for (int i=0; i<sum.C.size(); i++) sum.C[i] = sum.C[i];
         copy( sum.C.begin(), sum.C.end(), ostream_iterator<int>(cout," ") );
         cout<<endl<<"Power:"<<sum.power<<" "<<"Simple:"<<sum.simple<<endl;
		 value = 0;
	     q = sum.power;
		 for (int i=0; i<=q; i++) {
			  value += sum.C[i]*pow(p, q - i)*pow(z, i);
		 }
		 cout<<"Value at point "<<p<<": "<<(value + 1)<<endl; // + R11=1

/*
		 int s, t;
		 cout<<"Entry nodes:"<<endl;
		 cout<<"s=";
		 cin>>s; s--; // numerate start from 0
		 cout<<"t=";
		 cin>>t; t--;
         if (s!=t && s<t) { 
	         if (s!=0 || t!=1) { // after this 0,1
                 if (s!=0 && t!=1) {
                 renumerate (S, s, 0);
                 renumerate (S, t, 1);
                 }
                 if (s==0 && t!=1) renumerate (S, t, 1);
                 if (s!=0 && t==1) renumerate (S, s, 0);
             } 

			 unsigned int start_time =  clock();
             edge sum = procedure(S, F, r);
			 unsigned int end_time = clock();
			 double search_time = end_time - start_time;
			 cout<<"Time of programm:"<<search_time/1000<<" sec"<<endl;

             cout<<"Solution:"<<endl;
             copy( sum.C.begin(), sum.C.end(), ostream_iterator<int>(cout," ") );
             cout<<endl<<"Power:"<<sum.power<<" "<<"Simple:"<<sum.simple<<endl;
			 cout<<"Were ends of recursion:"<<num<<endl;
			 cout<<" 2-dimension graph:"<<num2<<endl;
			 cout<<" 3-dimension graph:"<<num3<<endl;
			 cout<<" 4-dimension graph:"<<num4<<endl;
			 cout<<" 5-dimension graph:"<<num5<<endl;
			 
			 double value = 0, p = 0.9, z = 0.1;
			 int q = sum.power;
			 for (int i=0; i<=q; i++) {
				 value += sum.C[i]*pow(p, q - i)*pow(z, i);
			 }
			 cout<<"Value at point "<<p<<": "<<value<<endl;
         }
*/
    }
}
input.close();
output.close();

/*
array2.front() //Первый элемент массива array2
array2.back() //Последний элемент массива array2
array2.capacity()
for(it=vecInt.begin();it<vecInt.end();++it)
vecIntFirst.empty(); //возрашает true false
"условие" ? "выражение 1" : "выражение 2";
vecIntFirst.empty()?cout<<"\nvecIntFirst is empty\n":cout<<"vecIntFirst is not empty\n";
vecIntSecond.erase(vecIntSecond.begin()+2,vecIntSecond.begin()+6);
vector <int> vecInt(3,100); //Создаем вектор из 3 элементов и заполняем его значением 100
vector <int>::iterator it;
it = vecInt.begin() + 3; //Вектор указывает на 4 элемент (0-элемент+3-элемента)
vecFirst.swap(vecSecond);
swap (vecFirst.back(),vecSecond.back());
sort(numbers.begin(), numbers.end());
vector<int>::const_iterator five = lower_bound(numbers.begin(), numbers.end(), 5);
numbers.erase(remove_if(numbers.begin(), numbers.end(), bind2nd(greater<int>(), 4)), numbers.end() ); // Удаляем все элементы больше 4-х 
*/

system("pause>>void");
return 0;
}