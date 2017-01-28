#include "formatBinaryStream.h"
#include "filesystemUtils.h"
#include "waitKeySeconds.h"
#include "gbh/fisherVector_Nc.h"
#include "gbh/stDescriptor.h"
#include "utils/Filesystem.h"

#include <opencv/cxcore.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <boost/filesystem.hpp>

#include <iostream>
#include <fstream>
#include <string>

#include <atlbase.h>  //for A2W(lpr)

const int dataNum = 150000; //number of total features for finding clusters by kmeans. 
//due to the too many training features(over millions), we normally random-choose "dataNum" from all training features
const int _maxFrames = 160; //maxium frames per video. if the video has more frames, it needs to split the video to do multiple processing to avoid memory overflow
const int pSamples = 100;  //number of features chosen from each clip to train gmm

const int _rootSz = 64;   //root size
const int _partsSz = 64 * 8;  //part size 
const int _maxChnl = 4;  //2 channels = root channel + part channel; if 4 channels, MBHx(root + part) + MBHy(root + part)
const int _pcaRtSz = 32;  //reduce the root dim from 64 to 32
const int _pcaPsSz = 64;   //reduce the part dim from 64*8 to 64

const int _Dim[4] = { 32, 64, 32, 64 };
const int _numClusters = 128;  //K = 128 
const std::string _pcaMat = "pca_Mat.yml";  //precomputed pca projective matrix

//This programs learn GMM and pca projection matrix for FV by kmeans from training videos

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: getGMM.exe <path>\n<path> - hmdb51 database path";
		discoverUO::wait(5);
		exit(-1);
	}

	const std::string outDir = utils::Filesystem::getDataPath() + "action/gbh/";
	boost::filesystem::create_directories(outDir);

	int chnl = 2;

	cv::RNG seed(unsigned(time(NULL)));
	MBHparam* para = new MBHparam();
	std::string filepath = utils::Filesystem::getDataPath() + "action/MBH_parameters_input.txt";
	if (!para->readParam(filepath, 1))
	{
		std::cout << "use default HOG3D parameters instead.\n";
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
	cv::Mat roi;
	int maxFrames = _maxFrames;

	bool flipIm4Training = 0;
	bool kmeanCluster = 0;

	cv::Mat feature, cluMat;

	BinClusterOutStream<float> *ofile, *rawFile = NULL;

	std::string fileName = outDir + "binOut.dat";
	std::string databaseDir = utils::Filesystem::unifyPath(argv[1]);

	std::string rFileNm = outDir + "randData100kc.dat";
	std::string classDir;

	int clsNum = utils::Filesystem::getNumDirs(databaseDir);

	//compute the features from training videos
	for (int i = 0; i < clsNum; i++)
	{
		classDir = databaseDir + std::to_string(i) + "/train/";
		if (!boost::filesystem::exists(classDir))
		{
			std::cout << "Class directory does not exist.\n";
			discoverUO::wait(5);
			exit(-1);
		}

		ofile = new BinClusterOutStream<float>(fileName);

		std::vector<std::string> fileList = utils::Filesystem::getFileList(classDir);
		FilesystemUtils::filterFiles(fileList);
		for (std::string fullName : fileList)
		{
			seed.next();
			if (!dscpt.preProcessing(fullName, maxFrames))
			{
				std::cout << "Unable to process loaded video for computing training features!\n";
				discoverUO::wait(5);
				exit(-1);
			}

			dscpt.getRandomFeatures(feature, pSamples, seed);
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
					dscpt.getRandomFeatures(feature, pSamples, seed);
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

		//randomly choose 200000 features from the computed training features
		iFile = new BinClusterInStream(fileName);
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
	//reading randomly chosen 200000 features from file
	iFile = new BinClusterInStream(rFileNm);
	iFile->read(cluMat);
	delete iFile;

	/************************Now doing pca reduction****************/
	if (chnl == 4)
		CV_Assert(2 * _rootSz + 2 * _partsSz == cluMat.cols);
	else
		CV_Assert(_rootSz + _partsSz == cluMat.cols);  //chnl==2;
	cv::Mat cMat[4], oMat[4], mCols;
	cv::PCA pca0[4], pcaOut[4];
	cv::Mat eigenval[4], eigenvec[4], mean[4];
	int maxComponent[4];

	cv::Range colRg[4];
	if (chnl == 2)
	{
		colRg[0] = cv::Range(0, _rootSz);
		colRg[1] = cv::Range(_rootSz, cluMat.cols);
	}
	else if (chnl == 4)
	{
		colRg[0] = cv::Range(0, _rootSz);
		colRg[1] = cv::Range(_rootSz, _rootSz + _partsSz);
		colRg[2] = cv::Range(_rootSz + _partsSz, 2 * _rootSz + _partsSz);
		colRg[3] = cv::Range(2 * _rootSz + _partsSz, cluMat.cols);
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
		pca0[i](cluMat.colRange(colRg[i]), cv::noArray(), CV_PCA_DATA_AS_ROW, maxComponent[i]);
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
	cv::FileStorage fs(outDir + "pca_Mat.yml", cv::FileStorage::WRITE);
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
	char tmpCs[10];
	for (int i = 0; i < chnl; i++)
		gmmFile[i] = outDir + "gmmResults" + (std::string)_itoa(i, tmpCs, 10) + (std::string)".yml";

	fvEncoding fvFt(fName, chnl, _Dim, _numClusters, 0, gmmFile);  //test reading leant gmm files

	return 0;
}