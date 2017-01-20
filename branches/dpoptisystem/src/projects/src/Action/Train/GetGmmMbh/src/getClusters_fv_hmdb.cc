#include "config/Directory.h"

#include "formatBinaryStream.h"
#include "waitKeySeconds.h"
#include "filesystemUtils.h"

#include "mbh//fisherVector_Nc.h"
#include "mbh//stDescriptor.h"

#include "utils/Filesystem.h"
#include "utils/Database.h"

#include <opencv/cxcore.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <boost/filesystem.hpp>

//#include <atlbase.h> //sava-linux.rev

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h> //for atoi

#include "dputils.h" 
#include "dplog.h"

int dataNum = 150000; //number of total features for finding clusters by kmeans. 
//due to the too many training features(over millions), we normally random-choose "dataNum" from all training features
int _maxFrames = 160; //maxium frames per video. if the video has more frames, it needs to split the video to do multiple processing to avoid memory overflow
int pSamples = 100;  //number of features chosen from each clip to train gmm
int xSize = 2;
int ySize = 2;
int tSize = 2;
int numBins = 8;
int rootFactor = 2;
int partFactor = 8;

int _rootSz = xSize * ySize * tSize * numBins;   //root size
int _partsSz = _rootSz * 8;  //part size 

const int _maxChnl = 4;  //2 channels = root channel + part channel; if 4 channels, MBHx(root + part) + MBHy(root + part)
int _pcaRtSz = _rootSz / rootFactor;  //reduce the root dim from 64 to 32
int _pcaPsSz = _partsSz / partFactor; //64;   //reduce the part dim from 64*8 to 64

int _Dim[4] = { _pcaRtSz, _pcaPsSz, _pcaRtSz, _pcaPsSz };
int _numClusters = 128;  //K = 128 
const std::string _pcaMat = "pca_Mat.yml";  //precomputed pca projective matrix
const std::string _gmmFileType = ".yml";

using namespace cv;
namespace fs = boost::filesystem;

//This programs get clusters centers (1000, 2000, 3000 and 4000) by kmeans from training videos

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

	std::string descParamDir = dp::oppGetValueForKey("descParamDir", coreParams) + "/";
	log.dbg("<cpp>descParamDir:");
	log.dbgl(descParamDir);

	std::string modmbhOutDir = dp::oppGetValueForKey("modmbhOutDir", coreParams) + "/";
	log.dbg("<cpp>modmbhOutDir:");
	log.dbgl(modmbhOutDir);

	std::string dbOutDir = dp::oppGetValueForKey("dbOutDir", coreParams) + "/";
	log.dbg("<cpp>dbOutDir:");
	log.dbgl(dbOutDir);

	std::string movies = dp::oppGetValueForKey("moviesDir", coreParams) + "/";
	log.dbg("<cpp>moviesDir:");
	log.dbgl(movies);
	//log.closeLogFile();

	//LOAD PROGRAM PARAMETERS
	if (!dp::oppGetValueForKey("dataNumP", coreParams).empty())
	{
		std::string dataNumP = dp::oppGetValueForKey("dataNumP", coreParams);
		log.dbg("<cpp>dataNumP:");
		log.dbgl(dataNumP);
		dataNum = atoi(dataNumP.c_str()); //double CE = atof(C.c_str());
	}

	if (!dp::oppGetValueForKey("_maxFramesP", coreParams).empty())
	{
		std::string _maxFramesP = dp::oppGetValueForKey("_maxFramesP", coreParams);
		log.dbg("<cpp>_maxFramesP:");
		log.dbgl(_maxFramesP);
		_maxFrames = atoi(_maxFramesP.c_str());
	}

	if (!dp::oppGetValueForKey("pSamplesP", coreParams).empty())
	{
		std::string pSamplesP = dp::oppGetValueForKey("pSamplesP", coreParams);
		log.dbg("<cpp>pSamplesP:");
		log.dbgl(pSamplesP);
		pSamples = atoi(pSamplesP.c_str());
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
		tSize = atoi(tSizeP.c_str()); //args to para->readParam() function
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
		std::string _numClustersPMbh = dp::oppGetValueForKey("_numClustersPMbh", coreParams);
		log.dbg("<cpp>_numClustersPMbh:");
		log.dbgl(_numClustersPMbh);
		_numClusters = atoi(_numClustersPMbh.c_str());
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

	// Write OPP string to result file
	/*
	log.initResultFile(coreParams);
	log.addResult("testName=" + testNameStr + ";accuracy=0.91");
	log.closeResultFile();*/

	std::string databaseOutDir = utils::Filesystem::getAppPath() + dbOutDir;
	utils::Database::setDatabaseDir(databaseOutDir);

	const std::string outDir = utils::Filesystem::getAppPath() + modmbhOutDir;
	boost::filesystem::create_directories(outDir);

	int chnl = 4;

	cv::RNG seed(unsigned(time(NULL)));

	MBHparam * para = new MBHparam();
	std::string filepath = utils::Filesystem::getAppPath() + descParamDir + "MBH_parameters_input.txt";
	if (!para->readParam(xSize, ySize, tSize, numBins, filepath, 1))
	{
		std::cerr << "use default HOG3D parameters instead.\n";
		discoverUO::wait(5);
		exit(-1);
	}

	stDetector dscpt(para);
	para->writeParam(outDir + "MBH_parameters_clustering.txt");
	delete para;

	int start, end, numClusters;

	//randomly choose 120000 features from the computed training features
	BinClusterInStream *iFile;
	int rowNo = 0;
	Mat roi;
	int maxFrames = _maxFrames;

	Mat feature, cluMat;

	BinClusterOutStream<float> *ofile, *rawFile = NULL;

	//delete ofile;

	std::string binName = outDir + "binOut.dat";
	std::string rFileNm = outDir + "randData100kc.dat";

	utils::Database database;
	if (!database.load())
	{
		std::cerr << "Cant load video database!\n";
		discoverUO::wait(5);
		exit(-1);
	}
	int clsNum = database.getNumActions();

	//compute the features from training videos
	for (int i = 0; i < clsNum; i++)
	{
		ofile = new BinClusterOutStream<float>(binName);

		std::vector<std::string> fileList = database.getVideos(i, utils::Database::TRAIN);
		for (std::string fullName : fileList)
		{
			seed.next();
			if (!dscpt.preProcessing(fullName, maxFrames))
			{
				std::cerr << "Unable to process loaded video for computing training features!\n";
				discoverUO::wait(5);
				exit(-1);
			}

			dscpt.getRandomFeatures(feature, 100, seed);
			int height = feature.rows, width = feature.cols;
			float *data;
			for (int i0 = 0; i0 < height; i0++)
			{
				data = feature.ptr<float>(i0);
				ofile->write(data, width);
			}

			int redoNum;
			if (redoNum = dscpt.reProcessNum())
			{
				for (int i0 = 1; i0 <= redoNum; i0++)
				{
					dscpt.re_Processing(fullName, maxFrames, i0);
					dscpt.getRandomFeatures(feature, 100, seed);
					height = feature.rows;
					width = feature.cols;
					for (int j0 = 0; j0 < height; j0++)
					{
						data = feature.ptr<float>(j0);
						ofile->write(data, width);
					}
				}
			}
			std::cout << "Done video file: " << fullName << std::endl;
		}

		feature.release();
		delete ofile;

		iFile = new BinClusterInStream(binName);
		iFile->read(cluMat, dataNum / clsNum);
		delete iFile;
		if (i == 0)
			rawFile = new BinClusterOutStream<float>(rFileNm);

		int height = cluMat.rows, width = cluMat.cols;
		for (int j = 0; j < height; j++)
		{
			float *data = cluMat.ptr<float>(j);
			rawFile->write(data, width);
		}
		cluMat.release();
		std::cout << "done the folder! i = " << i << std::endl;
	}
	delete rawFile;
	dscpt.clear();

	//reading randomly chosen 150000 features from file
	iFile = new BinClusterInStream(rFileNm);
	iFile->read(cluMat);
	delete iFile;
	std::cout << "\ncols: " << cluMat.cols << " rows: " << cluMat.rows << " " << cluMat.at<float>(0, 100) << " " << cluMat.at<float>(0, 200) << " " << cluMat.at<float>(0, 333)
		<< " " << cluMat.at<float>(38, 100) << " " << cluMat.at<float>(38, 200) << " " << cluMat.at<float>(38, 333) <<
		" " << cluMat.at<float>(330, 100) << " " << cluMat.at<float>(330, 200) << " " << cluMat.at<float>(330, 333) << "\n";

	/************************Now doing pca reduction****************/
	if (chnl == 4)
		CV_Assert(2 * _rootSz + 2 * _partsSz == cluMat.cols);
	else
		CV_Assert(_rootSz + _partsSz == cluMat.cols);  //chnl==2;
	Mat cMat[4], oMat[4], mCols;
	PCA pca0[4], pcaOut[4];
	Mat eigenval[4], eigenvec[4], mean[4];
	int maxComponent[4];

	Range colRg[4];
	if (chnl == 2)
	{
		colRg[0] = Range(0, _rootSz);
		colRg[1] = Range(_rootSz, cluMat.cols);
	}
	else if (chnl == 4)
	{
		colRg[0] = Range(0, _rootSz);
		colRg[1] = Range(_rootSz, _rootSz + _partsSz);
		colRg[2] = Range(_rootSz + _partsSz, 2 * _rootSz + _partsSz);
		colRg[3] = Range(2 * _rootSz + _partsSz, cluMat.cols);
	}
	else
		exit(-1);

	maxComponent[0] = _pcaRtSz;
	maxComponent[1] = _pcaPsSz;
	maxComponent[2] = _pcaRtSz;
	maxComponent[3] = _pcaPsSz;

	for (int i = 0; i < chnl; i++)
	{
		std::cout << "Now doing pca matrices training for channel " << i << "...\n";
		pca0[i](cluMat.colRange(colRg[i]), noArray(), CV_PCA_DATA_AS_ROW, maxComponent[i]);
	}


	std::cout << "Now doing pca projection and writing the result data...\n";
	std::string fName[4] = { outDir + "randData100kc0.dat", outDir + "randData100kc1.dat", outDir + "randData100kc2.dat", outDir + "randData100kc3.dat" };
	BinClusterOutStream<float> *ofiles;
	float *data1;
	for (int i0 = 0; i0 < chnl; i0++)
	{
		pca0[i0].project(cluMat.colRange(colRg[i0]), oMat[i0]);

		ofiles = new BinClusterOutStream<float>(fName[i0]);
		for (int j = 0; j < oMat[i0].rows; j++)
		{
			data1 = oMat[i0].ptr<float>(j);
			ofiles->write(data1, oMat[i0].cols);
		}
		delete ofiles;
	}

	std::cout << "Now writing pca matrices...\n";
	//Write matrices to pca_mat.yml
	FileStorage fs(outDir + "pca_Mat.yml", FileStorage::WRITE);
	fs << "number of channels" << chnl;
	fs << "Mean0" << pca0[0].mean;
	fs << "Eigenvalues0" << pca0[0].eigenvalues;
	fs << "Eigenvector0" << pca0[0].eigenvectors;
	fs << "Mean1" << pca0[1].mean;
	fs << "Eigenvalues1" << pca0[1].eigenvalues;
	fs << "Eigenvector1" << pca0[1].eigenvectors;
	if (chnl == 4)
	{
		fs << "Mean2" << pca0[2].mean;
		fs << "Eigenvalues2" << pca0[2].eigenvalues;
		fs << "Eigenvector2" << pca0[2].eigenvectors;
		fs << "Mean3" << pca0[3].mean;
		fs << "Eigenvalues3" << pca0[3].eigenvalues;
		fs << "Eigenvector3" << pca0[3].eigenvectors;
	}
	fs.release();
	std::cout << "done writing pca matrices!";
	/************************Now doing GMM leaning****************/


	std::cout << "now performing GMM learning...\n";
	std::string  gmmFile[4];
	for (int i = 0; i < chnl; i++)
	{
		//fName[i] = (string)"randData100kc"  + (string)itoa(i,tmpCs,10) + (string)".dat";
		gmmFile[i] = outDir + "gmmResults" + std::to_string(i) + (std::string)".yml";
	}

	fvEncoding fvFt(fName, chnl, _Dim, _numClusters, 0, gmmFile);

	//discoverUO::wait();
	//std::cin.get();
	//cin.get();
	return 0;
}