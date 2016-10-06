#pragma once
#ifndef PipeProcessUtils_h__
#define PipeProcessUtils_h__

#include <boost/filesystem.hpp>
#include <vector>
#include <set>
#include <map>
#include <regex>

namespace utils
{
	class PipeProcessUtils
	{
	public:
		typedef boost::filesystem::path FilesystemPath;
		enum
		{
			NO_INDEX = -1,
		};
		typedef int Index;
		typedef std::vector<Index> IndicesVector;
		struct IndicesCmp
		{
			bool operator()(const IndicesVector& v1, const IndicesVector& v2) const;
		};
		typedef std::set<IndicesVector, IndicesCmp> IndicesVectorSet;

		typedef std::set<Index> IndicesSet;
		typedef std::map<Index, FilesystemPath> IndicesMap;

		/// <summary>
		/// Gets the set of files indices.
		/// </summary>
		/// <param name="outFiles">The out files.</param>
		/// <param name="pattern">The pattern.</param>
		/// <param name="directory">The directory.</param>
		/// <param name="extension">The extension.</param>
		/// <param name="numIndices">The number indices.</param>
		/// <param name="filePattern">The file pattern.</param>
		static void getFilesIndices(IndicesVectorSet& outFiles, const IndicesVector& pattern, const FilesystemPath& directory, const std::string& filePattern, const std::string& extension, unsigned numIndices);
		
		/// <summary>
		/// Gets the set of files indices.
		/// </summary>
		/// <param name="outFiles">The out files.</param>
		/// <param name="directory">The directory.</param>
		/// <param name="extension">The extension.</param>
		/// <param name="filePattern">The file pattern.</param>
		static void getFilesIndices(IndicesSet& outFiles, const FilesystemPath& directory, const std::string& filePattern, const std::string& extension);

		/// <summary>
		/// Gets the map of files indices (key) with file names (value).
		/// </summary>
		/// <param name="outFiles">The out files.</param>
		/// <param name="directory">The directory.</param>
		/// <param name="extension">The extension.</param>
		static void getFilesIndices(IndicesMap& outFiles, const FilesystemPath& directory, const std::string& extension);

		/// <summary>
		/// Gets path of the first file in directory matching pattern. Usefull for pattern searching.
		/// </summary>
		/// <param name="outPath">The result file path.</param>
		/// <param name="directory">The input directory.</param>
		/// <param name="matchPattern">The regex pattern.</param>
		/// <returns></returns>
		static bool getFirstFile(FilesystemPath& outPath, const FilesystemPath& directory, const std::regex& matchPattern);
		
		/// <summary>
		/// Gets path of the first file in directory matching extension and number of indices. Usefull for pattern searching.
		/// </summary>
		/// <param name="outPath">The result file path.</param>
		/// <param name="directory">The input directory.</param>
		/// <param name="extension">The file extension.</param>
		/// <param name="numIndices">The number of incices. Default 1.</param>
		/// <returns></returns>
		static bool getFirstFile(FilesystemPath& outPath, const FilesystemPath& directory, const std::string& extension, unsigned numIndices = 1);
	};
}

#endif // PipeProcessUtils_h__