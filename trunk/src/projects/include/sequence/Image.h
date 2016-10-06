#pragma once
#ifndef Image_h__
#define Image_h__

#include <opencv2/core.hpp>

#include <boost/serialization/vector.hpp>

#include <vector>

namespace sequence
{
	class Image
	{
	public:
		//! Default compression rate
		static const int DEFAULT_COMPRESSION;

		//! Default constructor
		Image();
		explicit Image(const std::vector<uchar>& data);
		Image(const cv::Mat& matrix, const int compression = DEFAULT_COMPRESSION);
		Image(const Image& i) : m_Data(i.m_Data) {}
		//! Move constructor
		Image(Image&& i) : m_Data(std::move(i.m_Data)) { }

		//! Move assignment
		Image& operator=(Image&& i)
		{
			m_Data = std::move(i.m_Data);
			return *this;
		}

		//! Conversion to OpenCV matrix
		operator cv::Mat() const { return getMatrix(); }
		cv::Mat getMatrix() const;

	private:
		std::vector<uchar> m_Data;

		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version);
	};

	template<class Archive>
	void Image::serialize(Archive & ar, const unsigned int version)
	{
		ar & m_Data;
	}

}

#endif // Image_h__