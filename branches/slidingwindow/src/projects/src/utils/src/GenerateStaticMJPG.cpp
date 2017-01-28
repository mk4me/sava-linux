//
//
//  @ Project : Milestone
//  @ File Name : GenerateStaticMJPG.cpp
//  @ Date : 2016-03-30
//  @ Author : Kamil Lebek
//
//

#include "GenerateStaticMJPG.h"
#include "ICameraRawReader.h"

// Include
#include <opencv2/imgcodecs.hpp> // for imencode()

//! Global utils namespace
namespace utils
{
	//! Camera support namespace
	namespace camera
	{

		//
		// Class configuration
		//

		//! Text scale
		const double GenerateStaticMJPG::TEXT_SCALE = 2.0;


		//
		// Class implementation
		//

		GenerateStaticMJPG::GenerateStaticMJPG(const std::string& inText, const size_t& inWidth, const size_t& inHeight, const size_t& inCompression)
		{
			size_t myCompression = inCompression;

			// Compression (1 .. MAX)
			if ((inCompression < 1) || (inCompression > 99))
				myCompression = ICameraRawReader::DEFAULT_COMPRESSION;

			// Create empty OpenCV matrix with my text
			cv::Mat staticMat = CreateEmptyOpenCVMatWithText(inWidth, inHeight, inText);

			// Generate Raw MJPG
			cv::imencode(".jpg", staticMat, m_StaticMJPG, { cv::IMWRITE_JPEG_QUALITY, (100 - (int)myCompression) });
		}

		cv::Mat GenerateStaticMJPG::CreateEmptyOpenCVMatWithText(const size_t& inWidth, const size_t& inHeight, const std::string& inText)
		{
			size_t myWidth = inWidth, myHeight = inHeight;

			// Resolution - Width (1 .. MAX)
			if ((inWidth > ICameraRawReader::DEFAULT_WIDTH) || (!inWidth))
				myWidth = ICameraRawReader::DEFAULT_WIDTH;

			// Resolution - Height (1 .. MAX)
			if ((inHeight > ICameraRawReader::DEFAULT_HEIGHT) || (!inHeight))
				myHeight = ICameraRawReader::DEFAULT_HEIGHT;

			// Create empty image
			cv::Mat retImage((int)myHeight, (int)myWidth, CV_8UC3, CV_RGB(0, 128, 128));

			// Put text on it (in the center of the matrix)
			int baseline = 0;
			cv::Size textSize = cv::getTextSize(inText, TEXT_FONT_TYPE, TEXT_SCALE, TEXT_THICKNESS, &baseline);
			cv::Point textOrg((retImage.cols - textSize.width) / 2, (retImage.rows + textSize.height) / 2);
			cv::putText(retImage, inText, textOrg, TEXT_FONT_TYPE, TEXT_SCALE, CV_RGB(255, 255, 255), TEXT_THICKNESS);
			
			// Return it
			return retImage.clone();
		}

	} // camera

} // utils