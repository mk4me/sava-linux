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
	const int c_GbhChannels;
	const int c_NumClusters;

	const int c_ScaleMode;
	const float c_ScaleMin;
	const float c_ScaleMax;

	std::string m_GbhPath;

	cv::RNG m_GbhSeed;

	stDetector* m_GbhDetector;
	fvEncoding* m_GbhFvFt;
	std::vector<cv::PCA> m_GbhPCA;
	std::vector<int> m_GbhPcaCols;
	std::vector<int> m_GbhFtCols;
	scaleData* m_GbhScaleData;

	bool computeFvGbh(const std::string& i_VideoPath, std::vector<float>& i_FisherVector);

	bool loadGmmGbh();
	bool loadPcaGbh();

	bool normalize(scaleData* i_ScaleData, std::vector<float>& i_FisherVector);
};

#endif // FisherVector_h__
