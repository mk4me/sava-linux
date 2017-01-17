#ifndef FisherVector_h__
#define FisherVector_h__

#include <string>

#include <opencv2/core.hpp>

class stDetector;
class fvEncoding;
class scaleData;

class FisherVector
{
public:
	FisherVector();
	~FisherVector();

	bool load();
	bool computeFV(const std::string& i_VideoPath, std::vector<float>& i_FisherVector);

private:
	static const int c_Dim[4];

	const int c_MaxFrames;
	const int c_MbhChannels;
	const int c_NumClusters;

	const int c_ScaleMode;
	const float c_ScaleMin;
	const float c_ScaleMax;

	std::string m_MbhPath;

	cv::RNG m_MbhSeed;

	stDetector* m_MbhDetector;
	fvEncoding* m_MbhFvFt;
	std::vector<cv::PCA> m_MbhPCA;
	std::vector<int> m_MbhPcaCols;
	std::vector<int> m_MbhFtCols;
	scaleData* m_MbhScaleData;

	bool computeFvMbh(const std::string& i_VideoPath, std::vector<float>& i_FisherVector);

	bool loadGmmMbh();
	bool loadPcaMbh();

	bool normalize(scaleData* i_ScaleData, std::vector<float>& i_FisherVector);
};

#endif // FisherVector_h__
