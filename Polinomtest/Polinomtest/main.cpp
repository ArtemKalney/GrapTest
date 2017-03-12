#include "stdafx.h"
#include "funcs.h"
#include "globals.h"
#include "Cclass.h"

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

int main()
{
setlocale(LC_ALL, "");
ofstream output("C://output.txt");
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
	     F.C.resize(m + 1);
	     F.power = 0;
	     F.simple = 0;
		 F.ex = true;

         vector<int> weight(S.size()), V;
		 for (int i=0; i<weight.size(); i++) weight[i] = 1;

		 edge sum;
		 mask1.resize(S.size()); 
		 for (int i=0; i<mask1.size(); i++) { 
			  mask1[i].resize(S[i].size());
			  for (int j=0; j<mask1[i].size(); j++) i != j ? mask1[i][j] = true : mask1[i][j] = false;
		 }

		 unsigned int start_time = clock();
		 sum = chainreduction1(S, sum, V, F, r);
         
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
		 int search_time = end_time - start_time;
	     cout<<"Time of programm:"<<search_time<<" msec"<<endl;

		 cout<<"R:"<<endl;
         copy( sum.C.begin(), sum.C.end(), ostream_iterator<int>(cout," ") );
         cout<<endl<<"Power:"<<sum.power<<" "<<"Simple:"<<sum.simple<<endl;
		 value = 0;
		 q = sum.power;
		 for (int i=0; i<=q; i++) {
			  value += sum.C[i]*pow(p, q - i)*pow(z, i);
		 }
		 cout<<"Value at point "<<p<<": "<<value/Bin(S.size(), 2)<<endl;

/*
		 edge sum;
		 mask.resize(S.size()); 
		 for (int i=0; i<mask.size(); i++) i == 0 ? mask[i] = false : mask[i] = true;

		 unsigned int start_time = clock();
		 sum = chainreduction2(S, sum, weight, F, r);
         
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
		 int search_time = end_time - start_time;
	     cout<<"Time of programm:"<<search_time<<" msec"<<endl;

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
*/
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
			 int search_time = end_time - start_time;
			 cout<<"Time of programm:"<<search_time<<" msec"<<endl;

             cout<<"Solution:"<<endl;
             copy( sum.C.begin(), sum.C.end(), ostream_iterator<int>(cout," ") );
             cout<<endl<<"Power:"<<sum.power<<" "<<"Simple:"<<sum.simple<<endl;
			 cout<<"Were ends of recursion:"<<num<<endl;
			 cout<<" 2-dimension graph:"<<num2<<endl;
			 cout<<" 3-dimension graph:"<<num3<<endl;
			 cout<<" 4-dimension graph:"<<num4<<endl;
			 cout<<" 5-dimension graph:"<<num5<<endl;
			 cout<<"Were special chains:"<<ch<<endl;
			 cout<<" chains reduced:"<<ch - ch1 - ch2<<endl;
			 cout<<" 1-st type:"<<ch1<<endl;
			 cout<<" 2-nd type:"<<ch2<<endl;
			 
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