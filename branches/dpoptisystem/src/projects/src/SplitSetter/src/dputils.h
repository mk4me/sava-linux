//
// Created by mc on 1/19/17.
//

#ifndef EDR_UTILS_DPUTILS_H
#define EDR_UTILS_DPUTILS_H

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

#endif //EDR_UTILS_DPUTILS_H
