#pragma once
#ifndef Filesystem_h__
#define Filesystem_h__

#include <string>
#include <vector>

namespace boost
{
	namespace filesystem
	{
		class path;
	}
}

namespace utils
{
	/// <summary>
	/// Klasa dostarczaj¹ca zbiór funkcji do operacji na plikach i katalogach.
	/// </summary>
	class Filesystem
	{
	public:
		/// <summary>
		/// Converts dir path to use backslashes only and adds backslash at the end
		/// </summary>
		/// <param name="path">The path to convert.</param>
		/// <returns>Converted path.</returns>
		static std::string unifyPath(const std::string& path);

		/// <summary>
		/// Gets the backslash ended executable directory.
		/// </summary>
		/// <returns>Backslash ended executable directory.</returns>
		static const std::string& getAppPath();

		/// <summary>
		/// Gets the data directory path.
		/// </summary>
		/// <returns>Data directory path.</returns>
		static const std::string& getDataPath();

		/// <summary>
		/// Gets the configuration files directory.
		/// </summary>
		/// <returns>Configuration files directory.</returns>
		static const std::string& getConfigPath();

		/// <summary>
		/// Gets the user directory.
		/// </summary>
		/// <returns>User directory.</returns>
		static const std::string& getUserPath();

		/// <summary>
		/// Gets the file list in the given directory.
		/// </summary>
		/// <param name="directory">The directory to search for files.</param>
		/// <returns>File list from given directory</returns>
		static std::vector<std::string> getFileList(const std::string& directory);

		/// <summary>
		/// Gets the number directories inside the given one.
		/// </summary>
		/// <param name="directory">The path to count directories in.</param>
		/// <returns>Number of directories inside the given path</returns>
		static int getNumDirs(const std::string& directory);

		/// <summary>
		/// Checks if if given path exists.
		/// </summary>
		/// <param name="path">The path to check.</param>
		/// <returns>True if path exists otherwise false</returns>
		static bool exists(const boost::filesystem::path& path);

		/// <summary>
		/// Checks if if given path exists.
		/// </summary>
		/// <param name="path">The path to check.</param>
		/// <returns>True if path exists otherwise false</returns>
		static bool exists(const std::string& path);

		/// <summary>
		/// Removes the all the contents of the given directory.
		/// </summary>
		/// <param name="directory">The directory to clean.</param>
		static void removeContents(const std::string& directory);

	private:
		static std::string getOsAppPath();
	};
}

#endif // Filesystem_h__
