#include "PipeProcessUtils.h"

namespace utils
{
	bool PipeProcessUtils::getFirstFile(FilesystemPath& outPath, const FilesystemPath& directory, const std::regex& matchPattern)
	{
		boost::filesystem::directory_iterator endIt;
		for (boost::filesystem::directory_iterator it(directory); it != endIt; ++it)
		{
			try
			{
				if (!boost::filesystem::exists(*it) || !boost::filesystem::is_regular_file(*it))
					continue;
			}
			catch (const boost::filesystem::filesystem_error&) { continue; }

			if (!std::regex_match(it->path().string(), matchPattern))
				continue;

			outPath = *it;
			return true;
		}
		return false;
	}

	bool PipeProcessUtils::getFirstFile(FilesystemPath& outPath, const FilesystemPath& directory, const std::string& extension, unsigned numIndices)
	{
		std::string pattern = ".+\\.";
		for (unsigned i = 0; i < numIndices; ++i)
		{
			pattern += "\\d+\\.";
		}
		pattern += extension;
		pattern += "$";

		return getFirstFile(outPath, directory, std::regex(pattern, std::regex::icase));
	}
	void PipeProcessUtils::getFilesIndices(IndicesVectorSet& outFiles, const IndicesVector& pattern, const FilesystemPath& directory, const std::string& filePattern, const std::string& extension, unsigned numIndices)
	{
		assert(pattern.size() <= numIndices);
		std::string p = ".*" + filePattern + "\\.";
		for (unsigned i = 0; i < numIndices; ++i)
		{
			p += "(";
			if (i < pattern.size() && pattern[i] != NO_INDEX)
			{
				p += std::to_string(pattern[i]);
			}
			else
			{
				p += "\\d+";
			}
			p += ")\\.";
		}
		p += extension;
		p += "$";
		std::regex rx(p, std::regex::icase);

		boost::filesystem::directory_iterator endIt;
		for (boost::filesystem::directory_iterator it(directory); it != endIt; ++it)
		{
			try
			{
				if (!boost::filesystem::exists(*it) || !boost::filesystem::is_regular_file(*it))
					continue;
			}
			catch (const boost::filesystem::filesystem_error&) { continue; }

			std::smatch match;
			std::string search = it->path().string();
			if (!std::regex_search(search, match, rx))
				continue;

			IndicesVector indices;
			for (size_t i = 0; i < pattern.size(); ++i)
			{
				indices.push_back(std::stoi(match[i + 1]));
			}
			outFiles.insert(indices);
		}
	}
	
	void PipeProcessUtils::getFilesIndices(IndicesSet& outFiles, const FilesystemPath& directory, const std::string& filePattern, const std::string& extension)
	{
		std::string p = ".*" + filePattern + "\\.(\\d+)\\." + extension + "$";
		std::regex rx(p, std::regex::icase);

		boost::filesystem::directory_iterator endIt;
		for (boost::filesystem::directory_iterator it(directory); it != endIt; ++it)
		{
			try
			{
				if (!boost::filesystem::exists(*it) || !boost::filesystem::is_regular_file(*it))
					continue;
			}
			catch (const boost::filesystem::filesystem_error&) { continue; }

			std::smatch match;
			std::string search = it->path().string();
			if (!std::regex_search(search, match, rx))
				continue;

			outFiles.insert(std::stoi(match[1]));
		}
	}

	void PipeProcessUtils::getFilesIndices(IndicesMap& outFiles, const FilesystemPath& directory, const std::string& extension)
	{
		std::string p = ".*\\.(\\d+)\\." + extension + "$";
		std::regex rx(p, std::regex::icase);

		boost::filesystem::directory_iterator endIt;
		for (boost::filesystem::directory_iterator it(directory); it != endIt; ++it)
		{
			try
			{
				if (!boost::filesystem::exists(*it) || !boost::filesystem::is_regular_file(*it))
					continue;
			}
			catch (const boost::filesystem::filesystem_error&) { continue; }

			std::smatch match;
			std::string search = it->path().string();
			if (!std::regex_search(search, match, rx))
				continue;

			outFiles.insert(IndicesMap::value_type(std::stoi(match[1]), FilesystemPath(match[0].str())));
		}
	}

	bool PipeProcessUtils::IndicesCmp::operator()(const IndicesVector& v1, const IndicesVector& v2) const
	{

		for (IndicesVector::const_iterator it1 = v1.begin(), it2 = v2.begin(); it1 != v1.end() && it2 != v2.end(); ++it1, ++it2)
		{
			if (*it1 < *it2)
				return true;
		}
		return false;
	}
}