#include "stdafx.h"
using namespace std;
const int n=4,m=6;
vector<bool> visited(n);

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
	edge& operator +=(edge x){ 
		for (int i=0; i<C.size(); i++) C[i]+=x.C[i];
	    power+=x.power;
	    return *this;
    }

};

edge operator *(edge x,edge y)
{    edge F;F.C.resize(m);
     F.power=x.power+y.power;
	 for (int i=0; i<x.C.size(); i++) {
		  for (int j=0; j<y.C.size(); j++) {
			  if (x.C[i]!=0&&y.C[j]!=0) F.C[i+j]+=x.C[i]*y.C[j];
		  }
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
{   edge F;F.C.resize(m); 
    F.power=max(x.power,y.power);
    edge I;I.C.resize(m);
	I.power=abs(x.power-y.power);
	for (int j=0; j<=I.power; j++) I.C[j]=Bin(I.power,I.power-j);
	if (x.power<y.power) x=x*I;
	if (y.power<x.power) y=y*I;
	for (int i=0; i<x.C.size(); i++) F.C[i]=x.C[i]+y.C[i];
	return F;
}

edge operator -(edge x,edge y)
{   edge F;F.C.resize(m); 
    F.power=max(x.power,y.power);
	edge I;I.C.resize(m);
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
	sum.C.resize(m);sum.power=0;
    for (int i=0; i<H.size(); i++) H[i].resize(n); //инициализируем все строки, так как массив ассоциативный то не нулями
    for (int i=0; i<H.size(); i++) {
	    for (int j=0; j<H[i].size(); j++) {
		    H[i][j].ex=false;
		    for (int w=0; w<E.size(); w++) {
			    if ((i==E[w].node1&&j==E[w].node2)||(i==E[w].node2&&j==E[w].node1)) {
                   H[i][j] = E[w];//надо E[w] нормально задавать 
			     }
		     }
	    }
    }
}

vector<int> gnodepower(vector<vector<edge>> &H)
{   vector<int> nodepower(H.size());
	int i=0;
    while( i<nodepower.size() ) {
		for (int j=0; j<H[i].size(); j++) {
			if (H[i][j].ex==true) nodepower[i]++;
		}
		i++;
	}
	return nodepower;
}


void DFS(vector<vector<edge>> &H,int q)
{
     visited[q]=true;
	 for (int i=0; i<H.size(); i++) for (int j=i+1; j<H[i].size(); j++) if ( (H[i][j].ex==true)&&( (!visited[i])||(!visited[j]) ) ) {
		 if (!visited[i]){
		    // cout<<i<<" ";
		     DFS(H,i);
		 }
		 else {
			// cout<<j<<" ";
			 DFS(H,j);
		 }
	 }

}
void deledge (vector<vector<edge>> &H,int q,int w)
{  
	cout<<"удалили ребро:"<<q<<" "<<w<<endl;
	edge F;F.ex=false;
    H[q][w]=F;
    H[w][q]=F;
}
void delnode (int s, int t, vector<vector<edge>> &H,int q)
{
	cout<<"Удаляемая вершина:"<<q<<endl;
	H.erase(H.begin()+q);
	for (int i=0; i<H.size(); i++) H[i].erase(H[i].begin()+q);
	visited.erase(visited.begin()+q);
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

void contractedge (int s, int t, vector<vector<edge>> &H,int q,int w)
{
	cout<<"стянули ребро:"<<q<<" "<<w<<endl;
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
void penduntreduction(int s, int t, vector<vector<edge>> &H,edge F)
{
	vector<int> nodepower=gnodepower(H);
	cout<<"Редукция висячих вершин:"<<endl;
	//cout<<"Степени вершин:";
	//for (int i=0; i<nodepower.size(); i++) cout<<nodepower[i]<<" ";
	//cout<<endl;
	vector<int>::const_iterator smallest = min_element( nodepower.begin(), nodepower.end() ); //так как в начале 0 и он будет всегда наименьшим
	//cout<<"наименьшая степень:"<<*smallest<<endl;
	if((*smallest==1)&&(H.size()>2)) {
      for (int i=0; i<H[smallest - nodepower.begin()].size(); i++) { // ищем в строке инцидентное ребро полученной врешине
		if ((H[i][smallest - nodepower.begin()].ex==true)) {  
		F=F*H[i][smallest - nodepower.begin()]; 
		break;
	    }
	  }
		delnode(s,t,H,smallest - nodepower.begin());
		//for (int i=0; i<H.size(); i++) { for (int j=0; j<H[i].size(); j++) cout<<H[i][j].ex<<" "; cout<<endl;}
		penduntreduction(s,t,H,F);
	}

}
void bridgereduction(int s,int t, vector<vector<edge>> &H)
{
	cout<<"Редукция моста!"<<endl;
	if(visited[s]&&visited[t]) for (int i=0; i<H.size(); i++) if (!visited[i]) {cout<<"удалили несвязные вершины"<<endl; delnode(s,t,H,i);}
	if((!visited[s])&&(!visited[t])) for (int i=0; i<H.size(); i++) if (visited[i]) {cout<<"удалили несвязные вершины"<<endl; delnode(s,t,H,i); }
}

edge fedge(int s,int t,vector<vector<edge>> &H)
{
	edge F;
	vector<int> nodepower=gnodepower(H);
	//cout<<"Степени вершин:";
	//for (int i=0; i<nodepower.size(); i++) cout<<nodepower[i]<<" ";
	//cout<<endl;
	//for (int i=0; i<H.size(); i++) { for (int j=0; j<H[i].size(); j++) cout<<H[i][j].ex<<" "; cout<<endl;}

	vector<int>::const_iterator smallest = min_element( nodepower.begin()+2, nodepower.end() );// возращает nodepower.end() если диапозон пуст
	//cout<<"индекс вершины с найимегьщей степенью:"<<smallest - nodepower.begin()<<endl;
	/*cout << "Наименьший элемент " << *min << endl;
      cout << "Индекс этого элемента " << min - nodepower.begin() << endl;*/
	for (int i=0; i<H[smallest - nodepower.begin()].size(); i++) {
		if ( H[i][smallest - nodepower.begin()].ex==true ) {  // ищем в строке инцидентное ребро полученной врешине
		F=H[i][smallest - nodepower.begin()]; 
		//cout<<"Разрешающее вершины :"<<i<<" "<<smallest - nodepower.begin()<<endl;
		//cout<<"Разрешающее ребро1:"<<H[i][smallest - nodepower.begin()].node1<<" "<<H[i][smallest - nodepower.begin()].node2<<endl;
		break;
	    }
	}
	//cout<<"Разрешающее ребро:"<<F.node1<<" "<<F.node2<<endl;
	return F;
}
void renumerate (int s, int t, vector<vector<edge>> &H) 
{
	edge F,G; 
	for (int i=0; i<H.size(); i++) { 
		if ((H[s][i].ex==true)&&(H[0][i].ex==true)&&(i!=s)) {  // меняем строки и стобцы для s и 0, при i=s неверно работает 
		    F=H[s][i];
		    G=H[i][s];
		    H[s][i]==H[0][i];
		    H[i][s]==H[i][0];
		    H[0][i]==F;
		    H[i][0]==G;
		}
		if ((H[t][i].ex==true)&&(H[1][i].ex==true)&&(i!=t)) { // t и 1
		    F=H[t][i];
		    G=H[i][t];
		    H[t][i]==H[1][i];
		    H[i][t]==H[i][1];
		    H[1][i]==F;
		    H[i][1]==G;
		}
	}
}
bool gconnected (int s,vector<vector<edge>> &H)
{
	 int count=0,q=s;
	 //for (int i=0; i<H.size(); i++) q=H[0][i].node1;// берем вершину из существующего ребра
	 for (int i=0; i<visited.size(); i++) visited[i]=false;
     DFS(H,q);
	 for (int i=0; i<visited.size(); i++) if (visited[i]==true) count++;
	 cout<<"Связность графа для "<<q<<" = "<<H.size()<<" "<<count<<endl;
	 if (count!=H.size()) return false;
     else return true;
}

void procedure (int s,int t,vector<vector<edge>> &H,edge F,bool connected,int count,vector<vector<edge>> &P) // F псевдоребро
{
	if (!connected) {
		bridgereduction(s,t,H);
		connected=true;
	}
	penduntreduction(s,t,H,F);
	cout<<"Размерность матрицы смежности:"<<H.size()<<endl;
	if (H.size()<3) {
		sum=F;
		cout<<"Дошли до конца"<<endl;
		for (int i=0; i<sum.C.size(); i++) cout<<sum.C[i]<<" ";
		cout<<endl;
		cout<<sum.power<<endl;
		sum.power=0;
		cout<<"-------------------------------"<<endl;

		edge F;F.C.resize(m);F.C[0]=1;
        F.power=0;
		H.clear();visited.clear();
		visited.resize(n);
		H.resize(n);for (int i=0; i<H.size(); i++) H[i].resize(n);
		for (int i=0; i<P.size(); i++) for (int j=0; j<P[i].size(); j++) H[i][j]=P[i][j];
		penduntreduction(s,t,H,F);// на связность проверили в самом начале, можно и висячие было удалить, но надо всё переделывть
		/*
		renumerate (s,t,H);
		edge G;G.C.resize(m);G.C[1]=1;G.power=0;
		if (H.size()==5) {
        sum += G - ( H[0][1]*H[0][2]* ( (G - H[1][2])*H[1][4]*((G - H[0][4])*H[0][3]*H[2][4]*(H[3][4] + H[1][3]*H[2][3]*(G - H[3][4])) +
		(G - H[0][3])*H[1][3]*H[2][3]*(H[2][4] + H[0][4]*H[3][4]*(G - H[2][4]))) + H[0][3]*H[0][4]*(G - H[1][2]*H[1][3]*H[1][4]) ) +
		H[0][1]*H[1][3]* ( (G - H[1][4])*H[0][4]*((G - H[0][3])*H[0][2]*H[3][4]*(H[2][3] + H[1][2]*H[2][4]*(G - H[2][3])) +
		(G - H[0][2])*H[1][2]*H[2][4]*(H[3][4] + H[0][3]*H[2][3]*(G - H[3][4]))) + H[1][2]*H[1][4] ) +
		(G - H[1][3])*H[0][1]*H[0][3]*H[1][2]* ( (G - H[0][2])*H[0][4]*H[2][3]*(H[2][4] + H[1][4]*H[3][4]*(G - H[2][4])) +
		(G -H[1][5])*H[2][5]*H[4][5]*(H[3][4] + H[1][3]*H[3][5]*(G - H[3][4])) ) );
		}
		if (H.size()==4) {
			sum += H[0][1] + (G - H[0][1])*(H[1][2]*H[0][2] + H[0][3]*H[1][3] - H[1][2]*H[0][2]*H[0][3]*H[1][3] +
			H[1][2]*H[2][3]*H[0][3]*(G - H[1][3])*(G - H[0][2]) + (G - H[1][2])*H[2][3]*(G - H[2][3])*H[1][3]*H[0][2]);
		}
		if (H.size()==2) { // при последовательно паралельных редукциях 3 вершинные эквивалентны 2 верш
			sum+=H[0][1];
		}*/	

		 //system("pause>>void");
	}
	else {
	     //if (count<2){
		  count++;
		  edge W=fedge(s,t,H);
          deledge (H,W.node1,W.node2); 
		  for (int i=0; i<H.size(); i++) { for (int j=0; j<H[i].size(); j++) cout<<H[i][j].ex<<" "; cout<<endl;}
		  F.power++;
		  connected=gconnected(s,H);// после удаления ребра вершина с мин степенью может стать нулевой
		  if (!connected) cout<<"несвязный получили внутри рекурсии"<<endl;
		  //system("pause>>void");
          procedure (s, t, H, F, connected,count,P);
		  //system("pause>>void");

		  W=fedge(s,t,H);
		  contractedge (s,t,H,W.node1,W.node2);
		  for (int i=0; i<H.size(); i++) { for (int j=0; j<H[i].size(); j++) cout<<H[i][j].ex<<" "; cout<<endl;}
		  //system("pause>>void");
		  F.power++;
		  connected=gconnected(s,H);
		  W=F;
		  for (int i=0; i<F.C.size()-1; i++) F.C[i+1]=W.C[i];
		  F.C[0]=0;
		  procedure (s, t, H, F, connected,count,P);
		  //system("pause>>void");
		//}
	}
	//cout<<"степень суммы:"<<sum.power<<" "<<m<<endl; // столько доходит сколко было рекурсий

	
}

int main()
{
setlocale(LC_ALL, "");
vector<edge> E;
ifstream input("C://input.txt");
if (!input.is_open()) 
    cout << "Файл не может быть открыт!\n"; 
else {
char str[50];
input.getline(str,50);cout<<str<<endl;
int buf,numnodes,numedges,count=0;
input>>buf;cout<<"n="<<buf<<" ";
numnodes=buf;
input>>buf;cout<<"m="<<buf<<endl;
numedges=buf;
while (count!=numedges) {
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
if (strcmp(str,"$$$")==0) cout<<"ввод прошёл верно"<<endl; // 0  = , 1  < , -1  >
else cout<<"ввод прошёл неверно"<<endl;
}
input.close();

for (int i=0; i<E.size(); i++) cout<<E[i].ex<<" ";
cout<<endl;

vector<vector<edge>> S(n); 
table(S,E);
for (int i=0; i<S.size(); i++) { for (int j=0; j<S[i].size(); j++) cout<<S[i][j].ex<<" "; cout<<endl;}

edge F;F.C.resize(m);F.C[0]=1;
F.power=0;
cout<<"Ответ"<<endl;
bool connected=gconnected(0,S);
if (connected==false) cout<<"несвязный граф на входе"<<endl;
cout<<"Ответ"<<endl;

vector<vector<edge>> P(n);
for (int i=0; i<P.size(); i++)  P[i]=S[i];

procedure(0,1,S,F,connected,0,P);
cout<<"Ответ"<<endl;
for (int i=0; i<sum.C.size(); i++) cout<<sum.C[i]<<" ";
cout<<endl;
cout<<sum.power<<endl;


system("pause>>void");
return 0;
}