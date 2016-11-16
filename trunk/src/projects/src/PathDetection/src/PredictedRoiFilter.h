#pragma once
#ifndef PredictedRoiFilter_h__
#define PredictedRoiFilter_h__

#include <opencv2/core.hpp>

#include <boost/timer/timer.hpp>

namespace clustering
{
	class PredictedRoiFilter
	{
	public:
		PredictedRoiFilter();
		virtual ~PredictedRoiFilter();

		virtual void update(const cv::Mat& frame) = 0;
		virtual bool check(const cv::Point& point) = 0;

		virtual void createGui();

		int windowSize;
		int threshold;

		unsigned long long time;

	protected:
		class TimeMesurer : boost::timer::cpu_timer
		{
		public:
			TimeMesurer(unsigned long long& t) : time(t) {}
			~TimeMesurer() { time += elapsed().wall; }
		private:
			unsigned long long& time;
		};
	};

	class NullPredictedRoiFilter : public PredictedRoiFilter
	{
	public:
		virtual void update(const cv::Mat& frame) { }
		virtual bool check(const cv::Point& point) { return true; }
		virtual void createGui() { }
	};

	class VariancePredictedRoiFilter : public PredictedRoiFilter
	{
	public:
		virtual void update(const cv::Mat& frame);
		virtual bool check(const cv::Point& point);

	private:
		cv::Mat currentFrame;
	};

	class GradientMagnitudePredictedRoiFilter : public PredictedRoiFilter
	{
	public:
		GradientMagnitudePredictedRoiFilter();

		virtual void update(const cv::Mat& frame);

		virtual bool check(const cv::Point& point);

		virtual void createGui();

		/// <summary>
		/// Uses sobel or sharr filter. True - sobel, false - sharr.
		/// </summary>
		bool useSobel;

	private:
		cv::Mat gradient;

		static void useSobelChange(int state, void* data);
	};
}

#endif // PredictedRoiFilter_h__
