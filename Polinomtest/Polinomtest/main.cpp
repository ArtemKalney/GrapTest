#include "stdafx.h"
#include "funcs.h"
#include "globals.h"
#include "Cclass.h"

ofstream output("C://output.txt");
ifstream input("C://input.txt");

int num0 = 0, num2 = 0, num3 = 0, num4 = 0, num5 = 0, chr = 0, ch1 = 0, ch2 = 0, chrs = 0,
    decomp1 = 0, decomp2 = 0, decomp3 = 0, bridges = 0, pendunt = 0, factors = 0;
vector<bool> visited;
vector<vector<bool>> mask1;
vector<bool> mask;
vector<edge> Bin;

void table(vector<vector<edge>> &H, vector<edge> &E)
{
	for (int i = 0; i<H.size(); i++) H[i].resize(H.size());
	//initialize all strings, so as array is associative, not fill by nulls, or it is not global
	for (int i = 0; i<H.size(); i++) for (int j = 0; j<H[i].size(); j++) {
		H[i][j].ex = false;
		for (int w = 0; w<E.size(); w++)
			if ((i == E[w].node1 && j == E[w].node2) || (i == E[w].node2 && j == E[w].node1)) H[i][j] = E[w];
	}
}

int main()
{
	setlocale(LC_ALL, "");
	if (!input.is_open()) cout << "File can not be opened!\n";
	else {
		char str[50];
		int n = 0, m = 0;
		vector<edge> E;
		input.getline(str, 50); output << str << endl;
		int buf, count = 0;
		input >> buf; output << "Number of nodes n=" << buf << endl;
		n = buf;
		input >> buf; output << "Number of edges m=" << buf << endl;
		m = buf;

		while (count != m) {
			bool reduct = false;
			edge q; // filling data for new edge
			q.simple = 0;
			q.ex = true;
			q.power = 1;
			q.C.push_back(1);
			q.C.resize(m + 1); // numerate start from 0
			input >> buf; output << buf - 1 << " ";
			q.node1 = buf - 1;
			input >> buf; output << buf - 1 << endl;
			q.node2 = buf - 1;
			for (int i = 0; i<E.size(); i++) // work with multi edges
				if ((E[i].node1 == q.node1 && E[i].node2 == q.node2) || (E[i].node1 == q.node2 && E[i].node2 == q.node1)) {
					E[i].simple++;
					output << "E[" << i << "].simple=" << E[i].simple << endl;
					reduct = true;
				}
			if (!reduct) E.push_back(q);
			count++;
		}

		input >> str; output << str << endl; // end of input
		if (strcmp(str, "$$$") != 0) cout << "Uncorrect entry" << endl; // 0 = ; 1 < ; -1 >
		else {
			cout << "Correct entry" << endl;

			Bin.resize(m + 1); // max power is m-1
			for (int i = 0; i<Bin.size(); i++) {
				Bin[i].C.resize(m + 1);
				Bin[i].power = i;
				Bin[i].simple = 0;
				Bin[i].C.front() = 1;
				if (i != 0) for (int j = 1; j<m + 1; j++) Bin[i].C[j] = Bin[i - 1].C[j - 1] + Bin[i - 1].C[j];
			}

			output << "Bin:" << endl;
			for (int i = 0; i<Bin.size(); i++) {
				for (int j = 0; j<Bin[i].C.size(); j++) output << Bin[i].C[j] << " ";
				output << endl << "power = " << Bin[i].power << ", simple = " << Bin[i].simple << endl;
			}

			output << "Simple edges:" << endl;
			for (int i = 0; i<E.size(); i++) output << E[i].simple << " ";
			output << endl;

			vector<vector<edge>> S(n);
			table(S, E);
			output << "Adjacency matrix:" << endl;
			for (int i = 0; i<S.size(); i++) { for (int j = 0; j<S[i].size(); j++) output << S[i][j].ex << " "; output << endl; }

			bool r = gconnected(S);
			vector<bool> visited1 = visited;
			if (!r) cout << "Unconnected graph on input!" << endl;

			edge F = Bin.front(); // create pseudo edge for computing

			vector<int> weight(S.size()), V;
			for (int i = 0; i<weight.size(); i++) weight[i] = 1;


			edge sum;
			mask1.resize(S.size());
			for (int i = 0; i<mask1.size(); i++) {
				mask1[i].resize(S[i].size());
				for (int j = 0; j<mask1[i].size(); j++) i != j ? mask1[i][j] = true : mask1[i][j] = false;
			}

			unsigned int start_time = clock();
			sum = chainreduction1(S, sum, V, F, r);

			cout << "chainreduction:" << endl;
			sum.printedge();

			double value = 0, p = 0.9, z = 0.1;
			int q = sum.power;
			for (int i = 0; i < q; i++) value += sum.C[i] * pow(p, q - i)*pow(z, i);
			cout << "Value at point " << p << ": " << value / Bin[S.size()].C[2] << endl;

			for (int i = 0; i<S.size(); i++) for (int j = i + 1; j<S[i].size(); j++) if (mask1[i][j]) {
				cout << "+R" << i + 1 << j + 1 << endl;
				vector<vector<edge>> G(S.size());
				for (int i = 0; i<S.size(); i++) G[i] = S[i];
				if (i != 0 || j != 1) { // after this 0,1
					if (i != 0 && j != 1) {
						renumerate(G, i, 0);
						renumerate(G, j, 1);
					}
					if (i == 0 && j != 1) renumerate(G, j, 1);
					if (i != 0 && j == 1) renumerate(G, i, 0);
				}
				sum = sum + procedure(G, F, true); // G changed
			}
			unsigned int end_time = clock();
			int search_time = end_time - start_time;
			cout << "Time of programm " << search_time << " msec" << endl;
			cout << "Call prosedure " << factors << endl;
			cout << "Reductions : " << endl;
			cout << " reduced chains initially " << chrs << endl;
			cout << " bridge reductions " << bridges << endl;
			cout << " pendunt reducutions " << pendunt << endl;
			cout << " chains reduced " << chr << endl;
			cout << " 1-st type " << ch1 << endl;
			cout << " 2-nd type " << ch2 << endl;
			cout << " decompositions in chainreduction " << decomp1 << endl;
			cout << " decompositions in chr " << decomp2 << endl;
			cout << " decompositions in factoring " << decomp3 << endl;
			cout << "Were ends of recursion : " << num0 + num2 + num3 + num4 + num5 << endl;
			cout << " unconnected graphs " << num0 << endl;
			cout << " 2-dimension graphs " << num2 << endl;
			cout << " 3-dimension graphs " << num3 << endl;
			cout << " 4-dimension graphs " << num4 << endl;
			cout << " 5-dimension graphs " << num5 << endl;

			cout << "R:" << endl;
			sum.printedge();

			value = 0;
			q = sum.power;
			for (int i = 0; i < q; i++) value += sum.C[i] * pow(p, q - i)*pow(z, i);
			cout << "Value at point " << p << ": " << fixed << setprecision(10) << value / Bin[S.size()].C[2] << endl;

/*
			edge sum;
			mask.resize(S.size());
			for (int i=0; i<mask.size(); i++) i == 0 ? mask[i] = false : mask[i] = true;
			unsigned int start_time = clock();
			sum = chainreduction2(S, sum, weight, F, r);

			cout<<"chainreduction:"<<endl;
			sum.printedge();

			double value = 0, p = 0.9, z = 0.1;
			int q = sum.power;
			for (int i=0; i<q; i++) value += sum.C[i]*pow(p, q - i)*pow(z, i);
			cout<<"Value at point "<<p<<": " << value <<endl;

			for (int i = 0; i < S.size(); i++) if (mask[i]) {
				cout << "+R1" << i + 1 << endl;
				vector<vector<edge>> G(S.size());
				for (int j = 0; j < S.size(); j++) G[j] = S[j];
				if (i != 1) renumerate(G, i, 1); // after this i=1
				sum = sum + procedure(G, F, true); // G changed
			}
			unsigned int end_time = clock();
			int search_time = end_time - start_time;
			cout << "Time of programm " << search_time << " msec" << endl;
			cout << "Call prosedure " << factors << endl;
			cout << "Reductions : " << endl;
			cout << " reduced chains initially " << chrs << endl;
			cout << " bridge reductions " << bridges << endl;
			cout << " pendunt reducutions " << pendunt << endl;
			cout << " chains reduced " << chr << endl;
			cout << " 1-st type " << ch1 << endl;
			cout << " 2-nd type " << ch2 << endl;
			cout << " decompositions in chainreduction " << decomp1 << endl;
			cout << " decompositions in chr " << decomp2 << endl;
			cout << " decompositions in factoring " << decomp3 << endl;
			cout << "Were ends of recursion : " << num0 + num2 + num3 + num4 + num5 << endl;
			cout << " unconnected graphs " << num0 << endl;
			cout << " 2-dimension graphs " << num2 << endl;
			cout << " 3-dimension graphs " << num3 << endl;
			cout << " 4-dimension graphs " << num4 << endl;
			cout << " 5-dimension graphs " << num5 << endl;

			cout<<"C:"<<endl;
			sum.printedge();

			value = 0;
			q = sum.power;
			for (int i=0; i<q; i++) value += sum.C[i]*pow(p, q - i)*pow(z, i);
			cout<<"Value at point "<<p<<": "<< fixed << setprecision(10) << value + 1 <<endl; // + R11=1
*/
/*
			int s, t;
			cout<<"Entry nodes:"<<endl;
			cout<<"s=";
			cin>>s; s--; // numerate start from 0
			cout<<"t=";
			cin>>t; t--;
			if (s != t && s < t) {
				if (s != 0 || t != 1) { // after this 0,1
					if (s != 0 && t != 1) {
						renumerate(S, s, 0);
						renumerate(S, t, 1);
					}
					if (s == 0 && t != 1) renumerate(S, t, 1);
					if (s != 0 && t == 1) renumerate(S, s, 0);
				}
				r = gconnected(S);

				unsigned int start_time = clock();
				edge sum = procedure(S, F, true);
				unsigned int end_time = clock();
				int search_time = end_time - start_time;
				cout << "Time of programm:" << search_time << " msec" << endl;

				cout << "Solution:" << endl;
				sum.printedge();
				cout << "Time of programm " << search_time << " msec" << endl;
			cout << "Time of programm " << search_time << " msec" << endl;
			cout << "Call prosedure " << factors << endl;
			cout << "Reductions : " << endl;
			cout << " reduced chains initially " << chrs << endl;
			cout << " bridge reductions " << bridges << endl;
			cout << " pendunt reducutions " << pendunt << endl;
			cout << " chains reduced " << chr << endl;
			cout << " 1-st type " << ch1 << endl;
			cout << " 2-nd type " << ch2 << endl;
			cout << " decompositions in chainreduction " << decomp1 << endl;
			cout << " decompositions in chr " << decomp2 << endl;
			cout << " decompositions in factoring " << decomp3 << endl;
			cout << "Were ends of recursion : " << num0 + num2 + num3 + num4 + num5 << endl;
			cout << " unconnected graphs " << num0 << endl;
			cout << " 2-dimension graphs " << num2 << endl;
			cout << " 3-dimension graphs " << num3 << endl;
			cout << " 4-dimension graphs " << num4 << endl;
			cout << " 5-dimension graphs " << num5 << endl;

				double value = 0, p = 0.9, z = 0.1;
				int q = sum.power;
				for (int i = 0; i < q; i++) value += sum.C[i] * pow(p, q - i)*pow(z, i);
				cout << "Value at point " << p << ": " << fixed << setprecision(15) << value << endl;
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
	copy( F.C.begin(), F.C.end(), ostream_iterator<int>(output, " ") );
	*/

	system("pause>>void");
	return 0;
}