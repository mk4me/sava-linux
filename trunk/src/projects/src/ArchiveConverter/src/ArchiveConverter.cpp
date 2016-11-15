#include "ArchiveConverter.h"
#include "sequence/Cluster.h"
#include "sequence/Action.h"

namespace fs = boost::filesystem;

void checkDirs(const fs::path& inputFolder, const fs::path& outputFolder)
{
	if (!fs::is_directory(inputFolder) || !fs::is_directory(outputFolder)) {
		throw std::runtime_error("Directory does not exists");
	}
}


std::vector<fs::path> listFiles(const fs::path& dir, const std::string& ext = std::string())
{
	std::vector<fs::path> result;
	fs::directory_iterator it(dir);
	fs::directory_iterator endit;

	while (it != endit) {
		if (fs::is_regular_file(*it) && (ext.empty() || it->path().extension() == ext)) {
			result.push_back(it->path());
		}
		++it;
	}

	return result;
}

void ArchiveConverter::convertDir(const fs::path& inputFolder, const fs::path& outputFolder)
{
	throw std::runtime_error("NYI");
}

void ArchiveConverter::convertToText(const fs::path& inputFolder, const fs::path& outputFolder)
{
	checkDirs(inputFolder, outputFolder);
	auto acnFiles = listFiles(inputFolder, ".acn");
	for (auto& path : acnFiles) {
		sequence::Action action(path.string());
		action.saveAsText((outputFolder / (path.stem().string() + std::string(".acnt"))).string());
	}
	auto cluFiles = listFiles(inputFolder, ".clu");
	for (auto& path : cluFiles) {
		sequence::Cluster cluster(path.string());
		cluster.saveAsText((outputFolder / (path.stem().string() + std::string(".clut"))).string());
	}
}

void ArchiveConverter::convertToBinary(const fs::path& inputFolder, const fs::path& outputFolder)
{
    checkDirs(inputFolder, outputFolder);
    auto acnFiles = listFiles(inputFolder, ".acnt");
    for (auto& path : acnFiles) {
        sequence::Action action;
        action.loadFromText(path.string());
        action.save((outputFolder / (path.stem().string() + std::string(".acn"))).string());
    }
    auto cluFiles = listFiles(inputFolder, ".clut");
    for (auto& path : cluFiles) {
        sequence::Cluster cluster;
        cluster.loadFromText(path.string());
        cluster.save((outputFolder / (path.stem().string() + std::string(".clu"))).string());
    }
}
