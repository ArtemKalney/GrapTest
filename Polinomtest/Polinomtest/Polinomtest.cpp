#include "stdafx.h"
using namespace std;
vector<bool> visited;


struct edge    
{   
    vector<int> C; 
	int power;
	int node1;
	int node2;
	bool ex;

	edge& operator ==(edge x){
	C=x.C;
	power=x.power;
	return *this;
    }
};

edge operator *(edge x,edge y)
{    edge F;F.C.resize(x.C.size());
     F.power=x.power+y.power;	 
	 try {
	     for (int i=0; i<x.C.size(); i++) for (int j=0; j<y.C.size(); j++) {
			 if (x.C[i]!=0&&y.C[j]!=0) {
				 if (i+j >= x.C.size())  throw "������ - ������ �����������!";
				 F.C[i+j]+=x.C[i]*y.C[j];
			 }
		 }
	 }
	 catch (char *str){
	       cout<<str<<endl;
		   for (int i=0; i<x.C.size(); i++) cout<<x.C[i]<<" ";
	       cout<<endl<<"*"<<endl;;
	       for (int i=0; i<y.C.size(); i++) cout<<y.C[i]<<" ";
	       cout<<endl;
	 }
	 return F;
}
int factor(int k) {
	int l=1;
	for (int i=2; i<=k; i++) l=l*i;
	return l;
}

int Bin(int k,int l) {
    return factor(k)/(factor(l)*factor(k-l));
}

edge operator +(edge x,edge y)
{   edge F;F.C.resize(x.C.size()); 
    F.power=max(x.power,y.power);
    edge I;I.C.resize(x.C.size());
	I.power=abs(x.power-y.power);
	for (int j=0; j<=I.power; j++) I.C[j]=Bin(I.power,I.power-j);
	if (x.power<y.power) x=x*I;
	if (y.power<x.power) y=y*I;
	for (int i=0; i<x.C.size(); i++) F.C[i]=x.C[i]+y.C[i];
	return F;
}

edge operator -(edge x,edge y)
{   edge F;F.C.resize(x.C.size()); 
    F.power=max(x.power,y.power);
	edge I;I.C.resize(x.C.size());
	I.power=abs(x.power-y.power);
    for (int j=0; j<=I.power; j++) I.C[j]=Bin(I.power,I.power-j);
	if (x.power<y.power) x=x*I;
	if (y.power<x.power) y=y*I;
	for (int i=0; i<x.C.size(); i++) F.C[i]=x.C[i]-y.C[i];
	return F;
}
edge sum;

void table(vector<vector<edge>> &H,vector<edge> &E) 
{
    for (int i=0; i<H.size(); i++) H[i].resize(H.size()); //�������������� ��� ������, ��� ��� ������ ������������� �� �� ������
    for (int i=0; i<H.size(); i++) {
	    for (int j=0; j<H[i].size(); j++) {
		    H[i][j].ex=false;
		    for (int w=0; w<E.size(); w++) {
			    if ((i==E[w].node1&&j==E[w].node2)||(i==E[w].node2&&j==E[w].node1)) {
                   H[i][j] = E[w];//���� E[w] ��������� �������� 
			     }
		     }
	    }
    }
}

vector<int> gnodepower(int s,int t, vector<vector<edge>> &H)
{   vector<int> nodepower(H.size());
	int i=0;
    while( i<nodepower.size() ) {
		for (int j=0; j<H[i].size(); j++) if ( (H[i][j].ex==true) && (i!=s) && (i!=t) ) nodepower[i]++;
        if ( (i==s) || (i==t) ) nodepower[i]=H.size(); //�������
		i++;
	}
	return nodepower;
}


void DFS(vector<vector<edge>> &H,vector<bool> &visited, int q)
{
     visited[q]=true;
	 for (int i=0; i<H.size(); i++) for (int j=i+1; j<H[i].size(); j++) if ( (H[i][j].ex==true)&&( (!visited[i])||(!visited[j]) ) ) {
		 if (!visited[i]){
		    // cout<<i<<" ";
		     DFS(H,visited,i);
		 }
		 else {
			// cout<<j<<" ";
			 DFS(H,visited,j);
		 }
	 }

}

bool gconnected (int s,int t,vector<vector<edge>> &H)
{
	 int count=0;
	 visited.resize(H.size());
	 for (int i=0; i<visited.size(); i++) visited[i]=false;
     DFS(H,visited,s);
	 for (int i=0; i<visited.size(); i++) if (visited[i]==true) count++;
	 cout<<"��������� ����� ��� "<<s<<" = "<<H.size()<<" "<<count<<endl;
	 if (count!=H.size()) return false;
     else return true;
}

void delnode (int s, int t, vector<vector<edge>> &H,int q)
{
	cout<<"��������� �������:"<<q<<endl;
	H.erase(H.begin()+q);
	for (int i=0; i<H.size(); i++) H[i].erase(H[i].begin()+q);
	if (q<H.size()) {
	    for (int i=0; i<q; i++) for (int j=q; i<H.size(); i++) if (H[i][j].ex==true) H[i][j].node2--;
	    for (int i=q; i<H.size(); i++) for (int j=0; i<q; i++) if (H[i][j].ex==true) H[i][j].node1--;
	    for (int i=q; i<H.size(); i++) for (int j=q; i<H.size(); i++) if (H[i][j].ex==true){
		    H[i][j].node1--;
		    H[i][j].node2--;
	    }
	}
	if (s>q) s--;
	if (t>q) t--;

}

void deledge (int s,int t,vector<vector<edge>> &H,int q,int w) //��� �������� ����� ����� ���������� ������� �������
{  
	cout<<"������� �����:"<<q<<" "<<w<<endl;
	edge F;F.ex=false;
    H[q][w]=F;
    H[w][q]=F;
	vector<int> nodepower=gnodepower(s,t,H);
	vector<int>::iterator smallest = min_element( nodepower.begin(), nodepower.end() );
	if (*smallest==0) delnode(s,t,H,smallest - nodepower.begin());
}

void contractedge (int s, int t, vector<vector<edge>> &H,int q,int w)
{
	int r;
	if (w==s||w==t) { // ���� � ����������� ����� �������� s ��� t
		r=q;
		q=w;
		w=r;
	}
	cout<<"������� �����:"<<q<<" "<<w<<endl;
    for (int i=0; i<H[w].size(); i++) if (H[w][i].ex==true) {
		if (H[q][i].ex==true) {
			H[q][i]==H[w][i]+H[q][i]-H[w][i]*H[q][i];
			H[i][q]==H[i][w]+H[i][q]-H[i][w]*H[i][q];
		}
		else {
			 H[q][i] == H[w][i];
		     H[i][q] == H[i][w];
		}
	} 
	delnode (s,t,H,w);
}
void penduntreduction(int s, int t, vector<vector<edge>> &H)
{
	vector<int> nodepower=gnodepower(s,t,H);
	cout<<"�������� ������� ������:"<<endl;
	vector<int>::iterator smallest = min_element( nodepower.begin() , nodepower.end() ); 
	if((*smallest==1)&&(H.size()>2)) {         //����� �� ������
       for (int i=0; i<H[smallest - nodepower.begin()].size(); i++) if ((H[i][smallest - nodepower.begin()].ex==true)) { //������� ����������� �����  
	   sum = sum*H[i][smallest - nodepower.begin()]; 
	   //break;
	   }
	   delnode(s,t,H,smallest - nodepower.begin());
	   penduntreduction(s,t,H);
	}

}
void bridgereduction(int s,int t, vector<vector<edge>> &H)
{
	cout<<"�������� �����!"<<endl;
	if(visited[s]&&visited[t]) for (int i=0; i<H.size(); i++) if (!visited[i])  delnode(s,t,H,i);
	if((!visited[s])&&(!visited[t])) for (int i=0; i<H.size(); i++) if (visited[i])  delnode(s,t,H,i); 
}

edge fedge(int s,int t,vector<vector<edge>> &H)
{
	edge F;
	vector<int> nodepower=gnodepower(s,t,H);
	vector<int>::iterator smallest = min_element( nodepower.begin(), nodepower.end() );// ��������� nodepower.end() ���� �������� ����, �� s,t
	/*cout << "���������� ������� " << *smallest << endl;
      cout << "������ ����� �������� " << smallest - nodepower.begin() << endl;*/
	for (int i=0; i<H[smallest - nodepower.begin()].size(); i++) if ( H[i][smallest - nodepower.begin()].ex==true  ) {  // ���� � ������ ����������� ����� ���������� ������� ����� ���� s ��� t
		F = H[i][smallest - nodepower.begin()]; 
		//break;
	}
	return F;
}
void renumerate (int s, int t, vector<vector<edge>> &H) 
{
	edge F,G; 
	cout<<"renumerate:"<<s<<" "<<t<<endl;
	if (s!=0 && t!=1) for (int i=0; i<H.size(); i++) { //����� ������������� �� ������
		if ((H[s][i].ex==true)&&(H[0][i].ex==true)&&(i!=s)) {  // ������ ������ � ������ ��� s � 0, ��� i=s ������� �������� 
		    F=H[s][i];
		    G=H[i][s];
		    H[s][i]==H[0][i];
		    H[i][s]==H[i][0];
		    H[0][i]==F;
		    H[i][0]==G;
		}
		if ((H[t][i].ex==true)&&(H[1][i].ex==true)&&(i!=t)) { // t � 1
		    F=H[t][i];
		    G=H[i][t];
		    H[t][i]==H[1][i];
		    H[i][t]==H[i][1];
		    H[1][i]==F;
		    H[i][1]==G;
		}
	}
}

edge procedure (int s,int t,vector<vector<edge>> &H,bool connected) // F �����������
{
	if (connected!=true) {
		bridgereduction(s,t,H);
		connected=true;
	}
	penduntreduction(s,t,H);

	cout<<"����������� ������� ���������:"<<H.size()<<endl;
    for (int i=0; i<H.size(); i++) { for (int j=0; j<H[i].size(); j++) cout<<H[i][j].ex<<" "; cout<<endl;}

	if (H.size()<3) {
		renumerate (s,t,H);
		edge G;G.C.resize(H[0][1].C.size());G.C[1]=1;G.power=0;
		cout<<"����� �� �����"<<endl;
		for (int i=0; i<H[0][1].C.size(); i++) cout<<H[0][1].C[i]<<" ";
		cout<<endl;
		cout<<H[0][1].power<<endl;
		cout<<"-------------------------------"<<endl;
	
		if (H.size()==2) return H[0][1];
		if (H.size()==5) {
        return G - ( H[0][1]*H[0][2]* ( (G - H[1][2])*H[1][4]*((G - H[0][4])*H[0][3]*H[2][4]*(H[3][4] + H[1][3]*H[2][3]*(G - H[3][4])) +
		       (G - H[0][3])*H[1][3]*H[2][3]*(H[2][4] + H[0][4]*H[3][4]*(G - H[2][4]))) + H[0][3]*H[0][4]*(G - H[1][2]*H[1][3]*H[1][4]) ) +
		       H[0][1]*H[1][3]* ( (G - H[1][4])*H[0][4]*((G - H[0][3])*H[0][2]*H[3][4]*(H[2][3] + H[1][2]*H[2][4]*(G - H[2][3])) +
		       (G - H[0][2])*H[1][2]*H[2][4]*(H[3][4] + H[0][3]*H[2][3]*(G - H[3][4]))) + H[1][2]*H[1][4] ) +
		       (G - H[1][3])*H[0][1]*H[0][3]*H[1][2]* ( (G - H[0][2])*H[0][4]*H[2][3]*(H[2][4] + H[1][4]*H[3][4]*(G - H[2][4])) +
		       (G -H[1][5])*H[2][5]*H[4][5]*(H[3][4] + H[1][3]*H[3][5]*(G - H[3][4])) ) );
		}
		if (H.size()==4) {
			return H[0][1] + (G - H[0][1])*(H[1][2]*H[0][2] + H[0][3]*H[1][3] - H[1][2]*H[0][2]*H[0][3]*H[1][3] +
			       H[1][2]*H[2][3]*H[0][3]*(G - H[1][3])*( H[0][2]) + (G - H[1][2])*H[2][3]*(G - H[2][3])*H[1][3]*H[0][2]);
		}
	}
	else {
		 edge W=fedge(s,t,H);
		 vector<vector<edge>> H1(H.size());
         vector<vector<edge>> H2(H.size());
		 for (int i=0; i<H.size(); i++)  { 
			 H1[i]=H[i];
			 H2[i]=H[i];

		 }		 
		 edge F1,F2;
		 F1.C.resize(W.C.size());F1.C[0]=1;
		 F1.power=1;
         F2.C.resize(W.C.size());F2.C[1]=1;
		 F2.power=1;
         deledge (s,t,H1,W.node1,W.node2);
		 contractedge (s,t,H2,W.node1,W.node2);
		 if (gconnected(s,t,H1)!=true) cout<<"�������� ��������� ���� ������ ��������"<<endl;
		 
		 sum = F1*procedure (s, t, H1, gconnected(s,t,H1)) + F2*procedure (s, t, H2, connected);
		 return sum;
	}	
}

int main()
{
int n=0,m=0;
setlocale(LC_ALL, "");
vector<edge> E;
ifstream input("C://input.txt");
if (!input.is_open()) 
    cout << "���� �� ����� ���� ������!\n"; 
else {
char str[50];
input.getline(str,50);cout<<str<<endl;
int buf,count=0;
input>>buf;cout<<"n="<<buf<<" ";
n=buf;
input>>buf;cout<<"m="<<buf<<endl;
m=buf;
while (count!=m) {
	  bool reduct=false;
	  edge q;
	  q.ex=true;
	  q.power=1;
	  q.C.resize(m);q.C[0]=1;
      input>>buf;cout<<buf-1<<" ";
	  q.node1=buf-1;
	  input>>buf;cout<<buf-1<<endl;
	  q.node2=buf-1;
	  for (int i=0; i<E.size(); i++) if ( ((E[i].node1==q.node1)&&(E[i].node2==q.node2)) || ((E[i].node2==q.node1)&&(E[i].node1==q.node2)) ) {
		  cout<<"("<<i<<")"<<endl;
		  E[i]==E[i]+q-E[i]*q;
		  reduct=true;
	  }
	  if (reduct==false) E.push_back(q);
	  count++;
}
input>>str;cout<<str<<endl;
if (strcmp(str,"$$$")==0) cout<<"���� ������ �����"<<endl; // 0  = , 1  < , -1  >
else cout<<"���� ������ �������"<<endl;
}
input.close();

for (int i=0; i<E.size(); i++) cout<<E[i].ex<<" ";
cout<<endl<<endl;

vector<vector<edge>> S(n); 
table(S,E);
for (int i=0; i<S.size(); i++) { for (int j=0; j<S[i].size(); j++) cout<<S[i][j].ex<<" "; cout<<endl;}

if (!gconnected(1,0,S)) cout<<"��������� ���� �� �����!"<<endl;
//sum.C.resize(m);sum.power=0;
procedure(1,0,S,gconnected(1,0,S));
cout<<"�����:"<<endl;
copy( sum.C.begin(), sum.C.end(), ostream_iterator<int>(cout," ") );//��� ������
cout<<endl<<"�������:"<<sum.power<<" �������:"<<sum.node1<<","<<sum.node2<<" ���:"<<sum.ex<<endl;
//array2.front()������ ������� ������� array2
//array2.back()��������� ������� ������� array2
//array2.capacity()
//for(it=vecInt.begin();it<vecInt.end();++it)
//vecIntFirst.empty()��������� true false; vecIntFirst.empty()?cout<<"\nvecIntFirst is empty\n":cout<<"vecIntFirst is not empty\n";
//vecIntSecond.erase(vecIntSecond.begin()+2,vecIntSecond.begin()+6);
/* vector <int> vecInt(3,100); ������� ������ �� 3 ��������� � ��������� ��� ��������� 100
vector <int>::iterator it;
it = vecInt.begin() + 3;///������ ��������� �� 4 ������� (0-�������+3-��������)*/

 
cout<<endl;
cout<<sum.power<<endl;


system("pause>>void");
return 0;
}