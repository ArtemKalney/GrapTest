#include "stdafx.h"
#include "funcs.h"
#include "globals.h"
#include "Cclass.h"

void table(vector<vector<edge>> &H, vector<edge> &E) 
{
    for (int i=0; i<H.size(); i++) H[i].resize(H.size()); //initialize all strings, so as array is associative, not fill by nulls
    for (int i=0; i<H.size(); i++) for (int j=0; j<H[i].size(); j++) {
		 H[i][j].ex = false;
		 for (int w=0; w<E.size(); w++) 
		      if ((i==E[w].node1 && j==E[w].node2) || (i==E[w].node2 && j==E[w].node1)) H[i][j] = E[w];
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
     input.getline(str,50); 
     int buf, count = 0;
     input>>buf; 
     n = buf;
     input>>buf;
     m = buf;

     while (count != m) {
	       bool reduct = false;
	       edge q;
	       q.simple = 0; // filling data for new edge
	       q.ex = true;
	       q.power = 1;
	       q.C.push_back(1);
	       q.C.resize(m + 1); // numerate start from 0
           input>>buf; 
	       q.node1 = buf - 1; 
	       input>>buf; 
	       q.node2 = buf - 1;
	       for (int i=0; i<E.size(); i++) // work with multi edges
			   if ( (E[i].node1 == q.node1 && E[i].node2 == q.node2) || (E[i].node1 == q.node2 && E[i].node2 == q.node1) ) {
		          E[i].simple++;
		          reduct = true;
	           }
	       if (!reduct) E.push_back(q);
	       count++;
    }

    input>>str; // end of input
    if (strcmp(str,"$$$")!=0) cout<<"Uncorrect entry"<<endl; // 0 = ; 1 < ; -1 >
    else {
		 cout<<"Correct entry"<<endl;
/*
         vector<vector<edge>> S(n); 
         table(S, E);

		 bool r = gconnected(S);
         if (!r) cout<<"Unconnected graph on input!"<<endl;

         edge F; // create pseudo edge for computing
	     F.C.push_back(1);
	     F.C.resize(m + 1);
	     F.power = 0;
	     F.simple = 0;
         edge sum;
         
		 unsigned int start_time =  clock();
		 for (int i=0; i<S.size() - 1; i++) for (int j=i + 1; j<S[i].size(); j++) {
			 if (i!=0 || j!=1) { // after this 0,1
                 if (i!=0 && j!=1) {
                 renumerate (S, i, 0);
                 renumerate (S, j, 1);
                 }
                 if (i == 0 && j != 1) renumerate (S, j, 1);
                 if (i != 0 &&j == 1) renumerate (S, i, 0);
             } 
			 sum = sum + procedure(S, F, r);
		 }
		 unsigned int end_time = clock();
		 double search_time = end_time - start_time;
	     cout<<"Time of programm:"<<search_time/1000<<" sec"<<endl;

		 cout<<"Solution:"<<endl;
		 for (int i=0; i<sum.C.size(); i++) sum.C[i] = sum.C[i]/Bin(S.size() - 1, 2);
         copy( sum.C.begin(), sum.C.end(), ostream_iterator<int>(cout," ") );
         cout<<endl<<"Power:"<<sum.power<<" "<<"Simple:"<<sum.simple<<endl;
		 double value = 0, p = 0.9, z = 0.1;
		 int q = sum.power;
		 for (int i=0; i<=q; i++) {
			  value += sum.C[i]*pow(p, q - i)*pow(z, i);
		 }
		 cout<<"Value at point "<<p<<": "<<value<<endl;
*/

		 int s, t;
		 cout<<"Entry nodes:"<<endl;
		 cout<<"s=";
		 cin>>s; s--; // numerate start from 0
		 cout<<"t=";
		 cin>>t; t--;
         if (s!=t && s<t) { 

             vector<vector<edge>> S(n); 
             table(S, E);

	         if (s!=0 || t!=1) { // after this 0,1
                 if (s!=0 && t!=1) {
                 renumerate (S, s, 0);
                 renumerate (S, t, 1);
                 }
                 if (s==0 && t!=1) renumerate (S, t, 1);
                 if (s!=0 && t==1) renumerate (S, s, 0);
             } 
             
			 bool r = gconnected(S);
             if (!r) cout<<"Unconnected graph on input!"<<endl;

             edge F; // create pseudo edge for computing
	         F.C.push_back(1);
			 F.C.resize(m + 1);
			 F.power = 0;
			 F.simple = 0;

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

    }
}
input.close();

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