#include "sequence/Action.h"

#include "utils/ProgramOptions.h"

#include <boost/filesystem.hpp>

namespace bfs = boost::filesystem;

std::string g_InputDIr;
sequence::Action::Split g_Split;

bool parseOptions(int argc, const char** argv, ProgramOptions& options);
void setSplit();
bool isActionFile(const std::string& path);

int main(int argc, const char** argv)
{
	ProgramOptions options("Split setter");
	if (!parseOptions(argc, argv, options))
	{
		std::cout << options.printUsage() << std::endl;
		getchar();
		return 0;
	}

	setSplit();

	return 0;
}

bool parseOptions(int argc, const char** argv, ProgramOptions& options)
{
	options.add<std::string>("if", "  input folder");
	options.add<std::string>("split", "  one of the split values [train,test,unassigned]", "unassigned");

	options.parse(argc, argv);

	if (!options.get("if", g_InputDIr))
		return false;

	std::string split;
	options.get("split", split);

	if (split == "train")
		g_Split = sequence::Action::TRAIN;
	else if (split == "test")
		g_Split = sequence::Action::TEST;
	else if (split == "unassigned")
		g_Split = sequence::Action::UNASSIGNED;
	else
		return false;

	return true;
}

void setSplit()
{
	std::cout << "Changing splits.." << std::endl;
	
	int itemsChanged = 0;

	try
	{
		bfs::recursive_directory_iterator endIt;
		bfs::recursive_directory_iterator dirIter(g_InputDIr);
		for (; dirIter != endIt; ++dirIter)
		{
			const std::string& filePath = dirIter->path().string();
			if (!isActionFile(filePath))
				continue;

			sequence::Action action(filePath);
			action.setSplit(g_Split);
			action.save(filePath);

			++itemsChanged;
		}
	}
	catch (const std::exception& ex)
	{
		std::cerr << ex.what() << std::endl;
	}

	std::cout << itemsChanged << " items changed." << std::endl;
	getchar();
}

bool isActionFile(const std::string& path)
{
	if (path.length() < 5)
		return false;

	return path.compare(path.length() - 4, 4, ".acn") == 0;
}
