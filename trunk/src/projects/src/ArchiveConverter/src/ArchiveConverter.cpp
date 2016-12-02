#include "ArchiveConverter.h"
#include "sequence/Cluster.h"
#include "sequence/Action.h"
#include "Sequence/IVideo.h"

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

template<class T>
struct SimplePolicy
{
	static void convert(const std::string& input, const std::string& output)
	{
		T obj(input);
		obj.saveAsText(output);
	}
};

struct VideoPolicy
{
	static void convert(const std::string& input, const std::string& output)
	{
		auto video = sequence::IVideo::create(input);
		video->saveAsText(output);
	}
};

template <class ConversionPolicy>
void _convertToText(const std::string ext, const fs::path& inputFolder, const fs::path& outputFolder)
{
	auto files = listFiles(inputFolder, ext);
	for (auto& path : files) {
		auto outputPath = (outputFolder / (path.stem().string() + ext + std::string("t"))).string();
		try {
			ConversionPolicy::convert(path.string(), outputPath);
			std::cout << "Processing: " << path << std::endl;
		}
		catch (...) {
			std::cerr << "Problem with: " << path << " , skipping" <<std::endl;
		}
	}
}



void ArchiveConverter::convertToText(const fs::path& inputFolder, const fs::path& outputFolder)
{
	checkDirs(inputFolder, outputFolder);
	_convertToText<SimplePolicy<sequence::Action>>(".acn", inputFolder, outputFolder);
	_convertToText<SimplePolicy<sequence::Cluster>>(".clu", inputFolder, outputFolder);
	_convertToText<VideoPolicy>(".cvs", inputFolder, outputFolder);
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
