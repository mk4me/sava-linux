#include "filesystemUtils.h"

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

#include "dputils.h" 
#include "dplog.h"

int xSize = 2;
int ySize = 2;
int tSize = 2;
int numBins = 8;
int rootFactor = 2;
int partFactor = 8;
int _numClustersPGbh = 128;
int _numClustersPMbh = 128;

struct DescData
{
	std::string path;
	int bowSize;
};

void merge(const std::vector<DescData>& descriptors, int numClass, const std::string& outDir, const std::string& fileName);

int main(int argc, char **argv)
{
	//Parse OPP parameters
	std::string coreParams = dp::oppGetParamsFromArgs(argc, argv);

	//Use Dp logs
	dp::dpLog log;
	log.initLogFile(coreParams);

	log.dbg("<cpp>params:");
	log.dbgl(coreParams);

	std::string testNameStr = dp::oppGetValueForKey("testName", coreParams);
	log.dbg("<cpp>testNameStr:");
	log.dbgl(testNameStr);

	std::string mergedataOutDir = dp::oppGetValueForKey("mergedataOutDir", coreParams) + "/";
	log.dbg("<cpp>mergedataOutDir:");
	log.dbgl(mergedataOutDir);

	std::string splitdataOutDir = dp::oppGetValueForKey("splitdataOutDir", coreParams) + "/";
	log.dbg("<cpp>splitdataOutDir:");
	log.dbgl(splitdataOutDir);

	std::string dbOutDir = dp::oppGetValueForKey("dbOutDir", coreParams) + "/";
	log.dbg("<cpp>dbOutDir:");
	log.dbgl(dbOutDir);


	if (!dp::oppGetValueForKey("xSizeP", coreParams).empty())
	{
		std::string xSizeP = dp::oppGetValueForKey("xSizeP", coreParams);
		log.dbg("<cpp>xSizeP:");
		log.dbgl(xSizeP);
		xSize = atoi(xSizeP.c_str());
	}

	if (!dp::oppGetValueForKey("ySizeP", coreParams).empty())
	{
		std::string ySizeP = dp::oppGetValueForKey("ySizeP", coreParams);
		log.dbg("<cpp>ySizeP:");
		log.dbgl(ySizeP);
		ySize = atoi(ySizeP.c_str());
	}

	if (!dp::oppGetValueForKey("tSizeP", coreParams).empty())
	{
		std::string tSizeP = dp::oppGetValueForKey("tSizeP", coreParams);
		log.dbg("<cpp>tSizeP:");
		log.dbgl(tSizeP);
		tSize = atoi(tSizeP.c_str());
	}

	if (!dp::oppGetValueForKey("numBinsP", coreParams).empty())
	{
		std::string numBinsP = dp::oppGetValueForKey("numBinsP", coreParams);
		log.dbg("<cpp>numBinsP:");
		log.dbgl(numBinsP);
		numBins = atoi(numBinsP.c_str());
	}

	if (!dp::oppGetValueForKey("_numClustersPMbh", coreParams).empty())
	{
		std::string numClustersPMbh = dp::oppGetValueForKey("_numClustersPMbh", coreParams);
		log.dbg("<cpp>_numClustersPMbh:");
		log.dbgl(numClustersPMbh);
		_numClustersPMbh = atoi(numClustersPMbh.c_str());
	}

	if (!dp::oppGetValueForKey("_numClustersPGbh", coreParams).empty())
	{
		std::string numClustersPGbh = dp::oppGetValueForKey("_numClustersPGbh", coreParams);
		log.dbg("<cpp>_numClustersPGbh:");
		log.dbgl(numClustersPGbh);
		_numClustersPGbh = atoi(numClustersPGbh.c_str());
	}

	if (!dp::oppGetValueForKey("rootFactorP", coreParams).empty())
	{
		std::string rootFactorP = dp::oppGetValueForKey("rootFactorP", coreParams);
		log.dbg("<cpp>rootFactorP:");
		log.dbgl(rootFactorP);
		rootFactor = atoi(rootFactorP.c_str());
	}

	if (!dp::oppGetValueForKey("partFactorP", coreParams).empty())
	{
		std::string partFactorP = dp::oppGetValueForKey("partFactorP", coreParams);
		log.dbg("<cpp>partFactorP:");
		log.dbgl(partFactorP);
		partFactor = atoi(partFactorP.c_str());
	}
	log.closeLogFile();

	// Write OPP string to result file
	/*log.initResultFile(coreParams);
	log.addResult("testName=" + testNameStr + ";accuracy=0.91");
	log.closeResultFile();*/

	std::string databaseOutDir = utils::Filesystem::getAppPath() + dbOutDir;
	utils::Database::setDatabaseDir(databaseOutDir); 

	//std::string outDir = "";
	std::string inDir = utils::Filesystem::getAppPath() + splitdataOutDir;

	std::vector<DescData> descriptors;
	DescData data;

	config::Action::getInstance().load();
	config::Action::DescriptorType descType = config::Action::getInstance().getDescriptorType();
	for (auto descriptor : { config::Action::GBH, config::Action::MBH })
	{
		if (!(descType & descriptor))
			continue;
		/*
		if GBH
		data.path = outDir1 + "/gbh";
		else if MBH
		data.path = outDir1 + "/mbh";
		*/
		if (descriptor == 1)
		{
			data.path = inDir + "gbh/split1Rs0/";
			data.bowSize = 2 * (((xSize * ySize * tSize * numBins) / rootFactor) + ((8 / partFactor) * (xSize * ySize * tSize * numBins))) * _numClustersPGbh;
		}
		else if (descriptor ==2)
		{
			data.path = inDir + "mbh/split1Rs0/";
			data.bowSize = 2 * 2 * (((xSize * ySize * tSize * numBins) / rootFactor) + ((8 / partFactor) * (xSize * ySize * tSize * numBins))) * _numClustersPMbh;
		}

		
			
	
		//data.path = utils::Filesystem::getDataPath() + "action/" + config::Action::getInstance().getDescriptor(descriptor).path; //DO ZMIANY
		//data.bowSize = config::Action::getInstance().getDescriptor(descriptor).size;
		std::cout << "_numClustersPGbh" << _numClustersPGbh << std::endl;
		std::cout << "data BowSize" << data.bowSize << std::endl;
		descriptors.push_back(data);
	}

	utils::Database database;
	if (!database.load())
	{
		std::cerr << "Cant load video database!\n";
		discoverUO::wait(5);
	}
	int numClass = (int)database.getNumActions();
	const std::string outDir = utils::Filesystem::getAppPath() + mergedataOutDir;
	//const std::string outDir = utils::Filesystem::getDataPath() + "action/mergedData/"; //DO ZMIANY
	//_mkdir(outDir.c_str());

	//const std::string outDir = utils::Filesystem::getDataPath() + "action/mergedData/";
/*
#ifdef _WIN32
    _mkdir(outDir.c_str());
#else
    mkdir(outDir.c_str(), 0777);
#endif*/
    boost::filesystem::path dir(outDir.c_str());
    bool result = boost::filesystem::create_directory(dir);
    if(!result){
        std::cerr << "error";
    }
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
	std::cout << "descriptors size" << descriptors.size() << std::endl;
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