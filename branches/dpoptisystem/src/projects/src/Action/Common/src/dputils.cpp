/*****************************************
*  DP Optimization framework             *
*  author: Artur Bak, 2016               *
******************************************/

#include <iostream>
#include <sstream>
#include <string>
#include "dputils.h"



namespace dp
{

/**
* Constructs OPP string from program arguments in main()
**/
std::string oppGetParamsFromArgs(int argc, char* argv[])
{
    std::string params="";
    for(int i = 1; i < argc; i++)
    {
      std::string arg(argv[i]);
      params.append(arg);
      if (i<argc-1)
      {
          params.append(";");
      } 
    }
    return params;
}

/**
* Extracts the value for given key from OPP string
**/
std::string oppGetValueForKey(std::string key, std::string params)
{
    std::string result = "";

    std::string token;
    std::string keyitem, value, paramscopy = params;
    while(token != paramscopy){
      token = paramscopy.substr(0,paramscopy.find_first_of(";"));
      paramscopy = paramscopy.substr(paramscopy.find_first_of(";") + 1);

      keyitem = token.substr(0,token.find_first_of("="));
      value = token.substr(token.find_first_of("=") + 1);
      if (!keyitem.compare(key))
      {
          result = value;
      }
    }

    return result;
}

}
