#include "stdafx.h"
using namespace std;
vector<bool> visited;
ofstream output("C://output.txt");
int num = 0;


struct edge    
{   
    vector<int> C; 
	int power;
	int node1;
	int node2;
	int simple;
	bool ex;
};

int factor(int k) {
	int l=1;
	for (int i=2; i<=k; i++) l = l*i;
	return l;
}

int Bin(int k,int l) {
    return factor(k) / (factor(l)*factor(k - l));
}

edge operator *(edge x, edge y)
{    edge F;
     if (!x.C.empty() && !y.C.empty()) { // for edges not zero attendance
		 if ( x.C.size() == 1 ) return y;
		 if ( y.C.size() == 1 ) return x;
         F.C.resize(x.C.size());
		 F.power = 0;
	     F.simple = 0;

	     output<<endl<<"Operator *:"<<endl;
	     output<<"Edge x:"<<endl;
	     copy( x.C.begin(), x.C.end(), ostream_iterator<int>(output," ") );
	     output<<endl<<"power = "<<x.power<<" ; simple = "<<x.simple<<endl;
	     output<<"Edge y:"<<endl;
	     copy( y.C.begin(), y.C.end(), ostream_iterator<int>(output," ") );
	     output<<endl<<"power = "<<y.power<<" ; simple = "<<y.simple<<endl;

		 if (x.simple > 0 && x.power!=1) cout<<"Eror in Operator *"<<endl;
		 if (y.simple > 0 && y.power!=1) cout<<"Eror in Operator *"<<endl;

	     x.power += x.simple; 
	     y.power += y.simple;
	     if (x.simple > 0) for (int i=0; i<x.power; i++) x.C[i] = Bin(x.power,i); // paralel reduction
	     if (y.simple > 0) for (int i=0; i<y.power; i++) y.C[i] = Bin(y.power,i);
     
         try {	
		     F.power = x.power + y.power; 
	         for (int i=0; i<x.C.size(); i++) for (int j=0; j<y.C.size(); j++) {
			     if (x.C[i]!=0 && y.C[j]!=0) {
				     if (i+j >= F.C.size())  throw "Eror - vector is out of size";
				     F.C[i+j] += x.C[i] * y.C[j];
			     }
		     }
	     }
	     catch (char *str){
	            output<<str<<endl;
		        for (int i=0; i<x.C.size(); i++) output<<x.C[i]<<" ";
	            output<<endl<<"*"<<endl;;
	            for (int i=0; i<y.C.size(); i++) output<<y.C[i]<<" ";
	            output<<endl;
	     }
	 
	     output<<"Edge x*y:"<<endl;
	     copy( F.C.begin(), F.C.end(), ostream_iterator<int>(output," ") );
	     output<<endl<<"power = "<<F.power<<" ; simple = "<<F.simple<<endl;
	     output<<"End operator * ;"<<endl<<endl;
	 }
	 return F;
}

edge operator +(edge x,edge y)
{   edge F;
    if (!x.C.empty() && !y.C.empty()) { // for edges not zero attendance
        F.C.resize(x.C.size()); 
	    F.simple = 0;

	    output<<endl<<"Operator +:"<<endl;
	    output<<"Edge x:"<<endl;
	    copy( x.C.begin(), x.C.end(), ostream_iterator<int>(output," ") );
        output<<endl<<"power = "<<x.power<<" ; simple = "<<x.simple<<endl;
	    output<<"Edge y:"<<endl;
	    copy( y.C.begin(), y.C.end(), ostream_iterator<int>(output," ") );
	    output<<endl<<"power = "<<y.power<<" ; simple = "<<y.simple<<endl;

	    if (x.simple > 0 && x.power!=1) cout<<"Eror in Operator +"<<endl;
	    if (y.simple > 0 && y.power!=1) cout<<"Eror in Operator +"<<endl;

	    x.power += x.simple;
     	y.power += y.simple;
		F.power = max(x.power, y.power);
	    if (x.simple > 0) for (int i=0; i<x.power; i++) x.C[i] = Bin(x.power, i);
	    if (y.simple > 0) for (int i=0; i<y.power; i++) y.C[i] = Bin(y.power, i);
		x.simple = 0;
		y.simple = 0;

        edge I;
	    I.C.resize(x.C.size());
	    I.power = abs(x.power - y.power);
	    I.simple = 0;
	    for (int j=0; j<=I.power; j++) I.C[j] = Bin(I.power, j);
	    if (x.power < y.power) x = x * I; 
	    if (y.power < x.power) y = y * I;

		for (int i=0; i<x.C.size(); i++) F.C[i] = x.C[i] + y.C[i];
    
	    output<<"Edge x+y:"<<endl;
	    copy( F.C.begin(), F.C.end(), ostream_iterator<int>(output," ") );
	    output<<endl<<"power = "<<F.power<<" ; simple = "<<F.simple<<endl;
	    output<<"End operator + ;"<<endl<<endl;
	}
	if (!x.C.empty() && y.C.empty()) F = x;
	if (x.C.empty() && !y.C.empty()) F = y;
	return F;
}

edge operator -(edge x,edge y)
{   edge F;
    if (!x.C.empty() && !y.C.empty()) { // for edges not zero attendance
        F.C.resize(x.C.size()); 
	    F.simple = 0;

	    output<<endl<<"Operator -:"<<endl;
	    output<<"Edge x:"<<endl;
	    copy( x.C.begin(), x.C.end(), ostream_iterator<int>(output," ") );
        output<<endl<<"power = "<<x.power<<" ; simple = "<<x.simple<<endl;
	    output<<"Edge y:"<<endl;
	    copy( y.C.begin(), y.C.end(), ostream_iterator<int>(output," ") );
	    output<<endl<<"power = "<<y.power<<" ; simple = "<<y.simple<<endl;

	    if (x.simple > 0 && x.power!=1) cout<<"Eror in Operator -"<<endl;
	    if (y.simple > 0 && y.power!=1) cout<<"Eror in Operator -"<<endl;

	    x.power += x.simple;
     	y.power += y.simple;
		F.power = max(x.power, y.power);
	    if (x.simple > 0) for (int i=0; i<x.power; i++) x.C[i] = Bin(x.power, i);
	    if (y.simple > 0) for (int i=0; i<y.power; i++) y.C[i] = Bin(y.power, i);
		x.simple = 0;
		y.simple = 0;

        edge I;
	    I.C.resize(x.C.size());
	    I.power = abs(x.power - y.power);
	    I.simple = 0;
	    for (int j=0; j<=I.power; j++) I.C[j] = Bin(I.power, j);
	    if (x.power < y.power) x = x * I; 
	    if (y.power < x.power) y = y * I;

		for (int i=0; i<x.C.size(); i++) F.C[i] = x.C[i] - y.C[i];
    
	    output<<"Edge x-y:"<<endl;
	    copy( F.C.begin(), F.C.end(), ostream_iterator<int>(output," ") );
	    output<<endl<<"power = "<<F.power<<" ; simple = "<<F.simple<<endl;
	    output<<"End operator - ;"<<endl<<endl;
	}
	if (!x.C.empty() && y.C.empty()) F = x;
	if (x.C.empty() && !y.C.empty()) F = y;
	return F;
}

edge operator ~(edge x)
{   
	edge F;
	if (!x.C.empty()) { // for edges not zero attendance
		F.C.resize(x.C.size()); 
		F.simple = 0;

		output<<endl<<"Operator ~:"<<endl;
	    output<<"Edge x:"<<endl;
	    copy( x.C.begin(), x.C.end(), ostream_iterator<int>(output," ") );
	    output<<endl<<"power = "<<x.power<<" ; simple = "<<x.simple<<endl;
	
	    if (x.simple > 0 && x.power!=1) cout<<"Eror in Operator ~"<<endl;

	    x.power += x.simple;
		F.power = x.power;
	    if (x.simple > 0) for (int i=0; i<x.power; i++) x.C[i] = Bin(x.power, i);

	    for (int i=0; i<=F.power; i++) F.C[i] = Bin(x.power, i) - x.C[i];

	    output<<"Edge ~x:"<<endl;
	    copy( F.C.begin(), F.C.end(), ostream_iterator<int>(output," ") );
	    output<<endl<<"power = "<<F.power<<" ; simple = "<<F.simple<<endl;
	    output<<"End operator ~ ;"<<endl<<endl;
	}
	else {
		F.C.push_back(1);
	    F.power = 0;
	    F.simple = 0;
	}
	return F;
}

edge sum;

void table(vector<vector<edge>> &H, vector<edge> &E) 
{
    for (int i=0; i<H.size(); i++) H[i].resize(H.size()); //initialize all strings, so as array is associative, not fill by nulls
    for (int i=0; i<H.size(); i++) for (int j=0; j<H[i].size(); j++) {
		 H[i][j].ex = false;
		 for (int w=0; w<E.size(); w++) 
		      if ((i==E[w].node1 && j==E[w].node2) || (i==E[w].node2 && j==E[w].node1)) H[i][j] = E[w];
	}
}

vector<int> gnodepower( vector<vector<edge>> &H)
{   vector <int> nodepower(H.size());
	int i = 0; 
    while( i < H.size() ) {
		for (int j=0; j<H[i].size(); j++) if ( H[i][j].ex ) nodepower[i]++;
		i++;
	}
	return nodepower;
}


void DFS(vector<vector<edge>> &H, vector<bool> &visited, int q)
{
     visited[q] = true;
	 for (int i=0; i<H.size(); i++) for (int j=i+1; j<H[i].size(); j++) if ( H[i][j].ex && ( !visited[i] || !visited[j] ) ) {
		 if (!visited[i]) DFS(H, visited, i);
		 else DFS(H, visited, j);
	 }
}

bool gconnected (vector<vector<edge>> &H)
{
	 int count = 0;
	 visited.resize(H.size());
	 for (int i=0; i<visited.size(); i++) visited[i] = false;

     DFS(H, visited, 0);
	 for (int i=0; i<visited.size(); i++) if (visited[i]) count++;

	 output<<"Graph connectivity: "<<H.size()<<" = "<<count<<endl;
	 if (count!=H.size()) return false;
     else return true;
}

void delnode ( vector<vector<edge>> &H,int q)
{
	output<<"Deleted node:"<<q<<endl;
	H.erase(H.begin() + q);
	for (int i=0; i<H.size(); i++) H[i].erase(H[i].begin() + q);

	output<<"After delete:"<<endl;
	for (int i=0; i<H.size(); i++) { for (int j=0; j<H[i].size(); j++) output<<H[i][j].ex<<" "; output<<endl;}
}

void deledge (vector<vector<edge>> &H, int q, int w) 
{  
	output<<"Deleted edge:"<<q<<" "<<w<<endl;
	edge F;
	F.ex = false;
    H[q][w] = F;
    H[w][q] = F;

}

void contractedge ( vector<vector<edge>> &H,int q, int w)
{
	output<<"Contracted edge:"<<q<<" "<<w<<endl;
    if (q==0 || q==1) cout<<"Eror in Contract edge"<<endl;

    for (int i=0; i<H[q].size(); i++) if (H[q][i].ex && i!=w) { // do not consider H[q][w]
		if (H[w][i].ex && H[q][i].power==1 ) { //  power==1 is falg of simple edge
		    H[w][i].simple += H[q][i].simple + 1;
		    H[i][w].simple += H[q][i].simple + 1;
			output<<"Contracted to existen edge, H["<<w<<"]["<<i<<"].simple="<<H[w][i].simple<<endl;
		}
		if (H[w][i].ex && H[q][i].power!=1 ) {
			cout<<"Eror in Contracted edge"<<endl;
		}
		if (!H[w][i].ex)  {
			output<<"H["<<w<<"]["<<i<<"] = H["<<q<<"]["<<i<<"]"<<endl;
			H[w][i] = H[q][i];
		    H[i][w] = H[q][i];
		}
	} 
	delnode (H, q);
}

void renumerate (int s, int t, vector<vector<edge>> &H) 
{
	output<<"Renumerate:"<<s<<" "<<t<<endl; 
	if (s!=t) {
		vector<edge> F, G;
		F = H[s];
		G = H[t];

		for (int i=0; i<H.size(); i++) if (i!=s && i!=t) swap(F[i], G[i]);
		
		output<<"Just clear strings and columns for nodes:"<<s<<" "<<t<<endl;
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
    output<<"After renumerate:"<<endl;
	for (int i=0; i<H.size(); i++) { for (int j=0; j<H[i].size(); j++) output<<H[i][j].ex<<" "; output<<endl;}
	}
}

edge penduntreduction( vector<vector<edge>> &H, edge F , int q, bool w)
{
	int s,t;
	vector<int> nodepower = gnodepower(H);
	if (w) nodepower.erase(nodepower.begin() + q); // in one penduntreduction we not consider pendunt node 0 || 1
	output<<"Pendunt reduction:"<<endl;
    
	vector <int>::iterator it, smallest = nodepower.end() - 1;
    for(it=nodepower.begin();it<nodepower.end();++it) if ( *it == 1 ) smallest = it; 
    int r = smallest - nodepower.begin(); // find pendunt node, can be 0,1

	if ( (*smallest==1) && (H.size()>2) ) { //otherwise all will be deleted
       for (int i = H[r].size() - 1; i >= 0; i--) 
		   if (H[r][i].ex) { //find incident edge to pendunt node, can't be 0,1 || 1,0
               output<<"Pendunt node:"<<r<<endl;
	           output<<"Edge incident to pendunt node:"<<r<<","<<i<<endl;
		       
	           if (r!=0 && r!=1) delnode(H,r);
	           else {
		            if (r==0 && i!=1) {
					    F = F*H[r][i];
						delnode(H,r);
			            s = i;
						s--;
			            if (s != 1) renumerate (s, 1, H); 
		            }              
		            if (r==1 && i!=0) {
					    F = F*H[r][i];
						delnode(H,r);
			            t = i;
						t--;
			            if (t != 1) renumerate (t, 1, H); 
		            }
	           }
			   if ( (r==1 && i==0) || (r==0 && i==1) ) { 
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
	vector <int> nodepower = gnodepower(H);
	vector <int>::iterator it, smallest = nodepower.end() - 1;
    
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

edge procedure (vector<vector<edge>> &H, edge F, bool connected) 
{
	output<<"Adjacency matrix before reduction:"<<endl;
    for (int i=0; i<H.size(); i++) { for (int j=0; j<H[i].size(); j++) output<<H[i][j].ex<<" "; output<<endl;}
	output<<"Simple edges:"<<endl;
	for (int i=0; i<H.size(); i++) { for (int j=0; j<H[i].size(); j++) if (H[i][j].simple>=0) output<<H[i][j].simple<<"\t";
	    else  output<<"none\t"; 
		output<<endl;
	}
	output<<"Vector befor reduction F:"<<endl;
	copy( F.C.begin(), F.C.end(), ostream_iterator<int>(output, " ") );
	output<<endl<<F.power<<" "<<F.simple<<endl;

	if (!connected) {
		bridgereduction(H);
		connected = true;
	}

	F = penduntreduction(H, F, 0 , false);

	output<<"Adjacency matrix after reduction:"<<endl;
    for (int i=0; i<H.size(); i++) { for (int j=0; j<H[i].size(); j++) output<<H[i][j].ex<<" "; output<<endl;}
	output<<"Vector obtained after reduction F:"<<endl;
	copy( F.C.begin(), F.C.end(), ostream_iterator<int>(output," ") );
	output<<endl<<F.power<<" "<<F.simple<<endl;

	if (H.size() < 5) {
		output<<"End of recursion"<<endl;
		output<<"Adjacency at the End of recursion:"<<endl;
        for (int i=0; i<H.size(); i++) { for (int j=0; j<H[i].size(); j++) output<<H[i][j].ex<<" "; output<<endl;}
	    output<<"Simple edges:"<<endl;
	    for (int i=0; i<H.size(); i++) { for (int j=0; j<H[i].size(); j++) if (H[i][j].simple>=0) output<<H[i][j].simple<<"\t";
	    else  output<<"none\t"; 
		output<<endl;
	    }
		num++;
		output<<"-------------------------->>>"<<endl;
	    
		if (H.size()==2) return F*H[0][1];
		if (H.size()==3) return F*( H[0][1] + H[1][2]*H[0][2] - H[0][1]*H[1][2]*H[0][2] );
		if (H.size()==4) 
			return F*( H[0][1] + ~H[0][1]* ( H[1][2]*H[0][2] + H[0][3]*H[1][3] - H[1][2]*H[0][2]*H[0][3]*H[1][3] +
			       H[1][2]*H[2][3]*H[0][3]*~H[1][3]*~H[0][2] + ~H[1][2]*H[2][3]*~H[0][3]*H[1][3]*H[0][2] ) );
		
		if (H.size()==5) 
            return F*( ~( H[0][1]*H[0][2]* ( ~H[1][2]*H[1][4]*( ~H[0][4]*H[0][3]*H[2][4]*(H[3][4] + H[1][3]*H[2][3]*~H[3][4]) +
		           ~H[0][3]*H[1][3]*H[2][3]*(H[2][4] + H[0][4]*H[3][4]*~H[2][4])) + H[0][3]*H[0][4]*~(H[1][2]*H[1][3]*H[1][4]) ) +
		           H[0][1]*H[1][3]* ( ~H[1][4]*H[0][4]*(~H[0][3]*H[0][2]*H[3][4]*(H[2][3] + H[1][2]*H[2][4]*~H[2][3]) +
		           ~H[0][2]*H[1][2]*H[2][4]*(H[3][4] + H[0][3]*H[2][3]*~H[3][4])) + H[1][2]*H[1][4] ) +
		           ~H[1][3]*H[0][1]*H[0][3]*H[1][2]* ( ~H[0][2]*H[0][4]*H[2][3]*(H[2][4] + H[1][4]*H[3][4]*~H[2][4]) +
		           ~H[0][4]*H[1][4]*H[3][4]*(H[2][3] + H[1][4]*H[2][4]*~H[2][3]) ) ) );
	}
	else {
		 edge W = fedge(H);
		 output<<"\t Allowing edge:"<<W.node1<<" "<<W.node2<<endl;
		 if ((W.node1==1 && W.node2==0) || (W.node1==0 && W.node2==1)) cout<<"Eror in Allowing edge"<<endl;
		 
		 vector<vector<edge>> H1(H.size());
         vector<vector<edge>> H2(H.size());
		 for (int i=0; i<H.size(); i++)  { 
			 H1[i] = H[i];
			 H2[i] = H[i];
		 }
		 edge F1,F2;
		 if (W.power != 1) cout<<"Eror in Allowing edge"<<endl;
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
         
		 output<<"p*F:"<<endl;
	     copy( F1.C.begin(), F1.C.end(), ostream_iterator<int>(output," ") );
	     output<<endl<<F1.power<<" "<<F1.simple<<endl;
		 output<<"(1-p)*F:"<<endl;
	     copy( F2.C.begin(), F2.C.end(), ostream_iterator<int>(output," ") );
	     output<<endl<<F2.power<<" "<<F2.simple<<endl;
       
         contractedge (H1, W.node1, W.node2);
		 deledge (H2, W.node1, W.node2);
		 
		 bool r = gconnected(H2);
		 if (!r) cout<<"Get unconnected graph into the recursion"<<endl;

		 return  procedure ( H1, F1, connected ) + procedure ( H2, F2, r ) ;
			 //F1*procedure ( H1, r ) + F2*procedure ( H2, connected );
	}	
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

     while (count!=m) {
	       bool reduct = false;
	       edge q;
	       q.simple = 0; // filling data for new edge
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
		 int s, t;
		 cout<<"Entry nodes:"<<endl;
		 cout<<"s=";
		 cin>>s; s--; // numerate start from 0
		 cout<<"t=";
		 cin>>t; t--;
         output<<"Connectivity of nodes:"<<s<<" "<<t<<endl;
         if (s!=t && s<t) { 
	         output<<"Simple edges:"<<endl;
	         for (int i=0; i<E.size(); i++) output<<E[i].simple<<" ";
             output<<endl;

             vector<vector<edge>> S(n); 
             table(S,E);
	         output<<"Adjacency matrix:"<<endl;
             for (int i=0; i<S.size(); i++) { for (int j=0; j<S[i].size(); j++) output<<S[i][j].ex<<" "; output<<endl;}

	         if (s!=0 || t!=1) { // after this 0,1
                 if (s!=0 && t!=1) {
                 renumerate (s,0,S);
                 renumerate (t,1,S);
                 }
                 if (s==0 && t!=1) renumerate (t,1,S);
                 if (s!=0 && t==1) renumerate (s,0,S);
             } 
	         output<<"Adjacency matrix after initial renumerate:"<<endl;
             for (int i=0; i<S.size(); i++) { for (int j=0; j<S[i].size(); j++) output<<S[i][j].ex<<" "; output<<endl;}

             bool r = gconnected(S);
             if (!r) cout<<"Unconnected graph on input!"<<endl;

             edge F; // create pseudo edge for computing
	         F.C.push_back(1);
			 F.C.resize(m + 1); // That dont resize in recursion
	         F.power = 0;
	         F.simple = 0;

             sum = procedure(S, F, r);
             cout<<"Solution:"<<endl;
             copy( sum.C.begin(), sum.C.end(), ostream_iterator<int>(cout," ") );
             cout<<endl<<"Power:"<<sum.power<<" "<<"Simple:"<<sum.simple<<endl;
			 cout<<"Were ends of recursion:"<<num<<endl;
			 
			 float value = 0, p = 0.9;
			 int q = sum.power;
			 float z = 1 - p;
			 for (int i=0; i<=q; i++) {
				 value += sum.C[i]*pow(p, q - i)*pow(z, i);
			 }
			 cout<<"Value at point "<<p<<": "<<value<<endl;
         }
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