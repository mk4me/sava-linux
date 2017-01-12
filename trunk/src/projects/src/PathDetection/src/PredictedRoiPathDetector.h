#pragma once
#ifndef PredictedRoiPathDetector_h__
#define PredictedRoiPathDetector_h__

#include "DefaultPathDetector.h"
#include "PredictedRoiFilter.h"
#include "path.h"

namespace clustering
{
	/// <summary>
	/// klasy implementuj¹ce modyfikacje algorytmu o dodanie wykrywania w obszarach przewidywanych na podstawie istniej¹cy cie¿ek.
	/// </summary>
	/// <seealso cref="DefaultPathDetector" />
	class PredictedRoiPathDetector : public DefaultPathDetector
	{
	public:
		PredictedRoiPathDetector();

		virtual void initialize() override;
		virtual void processFrame(const cv::Mat& frame) override;
		virtual void savePaths(sequence::PathStream& pathStream) override;

		void setNullRoiFilter();
		void setVarianceRoiFilter(int windowSize, int threshold);
		void setGradientMagnitudeRoiFilter(int windowSize, int threshold, bool useSobel);

		void setPredictedRoiEnabled(bool val) { m_PredictedRoiEnabled = val; }
		bool isPredictedRoiEnabled() const { return m_PredictedRoiEnabled; }


	private:
		class PathCondition
		{
		public:
			PathCondition(int minLength, int minDistance) : m_MinPathLength(minLength), m_MinPathDistance(minDistance) {}
			bool check(path* p)
			{
				return p->getLength() > m_MinPathLength && p->getMaxDistFromBegin() < m_MinPathDistance;
			}

			int getMinPathLength() const { return m_MinPathLength; }
			void setMinPathLength(int val) { m_MinPathLength = val; }

			int getMinPathDistance() const { return m_MinPathDistance; }
			void setMinPathDistance(int val) { m_MinPathDistance = val; }

		private:
			int m_MinPathLength;
			int m_MinPathDistance;
		};

		PathCondition m_Distance0;
		PathCondition m_Distance1;

		std::shared_ptr<PredictedRoiFilter> m_PredictedRoiFilter;

		cv::Mat m_LastFrame;

		struct PathRemoveFilterParams
		{
			PathRemoveFilterParams() : T1(5), T2(6), N(5) { }
			int T1;
			int T2;
			int N;
		} m_PathRemoveFilterParams;

		bool m_PredictedRoiEnabled;
		int m_PredictedRoiRadius;
	};
}

#endif // PredictedRoiPathDetector_h__