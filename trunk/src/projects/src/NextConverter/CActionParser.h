#pragma once

#ifndef ACTIONPARSER_H
#define ACTIONPARSER_H

#include <string>

class CActionParser
{
public:
	static void Parse(const std::string _src, const std::string _dst);
	
private:
	static const std::string acnExt;
	static const std::string cluExt;
	static const std::string vsqExt;
};

#endif // ACTIONPARSER_H
