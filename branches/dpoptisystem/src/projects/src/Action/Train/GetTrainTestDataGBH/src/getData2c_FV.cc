#include "config/Directory.h"

#include "formatBinaryStream.h"
#include "waitKeySeconds.h"
#include "filesystemUtils.h"

#include "gbh/stDescriptor.h"
#include "gbh/fisherVector_Nc.h"

#include <utils/Filesystem.h>
#include <utils/Database.h>

#include <opencv/cxcore.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <boost/filesystem.hpp>
#include <stdlib.h> //for atoi
#include <string>

#include "dputils.h" 
#include "dplog.h"

using namespace cv;

const int _runNum = 1;
const int _channels = 2;
int _maxFrames = 160; //maxium frames per video. if the video has more frames, it needs to split the video to do multiple processing to avoid memory overflow and vector overflow(patcher sampling from video)
int samNum = 10000;
int xSize = 2;
int ySize = 2;
int tSize = 2;
int numBins = 8;
int rootFactor = 2;
int partFactor = 8;


int _rootSz = xSize * ySize * tSize * numBins;   //root size
int _partsSz = _rootSz * 8;  //part size 
int _pcaRtSz = _rootSz / rootFactor;  //reduce the root dim from 64 to 32
int _pcaPsSz = _partsSz / partFactor; //64;   //reduce the part dim from 64*8 to 64

int _Dim[4] = { _pcaRtSz, _pcaPsSz, _pcaRtSz, _pcaPsSz };
int _numClusters = 128;  //K = 128 
const std::string _pcaMat = "pca_Mat.yml";  //precomputed pca projective matrix

bool loadPCA(PCA* pca0, const std::string& preComputedFl);

int main(int argc, char *argv[])
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

	std::string descParamDir = dp::oppGetValueForKey("descParamDir", coreParams) +"/";
	log.dbg("<cpp>descParamDir:");
	log.dbgl(descParamDir);

	std::string descgbhOutDir = dp::oppGetValueForKey("descgbhOutDir", coreParams) +"/";
	log.dbg("<cpp>descgbhOutDir:");
	log.dbgl(descgbhOutDir);

	std::string modgbhOutDir = dp::oppGetValueForKey("modgbhOutDir", coreParams) +"/";
	log.dbg("<cpp>modgbhOutDir:");
	log.dbgl(modgbhOutDir);

	std::string dbOutDir = dp::oppGetValueForKey("dbOutDir", coreParams) +"/";
	log.dbg("<cpp>dbOutDir:");
	log.dbgl(dbOutDir);

	std::string movies = dp::oppGetValueForKey("moviesDir", coreParams) + "/";
	log.dbg("<cpp>moviesDir:");
	log.dbgl(movies);
	//LOAD PROGRAM PARAMETERS
	
	if (!dp::oppGetValueForKey("_maxFramesP", coreParams).empty())
	{
		std::string _maxFramesP = dp::oppGetValueForKey("_maxFramesP", coreParams);
		log.dbg("<cpp>_maxFramesP:");
		log.dbgl(_maxFramesP);
		_maxFrames = atoi(_maxFramesP.c_str());
	}

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

	if (!dp::oppGetValueForKey("_numClustersPGbh", coreParams).empty())
	{
		std::string _numClustersPGbh = dp::oppGetValueForKey("_numClustersPGbh", coreParams);
		log.dbg("<cpp>_numClustersPGbh:");
		log.dbgl(_numClustersPGbh);
		_numClusters = atoi(_numClustersPGbh.c_str());
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

	if (!dp::oppGetValueForKey("samNumP", coreParams).empty())
	{
		std::string samNumP = dp::oppGetValueForKey("samNumP", coreParams);
		log.dbg("<cpp>samNumP:");
		log.dbgl(samNumP);
		samNum = atoi(samNumP.c_str());
	}
	log.closeLogFile();

	
	//SET PROGRAM VARIABLES ACCORDING TO PARAMETERS 
	config::Directory::getInstance().setVideosPath(movies);
	std::cout << "getVideoPath:" << config::Directory::getInstance().getVideosPath() << std::endl;
	//PARAMS THAT DEPENDS ON THE GIVEN ONES
	_rootSz = xSize * ySize * tSize * numBins;   //root size
	_partsSz = _rootSz * 8;  //part size 
	_pcaRtSz = _rootSz / rootFactor;  //reduce the root dim
	_pcaPsSz = _partsSz / partFactor;   //reduce the part dim 

	_Dim[0] = _pcaRtSz;
	_Dim[1] = _pcaPsSz;
	_Dim[2] = _pcaRtSz;
	_Dim[3] = _pcaPsSz;

	//std::cout << "Po atoi " << std::endl; //do debugowania

	//log.closeLogFile();
	/*
	// Write OPP string to result file
	log.initResultFile(coreParams);
	log.addResult("testName=" + testNameStr + ";accuracy=0.91");
	log.closeResultFile();*/

	std::string databaseOutDir = utils::Filesystem::getAppPath() + dbOutDir;
	utils::Database::setDatabaseDir(databaseOutDir);

	//const std::string outDir = utils::Filesystem::getDataPath() + "action/gbh/";
	const std::string inDir = utils::Filesystem::getAppPath() + modgbhOutDir;
	const std::string outDir = utils::Filesystem::getAppPath() + descgbhOutDir;
	//std::cout << "PRzed opencv " << std::endl; //do debugowania
	printf("OpenCV version %s (%d.%d.%d)\n",
		CV_VERSION,
		CV_MAJOR_VERSION, CV_MINOR_VERSION, CV_SUBMINOR_VERSION);

	//USES_CONVERSION;  //for A2W(lpr) and W2A(...);

	RNG seed[3];

	unsigned randState = unsigned(time(NULL));
	{
		std::ofstream seedFile(outDir + "seed.txt");
		seedFile << randState;
	}
	randState = 1448876957;

	seed[0] = RNG(randState);

	int samNum = 10000;
	if (samNum < 1000 || samNum > 100000)
	{
		std::cout << "How many samples do you want to use?  \nInput: ";
		std::cin >> samNum;
		std::cout << "\n";
		if (samNum < 1000 || samNum>100000)
		{
			std::cout << "Wrong number of samples input! please re-input number of samples again (100-10000): ";
			std::cin >> samNum;
			std::cout << "\n";
		}
	}

	//input parameters
	MBHparam * para = new MBHparam();
	//std::string filepath = utils::Filesystem::getDataPath() + "action/MBH_parameters_input.txt";
	std::string filepath = utils::Filesystem::getAppPath() + descParamDir + "MBH_parameters_input.txt";
	//std::string filepath = utils::Filesystem::getAppPath() + modgbhOutDir + "MBH_parameters_clustering.txt";
	std::cout << filepath << std::endl;
	if (!para->readParam(xSize, ySize, tSize, numBins, filepath, 1))
	{
		std::cerr << "use default HOG3D parameters instead.\n";
		discoverUO::wait(5);
		exit(-1);
	}
	stDetector dscpt(para);

	para->writeParam(outDir + "MBH_parameters.txt");
	delete para;

	int maxFrames = _maxFrames;

	/*_rootSz = xSize * ySize * tSize * numBins;   //root size
	_partsSz = _rootSz * 8;  //part size */
	int rootSz1 = dscpt.toRootFtSz();
	int rootSz = _rootSz;//dscpt.toRootFtSz();
	int partsSz = _partsSz; // dscpt.toPartsFtSz();
	
	/*
	//ADDED FOR DBSYSTEM, VER WITH TXT FILE WITH PARAMS
	int _pcaRtSz = rootSz / atoi(rootFactorP.c_str());  //reduce the root dim
	int _pcaPsSz = partsSz / atoi(partFactorP.c_str());   //reduce the part dim */
	//CHECKING PARAMETERS - debugging
	std::cout << "_maxFrames: " << _maxFrames << std::endl;
	std::cout << "_numClusters: " << _numClusters << std::endl;
	std::cout << "root Sz: " << rootSz << std::endl;
	std::cout << "root Sz1: " << rootSz1 << std::endl;
	std::cout << "parts Sz: " << partsSz << std::endl;
	std::cout << "_pcaRtSz: " << _pcaRtSz << std::endl;
	std::cout << "_pcaPsSz: " << _pcaPsSz << std::endl;
	//_Dim[0] = _pcaRtSz;
	//_Dim[1] = _pcaPsSz;
	//_Dim[2] = _pcaRtSz;
	//_Dim[3] = _pcaPsSz;

	std::fstream fRst;
	fRst.open(outDir + "MBH_parameters.txt", std::ios::app | std::ios::out);
	if (!fRst.is_open())
	{
		std::cerr << "Error opening file to write data dimensions!\n";
		discoverUO::wait(5);
		exit(-1);
	}
	else
	{
		fRst << "\n\n********************************************************\n";
		fRst << "\tRoot Size of descript feature is: " << rootSz << "\n";
		fRst << "\tPart Size of descript feature is: " << partsSz << "\n";
		fRst << "\tThe vector dimension of descript feature is: " << rootSz + partsSz << "\n";
	}

	utils::Database database;
	if (!database.load())
	{
		std::cerr << "Cant load video database!\n";
		discoverUO::wait(5);
		exit(-1);
	}
	int clsNum = database.getNumActions();

	std::string fName[_channels], gmmFile[_channels];

	for (int i = 0; i < _channels; i++)
		fName[i] = inDir + (std::string)"gmmResults" + std::to_string(i) + (std::string)".yml";   //pre-computed GMMs

	fvEncoding fvFt(fName, _channels, _Dim, _numClusters, 2, NULL);   //use pre-computed GMMs

	int fvDim = fvFt.getFVdim();
	std::cout << "Done initializing GMM!" << std::endl;

	PCA pca0[_channels];
	if (!loadPCA(pca0, inDir + _pcaMat))
	{
		std::cerr << "The input precomputed pca file is wrong!" << std::endl;
		discoverUO::wait(5);
		exit(-1);
	}
	int pcaCols[_channels + 1], ftCols[_channels + 1];
	pcaCols[0] = 0;
	ftCols[0] = 0;

	for (int i = 0; i < _channels; i++)
	{
		pcaCols[i + 1] = _Dim[i] + pcaCols[i];
		ftCols[i + 1] = ((i % 2) ? partsSz : rootSz) + ftCols[i]; // 0%2 == 0; 1%2 == 1
	}

	std::string dName;
	std::string inFileNm[2];

	float *arr[_runNum];
	Mat feature2[_runNum];

	std::string fileName[_runNum];
	std::string dName2[_runNum];
	int redoNum;

	BinClusterOutStream<float> *ofile[_runNum];
	for (int i = 0; i < _runNum; i++)
	{
		ofile[i] = 0;
		arr[i] = new float[fvDim];
	}

	for (int i = 0; i < _runNum; i++)
	{
		dName2[i] = outDir + (std::string)"run" + std::to_string(i + 1);
		boost::filesystem::create_directories(dName2[i]);
	}

	for (int i = 0; i < clsNum; i++)
	{
		for (int j = 0; j < _runNum; j++)
		{
			dName2[j] = outDir + (std::string)"run" + std::to_string(j + 1) + (std::string)"/random" + std::to_string(i);
			boost::filesystem::create_directories(dName2[j]);
		}
		std::cout << "Now doing class: " << i << std::endl;

		std::vector<std::string> fileList = database.getVideos(i, utils::Database::TRAIN);
		std::vector<std::string> testFiles = database.getVideos(i, utils::Database::TEST);
		fileList.insert(fileList.end(), testFiles.begin(), testFiles.end());

		for (std::string fullName : fileList)
		{
			std::cout << "Processing file: " << fullName << std::endl;

			try
			{
				if (!dscpt.preProcessing(fullName, maxFrames))
				{
					std::cerr << "Unable to process loaded video for  computing training features 2!\n";
					discoverUO::wait(5);
					exit(-1);
				}
				for (int j = 0; j < _runNum; j++)
				{
					dscpt.getRandomFeatures(feature2[j], samNum, seed[j]);
				}

				if (redoNum = dscpt.reProcessNum())   //if redoNum("dscpt.reProcessNum()") is not equal to zero
				{
					int dSz0, dSz1;
					std::cout << "redo...\n";
					for (int i0 = 1; i0 <= redoNum; i0++)
					{
						dSz0 = dscpt.getSamplingSz();
						dscpt.re_Processing(fullName, maxFrames, i0);

						dSz1 = dscpt.getSamplingSz();
						Mat tmp0;
						for (int j = 0; j < _runNum; j++)
						{
							if (i0 < redoNum)
								dscpt.getRandomFeatures(tmp0, samNum, seed[j]);
							else
								dscpt.getRandomFeatures(tmp0, static_cast<int>(((float)dSz1 / (float)dSz0)*samNum), seed[j]);

							feature2[j].push_back(tmp0);
						}
					}
				}
				for (int j = 0; j < _runNum; j++)
				{
					Mat tmpPCA = Mat(feature2[j].rows, pcaCols[_channels], feature2[j].type());
					for (int i0 = 0; i0 < _channels; i0++)
					{
						Mat vec = feature2[j].colRange(ftCols[i0], ftCols[i0 + 1]);
						Mat coeffs = tmpPCA.colRange(pcaCols[i0], pcaCols[i0 + 1]);
						pca0[i0].project(vec, coeffs);

					}
					fvFt.getFeatures(tmpPCA, arr[j]);
					feature2[j].release();
				}

				for (int j = 0; j < _runNum; j++)
				{
					fileName[j] = dName2[j] + '/' + FilesystemUtils::getFileName(fullName) + ".dat";
					ofile[j] = new BinClusterOutStream<float>(fileName[j]);
					ofile[j]->write(arr[j], fvDim);
					delete ofile[j];
				}
			}
			catch (const std::exception& ex)
			{
				std::cerr << "Error while processing file " << fullName << std::endl << ex.what() << std::endl;
				if (FilesystemUtils::markAsIncorrect(fullName))
					std::cerr << "Video \"" << fullName << "\" marked as incorrect." << std::endl;
				discoverUO::wait();
			}
		}
	}

	for (int i = 0; i < _runNum; i++)
	{
		delete[]arr[i];
	}

	//discoverUO::wait();
	return 0;
}

bool loadPCA(PCA* pca0, const std::string& preComputedFl)
{
	cv::FileStorage fs(preComputedFl, cv::FileStorage::READ);
	if (!fs.isOpened())
	{
		std::cerr << "Can't open pca precomputed file: " << preComputedFl << std::endl;
		return false;
	}
	cv::Mat eigenval[_channels], eigenvec[_channels], mean[_channels];
	int chnl = (int)fs["number of channels"];
	if (chnl != _channels)
	{
		std::cerr << "Wong! The precomputed pca file has different number of channels!" << std::endl;
		return false;
	}

	{
		fs["Mean0"] >> pca0[0].mean;
		fs["Eigenvalues0"] >> pca0[0].eigenvalues;
		fs["Eigenvector0"] >> pca0[0].eigenvectors;
		fs["Mean1"] >> pca0[1].mean;
		fs["Eigenvalues1"] >> pca0[1].eigenvalues;
		fs["Eigenvector1"] >> pca0[1].eigenvectors;

	}
	if (chnl == 4)
	{
		fs["Mean2"] >> pca0[2].mean;
		fs["Eigenvalues2"] >> pca0[2].eigenvalues;
		fs["Eigenvector2"] >> pca0[2].eigenvectors;
		fs["Mean3"] >> pca0[3].mean;
		fs["Eigenvalues3"] >> pca0[3].eigenvalues;
		fs["Eigenvector3"] >> pca0[3].eigenvectors;
	}
	fs.release();
	return true;
}
