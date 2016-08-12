/** Copyright (C) 2013 David Braam - Released under terms of the AGPLv3 License */
#include "carpal.h"

Carpal::Carpal(std::vector<std::string> v){

	// printf("id:%s layer:%s fill:%s den: %s\n", v[0].c_str(), v[1].c_str(), v[2].c_str(), v[3].c_str());
	this->id = atoi(v[0].c_str()); 
   	this->layerID = atoi(v[1].c_str());
   	this->fillpattern = atoi(v[2].c_str());
   	char *ptr;
	this->density = std::strtod(v[3].c_str(), &ptr);
}

void Carpal::toString(){
	printf("id:%d layer:%d fill:%d den: %f\n", this->id, this->layerID, this->fillpattern, this->density);
}
int Carpal::inside(int layer){
	
	if(layer < this->layerID){
		int diff = std::abs(layer - this->layerID);
		if(diff > 5) return 1; // normal inside
		else return 2; // solid layer
	}
	else
		return -1; //outside
}