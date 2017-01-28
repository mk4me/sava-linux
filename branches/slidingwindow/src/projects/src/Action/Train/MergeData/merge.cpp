#include "biostream.h" 
#include "biistream.h" 
#include "formatBinaryStream.h"
#include <utils/Filesystem.h>

#include <opencv/cxcore.h>
#include <opencv/cv.h>

#include <direct.h>

struct DescData
{
	std::string path;
	int bowSize;
};

void merge(const std::vector<DescData>& descriptors, int numClass, const std::string& outDir, const std::string& fileName);

int main(int argc, char **argv)
{
	if (argc < 3)
	{
		std::cout << "Usage: merge.exe <path> <size> <path> <size>...\n<path> - split data path\n<size> - number of words in BOW";
		discoverUO::wait(5);
		exit(-1);
	}

	std::vector<DescData> descriptors;

	for (int i = 1;; i += 2)
	{
		if (i >= argc - 1)
			break;

		DescData data;
		data.path = utils::Filesystem::unifyPath(argv[i]);
		
		try
		{
			std::istringstream oss(argv[i + 1]);
			oss >> data.bowSize;
		}
		catch (...)
		{
			std::cout << "Error: Wrong parameters format\nUsage: merge.exe <path> <size> <path> <size>...\n<path> - split data path\n<size> - number of words in BOW";
			break;
		}

		descriptors.push_back(data);
	}

	if (descriptors.size() < 2)
	{
		std::cout << "Usage: merge.exe <path> <size> <path> <size>...\n<path> - split data path\n<size> - number of words in BOW";
		discoverUO::wait(5);
		exit(-1);
	}

	int numClass = 51;

	const std::string outDir = utils::Filesystem::getDataPath() + "action/mergedData/";
	_mkdir(outDir.c_str());

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