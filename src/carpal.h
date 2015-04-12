#include "csvread.h"


#include <vector>
using std::vector;

#include <string>


#ifndef CARPAL_H
#define CARPAL_H



class Carpal{
	public:
    	Carpal(std::vector<std::string> v);
    	void toString();
    	bool inside(int);
    	// [1, 25, 3, 0.58]
    	
    	int fillpattern;
    private: 
    	int id;
    	int layerID;
		double density;
    
};



#endif//CARPAL_H
