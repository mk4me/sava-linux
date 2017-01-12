#pragma once
#ifndef StatisticPathDistanceFactory_h__
#define StatisticPathDistanceFactory_h__

#include <motionanalysis/algorithms/PathDistanceFactory.h>
#include <iostream>
#include <utils/Stat.h>

namespace clustering
{
	/// <summary>
	/// klasa obliczaj¹ca statystyki tworzenia obiektu miary dopasowania cie¿ki do klastra i klastrów miêdzy sob¹.
	/// </summary>
	/// <seealso cref="motion_analysis::PathDistanceFactory" />
	class StatisticPathDistanceFactory : public motion_analysis::PathDistanceFactory
	{
	public:
		template<typename T>
		StatisticPathDistanceFactory() : m_Factory(std::make_shared<T>()) { }
		StatisticPathDistanceFactory(const std::shared_ptr<motion_analysis::PathDistanceFactory>& factory) : m_Factory(factory) { }

		static void printAndClearStats();

	public:
		virtual std::shared_ptr<motion_analysis::PathDistance> createImpl(const motion_analysis::PathCluster& cluster, const motion_analysis::Path& path, const motion_analysis::PathClusteringParams& params) override;
		virtual std::shared_ptr<motion_analysis::PathDistance> createImpl(const motion_analysis::PathCluster& cluster1, const motion_analysis::PathCluster& cluster2, const motion_analysis::PathClusteringParams& params) override;

	private:
		std::shared_ptr<motion_analysis::PathDistanceFactory> m_Factory;

		struct Stats
		{
			Stat<float> m_TotalStat;
			Stat<float> m_TangentialComponentStat;
			Stat<float> m_SquaredDispertionXStat;
			Stat<float> m_SquaredDispertionYStat;

			Stats() 
				: m_TotalStat("total")
				, m_TangentialComponentStat("tangential")
				, m_SquaredDispertionXStat("sqrDisp.x")
				, m_SquaredDispertionYStat("sqrDisp.y")
			{ }

			void update(const std::shared_ptr<motion_analysis::PathDistance>& distance);
			void printAndClear();

			template<typename T>
			void printAndClear(Stat<T>& stat)
			{
				std::cout << "\t" << stat.name << std::endl;
				std::cout << "\t\tmean = " << stat.mean() << std::endl;
				std::cout << "\t\tmedian = " << stat.median() << std::endl;
				std::cout << "\t\tstd = " << stat.std() << std::endl;
				stat.clear();
			}
		};

		static Stats ms_ClusterStats;
		static Stats ms_PathStats;
	};
}

#endif // StatisticPathDistanceFactory_h__