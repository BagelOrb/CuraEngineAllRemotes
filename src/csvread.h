// csvread.h

#ifndef CSVREAD_H
#define CSVREAD_H

#include "utils/logoutput.h"
#include <vector>
using std::vector;

namespace cura {
	
	std::vector<std::vector<std::string>> csv_read(std::string filename);
}//namespace cura

#endif//CSVREAD_H
