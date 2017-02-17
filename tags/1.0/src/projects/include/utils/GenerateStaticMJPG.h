//
//
//  @ Project : Milestone
//  @ File Name : GenerateStaticMJPG.h
//  @ Date : 2016-03-30
//  @ Author : Kamil Lebek
//
//

#ifndef _GENERATESTATICMJPG_H
#define _GENERATESTATICMJPG_H

// Includes
#include <vector>
#include <opencv2/opencv.hpp>	 // for Mat()

//! Global utils namespace
namespace utils
{
	//! Camera support namespace
	namespace camera
	{

		//! Class generating MJPG frames with proper timestamp with static text
		class GenerateStaticMJPG
		{
		public:
			//! Explicit constructor to avoid casting - all parameters of the static MJPG are passed here
			/*!
				\param inText text to display on static frame
				\param inWidth MJPG frame width
				\param inHeight MJPG frame height
				\param inCompression compression rate from 1 (lowest) to 99 (highest) - 30 is default setting
			*/
			explicit GenerateStaticMJPG(const std::string& inText, const size_t& inWidth, const size_t& inHeight, const size_t& inCompression);

			//! Simple polymorphic destructor
			virtual ~GenerateStaticMJPG()
			{
				// Everything should be destroyed automatically
				// ...
			}

			//! Returns static RawMJPG (must be copy, because it cannot be moved into RawMJPG)
			std::vector<unsigned char> getRawFrame() const
			{
				return m_StaticMJPG;
			}

			// no copy and assign
			GenerateStaticMJPG(const GenerateStaticMJPG&) = delete;
			GenerateStaticMJPG& operator=(const GenerateStaticMJPG&) = delete;

			// no move
			GenerateStaticMJPG(GenerateStaticMJPG&&) = delete;
			GenerateStaticMJPG& operator=(GenerateStaticMJPG&&) = delete;

		private:

			//! Creates OpenCV matrix with right dimensions
			static cv::Mat CreateEmptyOpenCVMatWithText(const size_t& inWidth, const size_t& inHeight, const std::string& inText);

			//! My copy of static MJPG
			std::vector<unsigned char> m_StaticMJPG;

			//! Text scale
			static const double TEXT_SCALE;

			//! Font face type
			static const int TEXT_FONT_TYPE = cv::FONT_HERSHEY_DUPLEX;

			//! Text thickness
			static const int TEXT_THICKNESS = 2;

		}; // GenerateStaticMJPG

	} // camera

} // utils

#endif // _GENERATESTATICMJPG_H