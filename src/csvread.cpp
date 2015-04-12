#include <iostream>
using std::cout;
using std::endl;

#include <fstream>
using std::ifstream;

#include <vector>
using std::vector;

#include <cstring>
#include <sstream>

using std::stringstream;
// #include <stringstream>
#include <string>


namespace cura {
    // const int MAX_CHARS_PER_LINE = 512;
    // const int MAX_TOKENS_PER_LINE = 20;
    // const char* const DELIMITER = " ";

    std::vector<std::string> getNextLineAndSplitIntoTokens(std::istream& str)
    {
        std::vector<std::string>   result;
        std::string                line;
        std::getline(str,line);

        std::stringstream          lineStream(line);
        std::string                cell;

        while(std::getline(lineStream,cell,','))
        {
            result.push_back(cell);
        }
        return result;
    }
    std::vector<std::vector<std::string>> csv_read( std::string filename ){
          ifstream fin;
          std::vector<std::vector<std::string> > result;
          fin.open(filename.c_str()); // open a file
          if (!fin.good()) return result; // exit if file not found
          
          // read each line of the file
          while (!fin.eof()){
            std::vector<std::string> v = getNextLineAndSplitIntoTokens(fin);
            printf("Line found %zu tokens.\n", v.size());
            result.push_back(v);
          }
            printf("Total lines found %zu.\n", result.size());
          fin.close();
        return result;
    }


}//namespace cura
