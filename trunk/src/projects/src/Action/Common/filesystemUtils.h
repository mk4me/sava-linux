#pragma once
#ifndef filesystemUtils_h__
#define filesystemUtils_h__

#include <utils/Filesystem.h>

#include <vector>
#include <regex>

class FilesystemUtils
{
public:
	static void filterFiles(std::vector<std::string>& files)
	{
		std::vector<std::string> inFiles;
		inFiles.swap(files);

		for (std::string file : inFiles)
		{
			std::regex base_regex(".+\\.(avi)|(clu)$", std::regex::icase);
			if (std::regex_match(file, base_regex))
				files.push_back(file);
		}
	}

	static std::string getFileName(const std::string& fullPath)
	{
		std::string name = fullPath;
		std::replace(name.begin(), name.end(), '\\', '/');
		name = name.substr(name.find_last_of('/') + 1);
		name = name.substr(0, name.find_last_of('.'));
		return name;
	}
};

#endif // filesystemUtils_h__
