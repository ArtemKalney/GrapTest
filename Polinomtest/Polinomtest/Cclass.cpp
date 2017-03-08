#pragma once
#include "stdafx.h"
#include "Cclass.h"

int factor(int k) {
	int l=1;
	for (int i=2; i<=k; i++) l = l*i;
	return l;
}

int Bin(int k,int l) {
    return factor(k) / (factor(l)*factor(k - l));
}

edge operator *(edge x, edge y)
{  
	edge F;
     if (!x.C.empty() && !y.C.empty()) { // for edges not zero attendance
		 if (x.C.size() == 1) return y;  // * on 1; can't ~ for 1
		 if (y.C.size() == 1) return x;
         F.C.resize(x.C.size());
		 F.power = 0;
	     F.simple = 0;
		 F.ex = true; // need for chainreduction

		 if (x.simple > 0 && x.power!=1) cout<<"Eror in Operator *"<<endl; // coudn't be becouse we contract them
		 if (y.simple > 0 && y.power!=1) cout<<"Eror in Operator *"<<endl;

	     x.power += x.simple; // paralel reduction
	     y.power += y.simple;
	     if (x.simple > 0) for (int i=0; i<x.power; i++) x.C[i] = Bin(x.power,i); 
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
	            cout<<str<<endl;
		        for (int i=0; i<x.C.size(); i++) cout<<x.C[i]<<" ";
	            cout<<endl<<"*"<<endl;;
	            for (int i=0; i<y.C.size(); i++) cout<<y.C[i]<<" ";
	            cout<<endl;
	     }
	 }
	 return F;
}

edge operator +(edge x,edge y)
{   
	edge F;
    if (!x.C.empty() && !y.C.empty()) { // for edges not zero attendance
		if (x.C.size() == 1 && y.C.size() != 1) { // need for chainreduction2
			x.C.resize(y.C.size());
			x.power = 0;
			x.simple = 0;
		}
		if (x.C.size() != 1 && y.C.size() == 1) {
			y.C.resize(x.C.size());
			y.power = 0;
			y.simple = 0;
		}
        F.C.resize(x.C.size()); 
	    F.simple = 0;
		F.ex = true; // need for chainreduction

	    if (x.simple > 0 && x.power!=1) cout<<"Eror in Operator +"<<endl;
	    if (y.simple > 0 && y.power!=1) cout<<"Eror in Operator +"<<endl;

	    x.power += x.simple; // paralel reduction
     	y.power += y.simple;
		F.power = max(x.power, y.power);
	    if (x.simple > 0) for (int i=0; i<x.power; i++) x.C[i] = Bin(x.power, i);
	    if (y.simple > 0) for (int i=0; i<y.power; i++) y.C[i] = Bin(y.power, i);
		x.simple = 0;
		y.simple = 0;
        
		if (x.power != y.power) {
            edge I;
	        I.C.resize(x.C.size());
	        I.power = abs(x.power - y.power);
	        I.simple = 0;
	        for (int j=0; j<=I.power; j++) I.C[j] = Bin(I.power, j);
	        if (x.power < y.power) x = x*I; 
	        if (y.power < x.power) y = y*I;
		}

		for (int i=0; i<F.C.size(); i++) F.C[i] = x.C[i] + y.C[i];
	}
	if (!x.C.empty() && y.C.empty()) F = x;
	if (x.C.empty() && !y.C.empty()) F = y;
	return F;
}

edge operator -(edge x,edge y)
{   
	edge F;
    if (!x.C.empty() && !y.C.empty()) { // for edges not zero attendance
		if (x.C.size() == 1 && y.C.size() != 1) { // need for chainreduction2
			x.C.resize(y.C.size());
			x.power = 0;
			x.simple = 0;
		}
		if (x.C.size() != 1 && y.C.size() == 1) {
			y.C.resize(x.C.size());
			y.power = 0;
			y.simple = 0;
		}
        F.C.resize(x.C.size()); 
	    F.simple = 0;
		F.ex = true;

	    if (x.simple > 0 && x.power!=1) cout<<"Eror in Operator -"<<endl;
	    if (y.simple > 0 && y.power!=1) cout<<"Eror in Operator -"<<endl;

	    x.power += x.simple; // paralel reduction
     	y.power += y.simple;
		F.power = max(x.power, y.power);
	    if (x.simple > 0) for (int i=0; i<x.power; i++) x.C[i] = Bin(x.power, i);
	    if (y.simple > 0) for (int i=0; i<y.power; i++) y.C[i] = Bin(y.power, i);
		x.simple = 0;
		y.simple = 0;

        if (x.power != y.power) {
            edge I;
	        I.C.resize(x.C.size());
	        I.power = abs(x.power - y.power);
	        I.simple = 0;
	        for (int j=0; j<=I.power; j++) I.C[j] = Bin(I.power, j);
	        if (x.power < y.power) x = x*I; 
	        if (y.power < x.power) y = y*I;
		}

		for (int i=0; i<F.C.size(); i++) F.C[i] = x.C[i] - y.C[i];
	}
	if (!x.C.empty() && y.C.empty()) {
		//for (int i=0; i<x.C.size(); i++) x.C[i] = -x.C[i]; 
		F = x;
	}
	if (x.C.empty() && !y.C.empty()) {
		for (int i=0; i<y.C.size(); i++) y.C[i] = -y.C[i]; // ?
		F = y;
	}
	return F;
}

edge operator ~(edge x)
{   
	edge F;
	if (!x.C.empty()) { // for edges not zero attendance
		F.C.resize(x.C.size()); 
		F.simple = 0;
		F.ex = true;

	    if (x.simple > 0 && x.power!=1) cout<<"Eror in Operator ~"<<endl;

	    x.power += x.simple; // paralel reduction
		F.power = x.power;
	    if (x.simple > 0) for (int i=0; i<x.power; i++) x.C[i] = Bin(x.power, i);

	    for (int i=0; i<=F.power; i++) F.C[i] = Bin(x.power, i) - x.C[i];
	}
	else {
		F.C.push_back(1);
	}
	return F;
}

edge operator *(int x, edge y)
{  
	 edge F;
     if (!y.C.empty()) { // for edges not zero attendance
         F = y;
         for (int i=0; i<F.C.size(); i++) F.C[i] = x*F.C[i];
	 }
	 return F;
}

edge operator *(edge x, int y)
{  
	 edge F;
     if (!x.C.empty()) { // for edges not zero attendance
         F = x;
         for (int i=0; i<F.C.size(); i++) F.C[i] = y*F.C[i];
	 }
	 return F;
}