#include "stdafx.h"
using namespace std;
vector<bool> visited;


struct edge    
{   
    vector<int> C; 
	int power;
	int node1;
	int node2;
	int simple;
	bool ex;

	edge& operator ==(edge x){
	C=x.C;
	ex=true;
	power=x.power;
	return *this;
    }
};

edge operator *(edge x,edge y)
{    edge F;F.C.resize(x.C.size());
     try {
         F.power = x.power + y.power;	 
	     for (int i=0; i<x.C.size(); i++) for (int j=0; j<y.C.size(); j++) {
			 if (x.C[i]!=0&&y.C[j]!=0) {
				 if (i+j >= x.C.size())  throw "Ошибка - Вектор переполенен!";
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
    for (int i=0; i<H.size(); i++) H[i].resize(H.size()); //инициализируем все строки, так как массив ассоциативный то не нулями
    for (int i=0; i<H.size(); i++) {
	    for (int j=0; j<H[i].size(); j++) {
		    H[i][j].ex=false;
		    for (int w=0; w<E.size(); w++) {
				//if ((i==E[w].node1&&j==E[w].node2)||(i==E[w].node2&&j==E[w].node1)) H[i][j] = E[w];

               
				if (i==E[w].node1 && j==E[w].node2) H[i][j] = E[w];
				if (i==E[w].node2 && j==E[w].node1) {
					int q = E[w].node2;
					E[w].node2 = E[w].node1;
                    E[w].node1 = q;
					H[i][j] = E[w];
				}
		        
		     }
	    }
    }
}

vector<int> gnodepower( vector<vector<edge>> &H)
{   vector<int> nodepower(H.size());
	int i=0; 
    while( i < H.size() ) {
		for (int j=0; j<H[i].size(); j++) if ( H[i][j].ex==true ) nodepower[i]++;
		i++;
	}
	return nodepower;
}


void DFS(vector<vector<edge>> &H,vector<bool> &visited, int q)
{
     visited[q]=true;
	 for (int i=0; i<H.size(); i++) for (int j=i+1; j<H[i].size(); j++) if ( (H[i][j].ex==true)&&( (!visited[i])||(!visited[j]) ) ) {
		 if (!visited[i]) DFS(H,visited,i);
		 else DFS(H,visited,j);
	 }

}

bool gconnected (vector<vector<edge>> &H)
{
	 int count=0;
	 visited.resize(H.size());
	 for (int i=0; i<visited.size(); i++) visited[i]=false;
     DFS(H,visited,0);
	 for (int i=0; i<visited.size(); i++) if (visited[i]==true) count++;
	 cout<<"Связность графа для "<<"0"<<" = "<<H.size()<<" >= "<<count<<endl;
	 if (count!=H.size()) return false;
     else return true;
}

void delnode ( vector<vector<edge>> &H,int q)
{
	cout<<"Удаляемая вершина:"<<q<<endl;
	H.erase(H.begin()+q);
	for (int i=0; i<H.size(); i++) H[i].erase(H[i].begin()+q);
}

void deledge (vector<vector<edge>> &H,int q,int w) //при удалении ребра может возникнуть нулевая вершина
{  
	//cout<<"удалили ребро:"<<q<<" "<<w<<endl;
	edge F;F.ex = false;
    H[q][w]=F;
    H[w][q]=F;
	//vector<int> nodepower=gnodepower(s,t,H);  //не произойдёт так как есть penduntreduction
	//vector<int>::iterator smallest = min_element( nodepower.begin(), nodepower.end() );
	//if (*smallest==0) delnode(s,t,H,smallest - nodepower.begin());
}

void contractedge ( vector<vector<edge>> &H,int q,int w)
{
	//cout<<"стянули ребро:"<<q<<" "<<w<<endl;
	
	if (w==0||w==1) { // если в разрешающее ребро попадает 0 или 1
		int r=q;
		q=w;
		w=r;
	}
	
    for (int i=0; i<H[w].size(); i++) if (H[w][i].ex && i!=q) { //??????
		if (H[q][i].ex) {
			H[q][i] == H[w][i]+H[q][i]-H[w][i]*H[q][i];
			H[i][q] == H[i][w]+H[i][q]-H[i][w]*H[i][q];
		}
		if (!H[q][i].ex) {
			 H[q][i] == H[w][i];
		     H[i][q] == H[i][w];
		}
	} 
	delnode (H,w);
}

void renumerate (int s, int t, vector<vector<edge>> &H) 
{

	cout<<"renumerate:"<<s<<" "<<t<<endl;
	if (s!=t) {
		vector<edge> F,G;
		F=H[s];
		G=H[t];

		for (int i=0; i<H.size(); i++) if (i!=s && i!=t) swap(F[i],G[i]);
	
		for (int i=0; i<H.size(); i++) { //очищаем строки и стольцы для вершин s t
			deledge(H,t,i);
            deledge(H,s,i);
		}
	    
		for (int i=0; i<H.size(); i++) { // заполняем
	         if ( F[i].ex && i!=s) {
			     H[s][i] == F[i];
		         if (i!=t) H[i][s] == F[i]; // чтобы на чужую строку не залезть
			 }
			 if ( G[i].ex && i!=t) {
			     H[t][i] == G[i];
		         if (i!=s) H[i][t] == G[i];
			 }
	    }
	}
	for (int i=0; i<H.size(); i++) { for (int j=0; j<H[i].size(); j++) cout<<H[i][j].ex<<" "; cout<<endl;}
    
}

edge penduntreduction( vector<vector<edge>> &H, edge F)
{
	int s,t;
	vector<int> nodepower = gnodepower(H);
	cout<<"Редукция висячих вершин:"<<endl;
    vector <int>::iterator it,smallest=nodepower.begin();
    for(it=nodepower.begin();it<nodepower.end();++it) if (*it == 1) smallest = it;
    int r = smallest - nodepower.begin();

	if ( (*smallest==1) && (H.size()>2) ) {         //иначе всё удалим
       for (int i = H[r].size() - 1; i >= 0; i--) if (H[r][i].ex) { //находим инцедентное ребро 
	       F.C.resize(H[r][i].C.size());
	       F == F*H[r][i];
	       cout<<"Ребро:"<<r<<","<<i<<endl;
	       if (r!=0 && r!=1) delnode(H,r);
	       else {
		        if (r==0) {
			        s=i;
			        delnode(H,r);
			        if (s==H.size()) s--;
			        renumerate (s,0,H); 
		        }              
		        if (r==1) {
			        t=i;
			        delnode(H,r);
			        if (t==H.size()) t--;
			        renumerate (t,1,H); 
		        }
	       }
		   break;
	   }
	   return penduntreduction(H,F);
	}
	return F;

}
void bridgereduction( vector<vector<edge>> &H)
{
	cout<<"Редукция моста!"<<endl;
	if(visited[0]&&visited[1]) for (int i=0; i<H.size(); i++) if (!visited[i]) {cout<<"Удалили несвязную вершину:"<<i<<endl; delnode(H,i); }
	if((!visited[0])&&(!visited[1])) for (int i=0; i<H.size(); i++) if (visited[i]) {cout<<"Удалили несвязную вершину:"<<i<<endl; delnode(H,i); }
}

edge fedge(vector<vector<edge>> &H)
{
	edge F;
	vector<int> nodepower=gnodepower(H);
	nodepower[0]=H.size();nodepower[1]=H.size();// костыль
	vector <int>::iterator it,smallest=nodepower.begin();
    for(it=nodepower.begin();it<nodepower.end();++it) if (*it <= *smallest) smallest = it;
    int r = smallest - nodepower.begin();
	/*cout << "Наименьший элемент " << *smallest << endl;
      cout << "Индекс этого элемента " << smallest - nodepower.begin() << endl;*/
	// for (int i = H[r].size() - 1; i >= 0; i--)
	// for (int i = 0; i < H[r].size(); i++)
	 for (int i = H[r].size() - 1; i >= 0; i--) if ( H[r][i].ex ) {  // ищем в строке инцидентное ребро полученной врешине может быть 0 или 1
		F == H[r][i]; 
		F.node1=r;
		F.node2=i;
		break;
	}
	return F;
}

edge procedure (vector<vector<edge>> &H, bool connected) 
{
	cout<<"Размерность матрицы смежности до редкукции:"<<H.size()<<endl;
    for (int i=0; i<H.size(); i++) { for (int j=0; j<H[i].size(); j++) cout<<H[i][j].ex<<" "; cout<<endl;}

	if (connected!=true) {
		bridgereduction(H);
		connected=true;
	}

	edge F;
	F.C.push_back(1);
	F.power=0;
	F == penduntreduction(H,F);
    cout<<"вот F:"<<endl;
	copy( F.C.begin(), F.C.end(), ostream_iterator<int>(cout," ") );
	cout<<endl<<F.power<<endl;
    
	cout<<"Размерность матрицы смежности после редукции:"<<H.size()<<endl;
    for (int i=0; i<H.size(); i++) { for (int j=0; j<H[i].size(); j++) cout<<H[i][j].ex<<" "; cout<<endl;}

	if (H.size()<3) {
		edge G;
		cout<<"Дошли до конца"<<endl;
		edge ed;
		cout<<"вот H[0][1]:"<<endl;
	    copy( H[0][1].C.begin(), H[0][1].C.end(), ostream_iterator<int>(cout," ") );
	    cout<<endl<<H[0][1].power<<endl;

	    if (F.C.size()>=2) ed == F*H[0][1];
		if ( (H.size()==2) && (F.C.size()>=2) ) { for (int i=0; i<ed.C.size(); i++) cout<<ed.C[i]<<" "; cout<<endl<<ed.power<<endl;}
		else { for (int i=0; i<H[0][1].C.size(); i++) cout<<H[0][1].C[i]<<" "; cout<<endl<<H[0][1].power<<endl;}

		cout<<"------------------------------------------"<<endl;
	
		if ( (H.size()==2) && (F.C.size()>=2) ) return F*H[0][1];
		else return H[0][1];

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
		 edge W=fedge(H);
		 cout<<"Разрешающее ребро:"<<W.node1<<" "<<W.node2<<endl;
		 edge F1,F2;
		 vector<vector<edge>> H1(H.size());
         vector<vector<edge>> H2(H.size());
		 for (int i=0; i<H.size(); i++)  { 
			 H1[i]=H[i];
			 H2[i]=H[i];
		 }	
		 F1.C.push_back(1);
		 F2.C.push_back(0);
		 F2.C.push_back(1);
		 F1.C.resize(W.C.size());
		 F2.C.resize(W.C.size());
		 F1.power=1;
		 F2.power=1;
		 if (F.C.size()>=2) {
		    F1==F*F1;
		    F2==F*F2;
		 }

         cout<<"вот F1:"<<endl;
	     copy( F1.C.begin(), F1.C.end(), ostream_iterator<int>(cout," ") );
	     cout<<endl<<F1.power<<endl;

		 cout<<"вот F2:"<<endl;
	     copy( F2.C.begin(), F2.C.end(), ostream_iterator<int>(cout," ") );
	     cout<<endl<<F2.power<<endl;

         deledge (H1,W.node1,W.node2);
		 contractedge (H2,W.node1,W.node2);
		 bool r = gconnected(H1);
		 if (!r) cout<<"получили несвязный граф внутри рекурсии"<<endl;

        // system("pause>>void");
		 sum = F1*procedure ( H1, r) + F2*procedure ( H2, connected);
			 //F1*procedure ( H1, r) + F2*procedure ( H2, connected);
		 return sum;
	}	
}

int main()
{
setlocale(LC_ALL, "");
int n=0,m=0;
vector<edge> E;
ifstream input("C://input.txt");
if (!input.is_open()) 
    cout << "Файл не может быть открыт!\n"; 
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
	  q.simple=0;
	  q.ex=true;
	  q.power=1;
	  q.C.push_back(1);
	  q.C.resize(m);
      input>>buf;cout<<buf-1<<" ";
	  q.node1=buf-1;
	  input>>buf;cout<<buf-1<<endl;
	  q.node2=buf-1;
	  for (int i=0; i<E.size(); i++) if ( ((E[i].node1==q.node1)&&(E[i].node2==q.node2)) || ((E[i].node1==q.node2)&&(E[i].node2==q.node1)) ) {
		  cout<<"("<<i<<")с вершинами "<<E[i].node1<<","<<E[i].node2<<endl;
		  E[i]==E[i]+q-E[i]*q;
		  //cout<<E[i].power<<endl;
		  //E[i].simple++;
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
cout<<endl<<endl;

vector<vector<edge>> S(n); 
table(S,E);
for (int i=0; i<S.size(); i++) { for (int j=0; j<S[i].size(); j++) cout<<S[i][j].ex<<" "; cout<<endl;}

int s=1,t=3; 
if (s!=t) {
	if (s!=0 && t!=1) { // после этого 0 1
       if (s!=1 && t!=0) {
       renumerate (s,0,S);
       renumerate (t,1,S);
       }
       if (s==1 && t!=0) {
       renumerate (s,0,S);
       renumerate (t,0,S);
       }
       if (s!=1 && t==0) {
       renumerate (s,0,S);
       renumerate (t,1,S);
       }
       if (s==1 && t==0) renumerate (s,t,S);
    }

   for (int i=0; i<S.size(); i++) { for (int j=0; j<S[i].size(); j++) cout<<S[i][j].ex<<" "; cout<<endl;}

   bool r=gconnected(S);
   if (!r) cout<<"несвязный граф на входе!"<<endl;

   procedure(S,r);
   cout<<"Ответ:"<<endl;
   copy( sum.C.begin(), sum.C.end(), ostream_iterator<int>(cout," ") );//как пример
   cout<<endl<<"Степень:"<<sum.power<<endl;
}
/*
array2.front()Первый элемент массива array2
array2.back()Последний элемент массива array2
array2.capacity()
for(it=vecInt.begin();it<vecInt.end();++it)
vecIntFirst.empty()возрашает true false; vecIntFirst.empty()?cout<<"\nvecIntFirst is empty\n":cout<<"vecIntFirst is not empty\n";
vecIntSecond.erase(vecIntSecond.begin()+2,vecIntSecond.begin()+6);
vector <int> vecInt(3,100); Создаем вектор из 3 элементов и заполняем его значением 100
vector <int>::iterator it;
it = vecInt.begin() + 3;///Вектор указывает на 4 элемент (0-элемент+3-элемента)
vecFirst.swap(vecSecond);
swap (vecFirst.back(),vecSecond.back());
"условие" ? "выражение 1" : "выражение 2";
sort(numbers.begin(), numbers.end());
vector<int>::const_iterator five = lower_bound(numbers.begin(), numbers.end(), 5);
numbers.erase(remove_if(numbers.begin(), numbers.end(), bind2nd(greater<int>(), 4)), numbers.end() ); // Удаляем все элементы больше 4-х 
*/


system("pause>>void");
return 0;
}