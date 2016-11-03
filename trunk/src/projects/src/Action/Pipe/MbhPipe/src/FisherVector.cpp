#include "FisherVector.h"
#include "scaleData.h"
#include "mbh/fisherVector_Nc.h"
#include "mbh/stDescriptor.h"

#include "Utils/Filesystem.h"

#include <opencv/cxcore.h>
#include <opencv/cv.h>

#include <algorithm>

const int FisherVector::c_Dim[4] = { 32, 64, 32, 64 };

FisherVector::FisherVector()
	: c_MaxFrames(160)
	, c_MbhChannels(4)
	, c_NumClusters(128) // K = 128
	, c_ScaleMode(0)
	, c_ScaleMin(0.0f)
	, c_ScaleMax(1.0f)
	, m_MbhDetector(nullptr)
	, m_MbhFvFt(nullptr)
	, m_MbhScaleData(nullptr)
{
	m_MbhSeed = cv::RNG(1448876957 /*unsigned(time(NULL))*/); // this is kind of magic!
}

FisherVector::~FisherVector()
{
	delete m_MbhScaleData;
	delete m_MbhDetector;
	delete m_MbhFvFt;
}

bool FisherVector::load()
{
	m_MbhPath = utils::Filesystem::getDataPath() + "action/mbh/";

	if (!loadGmmMbh())
		return false;
	if (!loadPcaMbh())
		return false;

	m_MbhScaleData = new scaleData(m_MbhFvFt->getFVdim(), c_ScaleMode, c_ScaleMin, c_ScaleMax);
	m_MbhScaleData->load(m_MbhPath + "statistics.dat");

	return true;
}

bool FisherVector::computeFV(const std::string& i_VideoPath, std::vector<float>& i_FisherVector)
{
	if (!computeFvMbh(i_VideoPath, i_FisherVector)) return false;
	if (!normalize(m_MbhScaleData, i_FisherVector)) return false;

	return true;
}

bool FisherVector::loadGmmMbh()
{
	std::vector<std::string> gmmFiles(c_MbhChannels);
	for (int i = 0; i < c_MbhChannels; ++i)
	{
		std::stringstream ss;
		ss << m_MbhPath << "gmmResults" << i << ".yml";
		gmmFiles[i] = ss.str();
	}
	//read parameters
	MBHparam * para = new MBHparam();
	if (!para->readParam(m_MbhPath + "MBH_parameters.txt"))
	{
		std::cout << "Cant read parameters" << std::endl;

		delete para;
		return false;
	}
	m_MbhDetector = new stDetector(para);
	delete para;

	m_MbhFvFt = new fvEncoding(&gmmFiles[0], c_MbhChannels, c_Dim, c_NumClusters);

	return true;
}

bool FisherVector::loadPcaMbh()
{
	m_MbhPCA.resize(c_MbhChannels);
	m_MbhPcaCols.resize(c_MbhChannels + 1);
	m_MbhFtCols.resize(c_MbhChannels + 1);
	m_MbhPcaCols[0] = 0;
	m_MbhFtCols[0] = 0;
	for (int i = 0; i < c_MbhChannels; ++i)
	{
		m_MbhPcaCols[i + 1] = c_Dim[i] + m_MbhPcaCols[i];
		m_MbhFtCols[i + 1] = ((i % 2) ? m_MbhDetector->toPartsFtSz() : m_MbhDetector->toRootFtSz()) + m_MbhFtCols[i];
	}

	cv::FileStorage fs(m_MbhPath + "pca_Mat.yml", cv::FileStorage::READ);
	if (!fs.isOpened())
	{
		std::cout << "Can't open pca_Mat.yml" << std::endl;
		return false;
	}

	//std::vector<cv::Mat> eigenval(c_Channels), eigenvec(c_Channels), mean(c_Channels);
	int chnl = (int)fs["number of channels"];
	if (chnl != c_MbhChannels)
	{
		std::cout << "Load pca failed: wrong number of channels" << std::endl;
		return false;
	}

	fs["Mean0"] >> m_MbhPCA[0].mean;
	fs["Eigenvalues0"] >> m_MbhPCA[0].eigenvalues;
	fs["Eigenvector0"] >> m_MbhPCA[0].eigenvectors;
	fs["Mean1"] >> m_MbhPCA[1].mean;
	fs["Eigenvalues1"] >> m_MbhPCA[1].eigenvalues;
	fs["Eigenvector1"] >> m_MbhPCA[1].eigenvectors;
	if (chnl == 4)
	{
		fs["Mean2"] >> m_MbhPCA[2].mean;
		fs["Eigenvalues2"] >> m_MbhPCA[2].eigenvalues;
		fs["Eigenvector2"] >> m_MbhPCA[2].eigenvectors;
		fs["Mean3"] >> m_MbhPCA[3].mean;
		fs["Eigenvalues3"] >> m_MbhPCA[3].eigenvalues;
		fs["Eigenvector3"] >> m_MbhPCA[3].eigenvectors;
	}
	fs.release();

	return true;
}

bool FisherVector::computeFvMbh(const std::string& i_VideoPath, std::vector<float>& i_FisherVector)
{
	int samNum = 10000;

	try
	{
		if (!m_MbhDetector->preProcessing(i_VideoPath, c_MaxFrames))
			return false;

		cv::Mat feature2;
		m_MbhDetector->getRandomFeatures(feature2, samNum, m_MbhSeed);

		int redoNum;
		if (redoNum = m_MbhDetector->reProcessNum())   //if redoNum(" m_Detector->reProcessNum()") is not equal to zero
		{
			int dSz0, dSz1;
			std::cout << "redo...\n";
			for (int i0 = 1; i0 <= redoNum; i0++)
			{
				dSz0 = m_MbhDetector->getSamplingSz();
				m_MbhDetector->re_Processing(i_VideoPath, c_MaxFrames, i0);

				dSz1 = m_MbhDetector->getSamplingSz();
				cv::Mat tmp0;
				if (i0 < redoNum)
					m_MbhDetector->getRandomFeatures(tmp0, samNum, m_MbhSeed);
				else
					m_MbhDetector->getRandomFeatures(tmp0, (unsigned)(((float)dSz1 / (float)dSz0)*samNum), m_MbhSeed);

				feature2.push_back(tmp0);
			}
		}

		cv::Mat tmpPCA = cv::Mat(feature2.rows, m_MbhPcaCols[c_MbhChannels], feature2.type());
		for (int i0 = 0; i0 < c_MbhChannels; i0++)
		{
			cv::Mat vec = feature2.colRange(m_MbhFtCols[i0], m_MbhFtCols[i0 + 1]);
			cv::Mat coeffs = tmpPCA.colRange(m_MbhPcaCols[i0], m_MbhPcaCols[i0 + 1]);
			m_MbhPCA[i0].project(vec, coeffs);
		}

		i_FisherVector.resize(m_MbhFvFt->getFVdim());
		m_MbhFvFt->getFeatures(tmpPCA, &i_FisherVector[0]);
		feature2.release();

		return true;
	}
	catch (...)
	{
		std::cerr << "Detecton error! Ignoring..." << std::endl;
		return false;
	}
}

bool FisherVector::normalize(scaleData* i_ScaleData, std::vector<float>& i_FisherVector)
{
	cv::Mat data = cv::Mat(1, i_FisherVector.size(), CV_32FC1, i_FisherVector.data());
	i_ScaleData->normTestData(data, data);
	i_FisherVector.assign(data.begin<float>(), data.end<float>());

	return true;
}
