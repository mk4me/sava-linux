//This code prepares the files for svm classificaiton. 
//It reads individual BoF file and group it into training/testing split groups of different classes.
//It needs split1.txt, split2.txt and split3.txt files.

#include "waitKeySeconds.h"
#include "matOperations.h"
#include "formatBinaryStream.h"
#include "scaleData.h"
#include "filesystemUtils.h"

#include "utils/Filesystem.h"
#include "utils/Database.h"
#include "config/Action.h"

#include <opencv/cxcore.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <iostream>
#include <fstream>
#include <string>

#include <omp.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h> 
#include <direct.h>  // for _mkdir( "\\testtmp" ); 
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

int main(int argc, char *argv[])
{
	config::Action::getInstance().load();
	config::Action::DescriptorType descType = config::Action::getInstance().getDescriptorType();
	std::vector<std::string> descritptors;
	if (descType & config::Action::DescriptorType::GBH)
		descritptors.push_back("gbh");
	if (descType & config::Action::DescriptorType::MBH)
		descritptors.push_back("mbh");

	std::string outDir = utils::Filesystem::getDataPath() + "action/";

	int normType = 0;
	float maxVal = 1.f, minVal = 0.f;

	utils::Database database;
	if (!database.load())
	{
		std::cerr << "Cant load video database!\n";
		discoverUO::wait(5);
		exit(-1);
	}
	int clsNum = database.getNumActions();

	for (size_t iDesc = 0; iDesc < descritptors.size(); ++iDesc)
	{
		int numWords;// = 24576;

		std::string  dirName2 = "random";

		std::string dName1, dName2, dNm;

		std::string fileName;
		float *mRow;

		cv::Mat tmpMat;

		cv::Mat cluMat21s;
		cv::Mat cluMat22s;

		cv::Mat sMat21s;
		cv::Mat sMat22s;
		std::vector<int> row11, row12, row21, row22;

		int label;

		BinClusterOutStream<float> *ofile = 0;
		BinClusterInStream *iFile = 0;

		if (normType < 0)
			dNm = outDir + descritptors[iDesc] + "/split1R/";
		else
			dNm = outDir + descritptors[iDesc] + "/split1Rs" + std::to_string(normType) + '/';
#ifdef _WIN32
		_mkdir(dNm.c_str());
#else
        mkdir(dNm.c_str(), 0777);
#endif
		for (int i = 0; i < clsNum; i++)
		{
			cluMat21s = cv::Mat();
			cluMat22s = cv::Mat();

			std::vector<std::string> fileList = database.getVideos(i, utils::Database::TRAIN);
			for (std::string file : fileList)
			{
				std::string filePath = outDir + descritptors[iDesc] + "/run1/random" + std::to_string(i) + "/" + FilesystemUtils::getFileName(file) + ".dat";

				iFile = new BinClusterInStream(filePath);
				tmpMat = cv::Mat();
				iFile->read(tmpMat);
				delete iFile;

				numWords = tmpMat.cols;
				cluMat21s.push_back(tmpMat);
			}

			fileList = database.getVideos(i, utils::Database::TEST);
			for (std::string file : fileList)
			{
				std::string filePath = outDir + descritptors[iDesc] + "/run1/random" + std::to_string(i) + "/" + FilesystemUtils::getFileName(file) + ".dat";

				iFile = new BinClusterInStream(filePath);
				tmpMat = cv::Mat();
				iFile->read(tmpMat);
				delete iFile;

				numWords = tmpMat.cols;
				cluMat22s.push_back(tmpMat);
			}

			if (normType < 0)
			{
				dNm = outDir + descritptors[iDesc] + "/split1R/";
				fileName = dNm + "pwords" + std::to_string(i) + '_' + std::to_string(numWords) + ".dat";
				ofile = new BinClusterOutStream<float>(fileName);
				for (int j0 = 0; j0 < cluMat21s.rows; j0++)
				{
					mRow = cluMat21s.ptr<float>(j0);
					ofile->write(mRow, numWords);
				}
				delete ofile;
				fileName = dNm + "bagWord" + std::to_string(i) + "_" + std::to_string(numWords) + ".dat";
				ofile = new BinClusterOutStream<float>(fileName);
				for (int j0 = 0; j0 < cluMat22s.rows; j0++)
				{
					mRow = cluMat22s.ptr<float>(j0);
					ofile->write(mRow, numWords);
				}
				delete ofile;
			}
			else
			{
				sMat21s.push_back(cluMat21s);
				row21.push_back(cluMat21s.rows);

				sMat22s.push_back(cluMat22s);
				row22.push_back(cluMat22s.rows);
			}
		}

		scaleData scaling(numWords, normType, minVal, maxVal);

		if (normType >= 0)
		{
			scaling.normTrainData(sMat21s, sMat21s);
			scaling.normTestData(sMat22s, sMat22s);
			
			dNm = outDir + descritptors[iDesc] + "/split1Rs" + std::to_string(normType) + '/';
			int count1 = 0, count2 = 0;
			for (int i = 0; i < clsNum; i++)
			{
				fileName = dNm + "pwords" + std::to_string(i) + "_" + std::to_string(numWords) + ".dat";
				ofile = new BinClusterOutStream<float>(fileName);
				for (int i0 = 0; i0 < row21.at(i); i0++)
				{
					mRow = sMat21s.ptr<float>(i0 + count1);
					ofile->write(mRow, numWords);
				}
				count1 += row21.at(i);
				delete ofile;

				fileName = dNm + "bagWord" + std::to_string(i) + "_" + std::to_string(numWords) + ".dat";
				ofile = new BinClusterOutStream<float>(fileName);
				for (int i0 = 0; i0 < row22.at(i); i0++)
				{
					mRow = sMat22s.ptr<float>(i0 + count2);
					ofile->write(mRow, numWords);
				}
				count2 += row22.at(i);
				delete ofile;
			}
		}

		scaling.save(outDir + descritptors[iDesc] + "/statistics.dat");
	}

	//discoverUO::wait();

	return 0;
}