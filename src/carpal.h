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
    	int inside(int);
    	// [1, 25, 3, 0.58]
        double density;

    	int fillpattern;
    private: 
    	int id;
    	int layerID;    
};



#endif//CARPAL_H
