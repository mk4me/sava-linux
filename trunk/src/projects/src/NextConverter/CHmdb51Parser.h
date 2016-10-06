#pragma once

#ifndef HMDB51PARSER_H
#define HMDB51PARSER_H

#include <string>

class CHmdb51Parser
{
	
public:
	static void Parse(const std::string _srcVideo, const std::string _srcClassification, const std::string _dst);
	

private:
	static const std::string trainFolder;
	static const std::string testFolder;
	static const std::string splitMarker;
	

};

#endif // HMDB51PARSER_H
