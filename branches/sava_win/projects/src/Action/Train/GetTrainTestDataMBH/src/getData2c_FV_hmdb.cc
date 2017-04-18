#include "formatBinaryStream.h"
#include "waitKeySeconds.h"
#include "filesystemUtils.h"

#include "mbh/fisherVector_Nc.h"
#include "mbh/stDescriptor.h"

#include <utils/Filesystem.h>
#include <utils/Database.h>

#include <opencv/cxcore.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <boost/filesystem.hpp>

#include <string>

using namespace cv;

const int _runNum = 1; //for random feature seletion, run 3 times
const int _channels = 4;
const int _maxFrames = 160; //maxium frames per video. if the video has more frames, it needs to split the video to do multiple processing to avoid memory overflow and vector overflow(patcher sampling from video)

const int _rootSz = 64;   //root size
const int _partsSz = 64 * 8;  //part size 

const int _Dim[4] = { 32, 64, 32, 64 };
const int _numClusters = 128;  //K = 128 
const std::string _pcaMat = "pca_Mat.yml";  //precomputed pca projective matrix
const std::string _gmmFileType = ".yml";

bool loadPCA(PCA* pca0, const std::string& preComputedFl);

int main(int argc, char *argv[])
{
	const std::string outDir = utils::Filesystem::getDataPath() + "action/mbh/";

	printf("OpenCV version %s (%d.%d.%d)\n",
		CV_VERSION,
		CV_MAJOR_VERSION, CV_MINOR_VERSION, CV_SUBMINOR_VERSION);

	RNG seed[3];
	seed[0] = RNG(1448876957);

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

	MBHparam * para = new MBHparam();
	std::string filepath = utils::Filesystem::getDataPath() + "action/MBH_parameters_input.txt";
	if (!para->readParam(filepath, 1))
	{
		std::cerr << "use default HOG3D parameters instead.\n";
		discoverUO::wait(5);
		exit(-1);
	}

	stDetector dscpt(para);
	para->writeParam(outDir + "MBH_parameters.txt");
	delete para;

	int maxFrames = _maxFrames;

	int rootSz = dscpt.toRootFtSz();
	int partsSz = dscpt.toPartsFtSz();

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
		fName[i] = outDir + "gmmResults" + std::to_string(i) + ".yml";   //pre-computed GMMs

	fvEncoding fvFt(fName, _channels, _Dim, _numClusters, 2, NULL);  //use pre-computed GMMs
	int fvDim = fvFt.getFVdim();
	std::cout << "Done initializing GMM!" << std::endl;

	PCA pca0[_channels];
	if (!loadPCA(pca0, outDir + _pcaMat))
	{
		std::cerr << "The input precomputed pca file is wrong!" << std::endl;
		discoverUO::wait(5);
		exit(-1);
	}
	Mat tmpPCA;
	int pcaCols[_channels + 1], ftCols[_channels + 1];
	pcaCols[0] = 0;
	ftCols[0] = 0;

	for (int i = 0; i < _channels; i++)
	{
		pcaCols[i + 1] = _Dim[i] + pcaCols[i];
		ftCols[i + 1] = ((i % 2) ? partsSz : rootSz) + ftCols[i];
	}

	std::string dName;
	std::string inFileNm[2];

	float *arr[_runNum];

	Mat feature2[_runNum];

	std::string fileName[_runNum];
	std::string dName2[_runNum];
	std::string sName;
	int redoNum;

	BinClusterOutStream<float> *ofile[_runNum];
	for (int i = 0; i < _runNum; i++)
	{
		ofile[i] = 0;
		arr[i] = new float[fvDim];
	}

	for (int i = 0; i < _runNum; i++)
	{
		dName2[i] = outDir + "run" + std::to_string(i + 1);
		boost::filesystem::create_directories(dName2[i]);
	}

	for (int i = 0; i < clsNum; i++)
	{
		for (int j = 0; j < _runNum; j++)
		{
			dName2[j] = outDir + "run" + std::to_string(j + 1) + "/random" + std::to_string(i);
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
						dscpt.re_Processing(sName, maxFrames, i0);

						dSz1 = dscpt.getSamplingSz();
						Mat tmp0;
						for (int j = 0; j < _runNum; j++)
						{
							if (i0 < redoNum)
								dscpt.getRandomFeatures(tmp0, samNum, seed[j]);
							else
								dscpt.getRandomFeatures(tmp0, unsigned(((float)dSz1 / (float)dSz0)*samNum), seed[j]);

							feature2[j].push_back(tmp0);
						}
					}
				}
				for (int j = 0; j < _runNum; j++)
				{
					tmpPCA = Mat(feature2[j].rows, pcaCols[_channels], feature2[j].type());
#pragma omp parallel for 
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
