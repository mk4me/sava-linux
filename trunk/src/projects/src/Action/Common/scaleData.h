#ifndef _SCALE_DATA_H_
#define _SCALE_DATA_H_

#include "waitKeySeconds.h"

#include <utils/CvSerialization.h>

#include <opencv/cxcore.h>
#include <opencv/cv.h>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <string>
#include <iostream>
#include <fstream>

class scaleData 
{
	cv::Mat			_max;	//maximum for each column
	cv::Mat			_min;	//minimum for each column
	cv::Mat			_range;  //_max - _min
	cv::Mat			_mean;
	cv::Mat			_stddev;
	cv::Mat			_midVal;
	bool		_hasStatics;
	int			_dim;   //feature dim
	int			_normType;
	float		_upper;
	float		_lower;

	void doScaling(const cv::Mat& src, cv::Mat& rst)
	{
		float val0, vMax, vMin, value;
		for (int i = 0; i < src.cols; i++)
		{
			vMax = _max.at<float>(0, i);
			vMin = _min.at<float>(0, i);
			val0 = vMax - vMin;
			if (val0 < FLT_EPSILON)
			{
				rst.col(i) = 0;
				continue;
			}

			for (int j = 0; j < src.rows; j++)
			{
				value = src.at<float>(j, i);
				if (value == vMin)
					rst.at<float>(j, i) = _lower;
				else if (value == vMax)
					rst.at<float>(j, i) = _upper;
				else
					rst.at<float>(j, i) = _lower + (_upper - _lower)* (value - vMin) / val0;
			}
		}
	}
public:

	scaleData(int dim = 4000, int nType = 1, float low = -1., float up = 1.) :
		_dim(dim), _normType(nType), _hasStatics(0),
		_upper(up), _lower(low)
	{
		if (_upper <= _lower && _normType == 0)
		{
			std::cerr << "Wrong input value! The upper value should be larger than lower value.\n";
			discoverUO::wait();
			exit(-1);
		}
	}

	void save(const std::string& file)
	{
		std::ofstream ofs(file);
		boost::archive::text_oarchive oa(ofs);
		oa & _max;
		oa & _min;
		oa & _range;
		oa & _mean;
		oa & _stddev;
		oa & _midVal;
	}

	void load(const std::string& file)
	{
		std::ifstream ifs(file);
		boost::archive::text_iarchive ia(ifs);
		ia & _max;
		ia & _min;
		ia & _range;
		ia & _mean;
		ia & _stddev;
		ia & _midVal;

		_hasStatics = true;
	}

	void normTrainData(const cv::Mat src, cv::Mat &rst)
	{
		float val0, vMax, vMin, value;
		//Mat rst0(src.rows, src.cols, CV_32FC1); 
		cv::Mat rst0(src.rows, src.cols, src.type());
		if (_normType == 1)
		{
			reduce(src, _max, 0, CV_REDUCE_MAX, CV_32F);
			reduce(src, _min, 0, CV_REDUCE_MIN, CV_32F);
			_range = (_max - _min) / 2.;
			_midVal = (_max + _min) / 2.;
			for (int i = 0; i < src.cols; i++)
			{
				val0 = _range.at<float>(0, i);
				if (val0 > FLT_EPSILON)
					rst0.col(i) = (src.col(i) - _midVal.at<float>(0, i)) / val0;
				else
					rst0.col(i) = 0;
			}
		}
		else if (_normType == 2)
		{
			_mean.create(1, src.cols, CV_32FC1);
			_stddev.create(1, src.cols, CV_32FC1);
			cv::Mat col;
			cv::Scalar stddev, mean;

			for (int i = 0; i < src.cols; i++)
			{
				col = src.col(i);
				meanStdDev(col, mean, stddev);
				_mean.at<float>(0, i) = mean[0];
				_stddev.at<float>(0, i) = stddev[0];

				if (stddev[0] > FLT_EPSILON)
					rst0.col(i) = (src.col(i) - mean[0]) / stddev[0];
				else
					rst0.col(i) = 0;
			}
		}
		else
		{
			reduce(src, _max, 0, CV_REDUCE_MAX, CV_32F);
			reduce(src, _min, 0, CV_REDUCE_MIN, CV_32F);
			_range = _max - _min;
			doScaling(src, rst0);
		}
		_hasStatics = 1;
		rst = rst0;
	}

	bool normTestData(const cv::Mat src, cv::Mat &rst)
	{
		if (!_hasStatics)
		{
			std::cerr << "Warning! The statics of training data is not available. Please do scaling traning data first before performing test data scaling.\n";
			return false;
		}
		float val0, vMax, vMin, value;
		//Mat rst0(src.rows, src.cols, CV_32FC1); 
		cv::Mat rst0(src.rows, src.cols, src.type());
		if (_normType == 1)
		{
			for (int i = 0; i < src.cols; i++)
			{
				val0 = _range.at<float>(0, i);
				if (val0 > FLT_EPSILON)
					rst0.col(i) = (src.col(i) - _midVal.at<float>(0, i)) / val0;
				else
					rst0.col(i) = 0;
			}
		}
		else if (_normType == 2)
		{
			for (int i = 0; i < rst0.cols; i++)
			{
				val0 = _stddev.at<float>(0, i);
				if (val0 > FLT_EPSILON)
					rst0.col(i) = (src.col(i) - _mean.at<float>(0, i)) / val0;
				else
					rst0.col(i) = 0;
			}
		}
		else
		{
			doScaling(src, rst0);
		}
		rst = rst0;

		return true;
	}

	void normTrainData(const cv::Mat* src, cv::Mat* rst, int num)
	{
		cv::Mat Data, roi;
		int count = 0;
		for (int i = 0; i < num; i++)
		{
			Data.push_back(src[i]);
		}
		normTrainData(Data, Data);
		for (int i = 0; i < num; i++)
		{
			roi = Data.rowRange(count, count + src[i].rows);
			roi.copyTo(rst[i]);
			count += src[i].rows;
		}
	}

	void normTestData(const cv::Mat* src, cv::Mat* rst, int num)
	{
		cv::Mat Data, roi;
		int count = 0;
		for (int i = 0; i < num; i++)
		{
			Data.push_back(src[i]);
		}
		normTestData(Data, Data);
		for (int i = 0; i < num; i++)
		{
			roi = Data.rowRange(count, count + src[i].rows);
			roi.copyTo(rst[i]);
			count += src[i].rows;
		}
	}

};

#endif	
