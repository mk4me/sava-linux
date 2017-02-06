#include "biostream.h" 
#include "biistream.h" 
#include "formatBinaryStream.h"

#include "utils/Filesystem.h"
#include "utils/Database.h"
#include "config/Action.h"

#include <opencv/cxcore.h>
#include <opencv/cv.h>

#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif
struct DescData
{
	std::string path;
	int bowSize;
};

void merge(const std::vector<DescData>& descriptors, int numClass, const std::string& outDir, const std::string& fileName);

int main(int argc, char **argv)
{
	std::vector<DescData> descriptors;
	DescData data;

	config::Action::getInstance().load();
	config::Action::DescriptorType descType = config::Action::getInstance().getDescriptorType();
	for (auto descriptor : { config::Action::GBH, config::Action::MBH })
	{
		if (!(descType & descriptor))
			continue;
		
		data.path = utils::Filesystem::getDataPath() + "action/" + config::Action::getInstance().getDescriptor(descriptor).path;
		data.bowSize = config::Action::getInstance().getDescriptor(descriptor).size;
		descriptors.push_back(data);
	}

	utils::Database database;
	if (!database.load())
	{
		std::cerr << "Cant load video database!\n";
		discoverUO::wait(5);
	}
	int numClass = (int)database.getNumActions();

	const std::string outDir = utils::Filesystem::getDataPath() + "action/mergedData/";
#ifdef _WIN32
    _mkdir(outDir.c_str());
#else
    mkdir(outDir.c_str(), 0777);
#endif
	for (int i = 0; i < numClass; i++)
	{
		merge(descriptors, i, outDir, "bagWord");
		merge(descriptors, i, outDir, "pwords");
	}

	std::cout << "Files saved to \"" + outDir + "\"" << std::endl;
	//discoverUO::wait();
	return 0;
}

void merge(const std::vector<DescData>& descriptors, int numClass, const std::string& outDir, const std::string& fileName)
{
	cv::Mat outMat;
	cv::Mat* inMat = new cv::Mat[descriptors.size()];

	for (int i = 0; i < descriptors.size(); ++i)
	{
		BinClusterInStream pFile(descriptors[i].path + fileName + std::to_string(numClass) + '_' + std::to_string(descriptors[i].bowSize) + ".dat");
		cv::Mat* mat = &inMat[i];
		pFile.read(*mat);
	}

	cv::hconcat(inMat, descriptors.size(), outMat);

	delete[] inMat;

	BinClusterOutStream<float> ofile(outDir + fileName + std::to_string(numClass) + '_' + std::to_string(outMat.cols) + ".dat");
	for (int i = 0; i < outMat.rows; i++)
		ofile.write(outMat.ptr<float>(i), outMat.cols);

	std::cout << "Saved class " << numClass << std::endl;
}