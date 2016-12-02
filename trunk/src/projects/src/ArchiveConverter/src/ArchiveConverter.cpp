#include "ArchiveConverter.h"
#include "sequence/Cluster.h"
#include "sequence/Action.h"
#include "sequence/IVideo.h"

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
	static void convertToText(const std::string& input, const std::string& output)
	{
		T obj(input);
		obj.saveAsText(output);
	}

    static void convertToBinary(const std::string& input, const std::string& output)
    {
        T obj;
        obj.loadFromText(input);
        obj.save(output);
    }
};

struct VideoPolicy
{
	static void convertToText(const std::string& input, const std::string& output)
	{
		auto video = sequence::IVideo::create(input);
		video->saveAsText(output);
	}

    static void convertToBinary(const std::string& input, const std::string& output)
    {
        auto video = sequence::IVideo::createFromText(input);
        video->save(output);
    }
};

template<class Policy>
struct ToText
{
    static void convert(const std::string& input, const std::string& output)
    {
        Policy::convertToText(input, output);
    }

    static std::string newExtension(const std::string& ext)
    {
        return ext + std::string("t");
    }
};

template<class Policy>
struct ToBinary
{
    static void convert(const std::string& input, const std::string& output)
    {
        Policy::convertToBinary(input, output);
    }

    static std::string newExtension(const std::string& ext)
    {
        if (ext.empty() || *ext.rbegin() != 't') {
            throw std::runtime_error("wrong extension format - \"" + ext + "\" was given");
        }
        return ext.substr(0, ext.size() - 1);
    }
};

template <class Conversion>
void _convert(const std::string ext, const fs::path& inputFolder, const fs::path& outputFolder)
{
	auto files = listFiles(inputFolder, ext);
	for (auto& path : files) {
		auto outputPath = (outputFolder / (path.stem().string() + Conversion::newExtension(ext))).string();
		try {
			Conversion::convert(path.string(), outputPath);
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
	_convert<ToText<SimplePolicy<sequence::Action>>>(".acn", inputFolder, outputFolder);
	_convert<ToText<SimplePolicy<sequence::Cluster>>>(".clu", inputFolder, outputFolder);
	_convert<ToText<VideoPolicy>>(".cvs", inputFolder, outputFolder);
}

void ArchiveConverter::convertToBinary(const fs::path& inputFolder, const fs::path& outputFolder)
{
    checkDirs(inputFolder, outputFolder);
    _convert<ToBinary<SimplePolicy<sequence::Action>>>(".acnt", inputFolder, outputFolder);
    _convert<ToBinary<SimplePolicy<sequence::Cluster>>>(".clut", inputFolder, outputFolder);
    _convert<ToBinary<VideoPolicy>>(".cvst", inputFolder, outputFolder);
}
