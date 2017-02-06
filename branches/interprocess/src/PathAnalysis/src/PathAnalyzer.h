#pragma once
#ifndef PathAnalyzer_h__
#define PathAnalyzer_h__

#include <utils/ProgramOptions.h>
#include <boost/timer/timer.hpp>

namespace sequence
{
	class Cluster;
}

namespace clustering
{
	/// <summary>
	/// klasa abstrakcyjna pozwalaj¹ca na zdefiniowanie ró¿nych metod klasteryzacji. Zapewnia implementacjê pomocniczych funkcji.
	/// </summary>
	class PathAnalyzer
	{
	public:
		virtual ~PathAnalyzer();

		virtual bool loadParameters(const ProgramOptions& options) = 0;
		virtual void process() = 0;

		virtual void show() { }

		void setFiles(const std::string& inputSequence, const std::string& inputPaths, const std::string& outputClusterPattern, const std::string& outputClusterExtension);
		void setVisualize(bool val) { m_Visualize = val; }
		bool isVisualize() const { return m_Visualize; }

		void setMinProcessTime(long val) { m_MinProcessTime = val; }
		long getMinProcessTime() const { return m_MinProcessTime; }

	protected:
		PathAnalyzer();

		std::string getInFileName() const;
		std::string getVideoFileName() const;

		void saveCluster(sequence::Cluster& cluster);
		void cleanup();
				
		// FIX na zbyt wolne odswiezanie systemu plikow
		void waitIdle();
		boost::timer::cpu_timer m_Timer;
		long m_MinProcessTime;

	private:
		std::string getOutFileName(unsigned id) const;

		std::string m_InputSequence;
		std::string m_InputPaths;
		std::string m_OutputClusterPattern;
		std::string m_OutputClusterExtension;

		unsigned m_OutputClusterId;

		bool m_Visualize;
	};
}
#endif // PathAnalyzer_h__
