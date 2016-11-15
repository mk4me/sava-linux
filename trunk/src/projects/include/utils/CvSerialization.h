#pragma once
#ifndef CvSerialization_h__
#define CvSerialization_h__

#include <opencv/cv.h>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>

#include <iostream>
/// <summary>
/// This file contains serialization procedures for OpenCV classes.
/// </summary>
namespace boost
{
	namespace serialization
	{
		/// <summary>
		/// Serializes and deserializes cv:Point_.
		/// </summary>
		/// <param name=archive>Boost serialization archive.</param>
		/// <param name=value>Point object to serialized/deserialized.</param>
		/// <param name=version>Archive version.</param>
		template<class Archive, class Type>
		void serialize(Archive & archive, cv::Point_<Type> & point, const unsigned int version)
		{
			archive & point.x;
			archive & point.y;
		}

		/// <summary>
		/// Serializes and deserializes cv:Size_.
		/// </summary>
		/// <param name=archive>Boost serialization archive.</param>
		/// <param name=value>Point object to serialized/deserialized.</param>
		/// <param name=version>Archive version.</param>
		template<class Archive, class Type>
		void serialize(Archive & archive, cv::Size_<Type> & size, const unsigned int version)
		{
			archive & size.width;
			archive & size.height;
		}

		/// <summary>
		/// Serializes and deserializes cv:Rect_.
		/// </summary>
		/// <param name=archive>Boost serialization archive.</param>
		/// <param name=value>Rect object to serialized/deserialized.</param>
		/// <param name=version>Archive version.</param>
		template<class Archive, class Type>
		void serialize(Archive & archive, cv::Rect_<Type> & rect, const unsigned int version)
		{
			archive & rect.x;
			archive & rect.y;
			archive & rect.width;
			archive & rect.height;
		}

		/// <summary>
		/// Serializes and deserializes cv:Mat.
		/// </summary>
		/// <param name=archive>Boost serialization archive.</param>
		/// <param name=mat>Mat object to serialized/deserialized.</param>
		/// <param name=version>Archive version.</param>
		template<class Archive>
		void serialize(Archive &archive, cv::Mat& mat, const unsigned int version)
		{
			int cols, rows, type;
			bool continuous;

			if (Archive::is_saving::value) {
				cols = mat.cols; rows = mat.rows; type = mat.type();
				continuous = mat.isContinuous();
			}

			archive & cols & rows & type & continuous;

			if (Archive::is_loading::value)
				mat.create(rows, cols, type);

			if (continuous) {
				size_t data_size = rows * cols * mat.elemSize();
				archive & boost::serialization::make_array(mat.ptr(), data_size);
			}
			else {
				size_t row_size = cols*mat.elemSize();
				for (int i = 0; i < rows; i++) {
					archive & boost::serialization::make_array(mat.ptr(i), row_size);
				}
			}
		}
	}
}

#endif // CvSerialization_h__
