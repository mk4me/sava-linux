/*****************************************
*  DP Optimization framework             *
*  author: Artur Bak, 2016               *
******************************************/

/**
 * Utils functions for optimization modules
 */

#ifndef DP_UTILS_H
#define DP_UTILS_H

namespace dp 
{

/**
* Constructs OPP string from program arguments in main()
**/
std::string oppGetParamsFromArgs(int argc, char* argv[]);

/**
* Extracts the value for given key from OPP string
**/
std::string oppGetValueForKey(std::string key, std::string params);

}

#endif //DP_UTILS_H


