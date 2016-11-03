#include "FisherVector.h"
#include "scaleData.h"
#include "gbh/fisherVector_Nc.h"
#include "gbh/stDescriptor.h"

#include "utils/Filesystem.h"

#include <opencv/cxcore.h>
#include <opencv/cv.h>

#include <algorithm>

const int FisherVector::c_Dim[4] = { 32, 64, 32, 64 };

FisherVector::FisherVector()
	: c_MaxFrames(160)
	, c_GbhChannels(2)
	, c_NumClusters(128) // K = 128
	, c_ScaleMode(0)
	, c_ScaleMin(0.0f)
	, c_ScaleMax(1.0f)
	, m_GbhDetector(nullptr)
	, m_GbhFvFt(nullptr)
	, m_GbhScaleData(nullptr)
{
	m_GbhSeed = cv::RNG(1448876957 /*unsigned(time(NULL))*/); // this is kind of magic!
}

FisherVector::~FisherVector()
{
	delete m_GbhScaleData;
	delete m_GbhDetector;
	delete m_GbhFvFt;
}

bool FisherVector::load()
{
	m_GbhPath = utils::Filesystem::getDataPath() + "action/gbh/";

	if (!loadGmmGbh())
		return false;
	if (!loadPcaGbh())
		return false;

	m_GbhScaleData = new scaleData(m_GbhFvFt->getFVdim(), c_ScaleMode, c_ScaleMin, c_ScaleMax);
	m_GbhScaleData->load(m_GbhPath + "statistics.dat");

	return true;
}

bool FisherVector::computeFV(const std::string& i_VideoPath, std::vector<float>& i_FisherVector)
{
	if (!computeFvGbh(i_VideoPath, i_FisherVector)) return false;
	if (!normalize(m_GbhScaleData, i_FisherVector)) return false;

	return true;
}

bool FisherVector::loadGmmGbh()
{
	std::vector<std::string> gmmFiles(c_GbhChannels);
	for (int i = 0; i < c_GbhChannels; ++i)
	{
		std::stringstream ss;
		ss << m_GbhPath << "gmmResults" << i << ".yml";
		gmmFiles[i] = ss.str();
	}
	//read parameters
	MBHparam * para = new MBHparam();
	if (!para->readParam(m_GbhPath + "MBH_parameters.txt"))
	{
		std::cout << "Cant read parameters" << std::endl;

		delete para;
		return false;
	}
	m_GbhDetector = new stDetector(para);
	delete para;

	m_GbhFvFt = new fvEncoding(&gmmFiles[0], c_GbhChannels, c_Dim, c_NumClusters);

	return true;
}

bool FisherVector::loadPcaGbh()
{
	m_GbhPCA.resize(c_GbhChannels);
	m_GbhPcaCols.resize(c_GbhChannels + 1);
	m_GbhFtCols.resize(c_GbhChannels + 1);
	m_GbhPcaCols[0] = 0;
	m_GbhFtCols[0] = 0;
	for (int i = 0; i < c_GbhChannels; ++i)
	{
		m_GbhPcaCols[i + 1] = c_Dim[i] + m_GbhPcaCols[i];
		m_GbhFtCols[i + 1] = ((i % 2) ? m_GbhDetector->toPartsFtSz() : m_GbhDetector->toRootFtSz()) + m_GbhFtCols[i];
	}

	cv::FileStorage fs(m_GbhPath + "pca_Mat.yml", cv::FileStorage::READ);
	if (!fs.isOpened())
	{
		std::cout << "Can't open pca_Mat.yml" << std::endl;
		return false;
	}

	//std::vector<cv::Mat> eigenval(c_Channels), eigenvec(c_Channels), mean(c_Channels);
	int chnl = (int)fs["number of channels"];
	if (chnl != c_GbhChannels)
	{
		std::cout << "Load pca failed: wrong number of channels" << std::endl;
		return false;
	}

	fs["Mean0"] >> m_GbhPCA[0].mean;
	fs["Eigenvalues0"] >> m_GbhPCA[0].eigenvalues;
	fs["Eigenvector0"] >> m_GbhPCA[0].eigenvectors;
	fs["Mean1"] >> m_GbhPCA[1].mean;
	fs["Eigenvalues1"] >> m_GbhPCA[1].eigenvalues;
	fs["Eigenvector1"] >> m_GbhPCA[1].eigenvectors;
	if (chnl == 4)
	{
		fs["Mean2"] >> m_GbhPCA[2].mean;
		fs["Eigenvalues2"] >> m_GbhPCA[2].eigenvalues;
		fs["Eigenvector2"] >> m_GbhPCA[2].eigenvectors;
		fs["Mean3"] >> m_GbhPCA[3].mean;
		fs["Eigenvalues3"] >> m_GbhPCA[3].eigenvalues;
		fs["Eigenvector3"] >> m_GbhPCA[3].eigenvectors;
	}
	fs.release();

	return true;
}

bool FisherVector::computeFvGbh(const std::string& i_VideoPath, std::vector<float>& i_FisherVector)
{
	int samNum = 10000;

	try
	{
		if (!m_GbhDetector->preProcessing(i_VideoPath, c_MaxFrames))
			return false;

		cv::Mat feature2;
		m_GbhDetector->getRandomFeatures(feature2, samNum, m_GbhSeed);

		int redoNum;
		if (redoNum = m_GbhDetector->reProcessNum())   //if redoNum(" m_Detector->reProcessNum()") is not equal to zero
		{
			int dSz0, dSz1;
			std::cout << "redo...\n";
			for (int i0 = 1; i0 <= redoNum; i0++)
			{
				dSz0 = m_GbhDetector->getSamplingSz();
				m_GbhDetector->re_Processing(i_VideoPath, c_MaxFrames, i0);

				dSz1 = m_GbhDetector->getSamplingSz();
				cv::Mat tmp0;
				if (i0 < redoNum)
					m_GbhDetector->getRandomFeatures(tmp0, samNum, m_GbhSeed);
				else
					m_GbhDetector->getRandomFeatures(tmp0, (unsigned)(((float)dSz1 / (float)dSz0) * samNum), m_GbhSeed);

				feature2.push_back(tmp0);
			}
		}

		cv::Mat tmpPCA = cv::Mat(feature2.rows, m_GbhPcaCols[c_GbhChannels], feature2.type());
		for (int i0 = 0; i0 < c_GbhChannels; i0++)
		{
			cv::Mat vec = feature2.colRange(m_GbhFtCols[i0], m_GbhFtCols[i0 + 1]);
			cv::Mat coeffs = tmpPCA.colRange(m_GbhPcaCols[i0], m_GbhPcaCols[i0 + 1]);
			m_GbhPCA[i0].project(vec, coeffs);
		}

		i_FisherVector.resize(m_GbhFvFt->getFVdim());
		m_GbhFvFt->getFeatures(tmpPCA, &i_FisherVector[0]);
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
