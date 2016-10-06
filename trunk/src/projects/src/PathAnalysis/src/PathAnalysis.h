#pragma once
#ifndef PathAnalysis_h__
#define PathAnalysis_h__

#include <utils/PipeProcess.h>

#include <memory>

namespace motion_analysis
{
	class PathClustering;
}

namespace utils
{
	class FileLock;
}

namespace clustering
{	
	class PathAnalysis : public utils::PipeProcess
	{
	public:
		virtual ~PathAnalysis();

		static utils::IAppModule* create();
		static void registerParameters(ProgramOptions& programOptions);

		virtual bool loadParameters(const ProgramOptions& options) override;

	protected:
		virtual void reserve() override;
		virtual void process() override;
		virtual void finalize() override;

		void save();

		void cleanup();

	private:
		PathAnalysis();

		static const std::string INPUT_FILE_EXTENSION;
		static const std::string OUTPUT_FILE_EXTENSION;

		std::shared_ptr<motion_analysis::PathClustering> m_PathClustering;
		
		std::shared_ptr<utils::FileLock> m_FileLock;
		std::string m_FileName;
		int m_FileNumber;
		long long m_StartTime;
		unsigned long long m_LastMaxPathId;

		std::string getInFileName() const;
		std::string getOutFileName(unsigned subId) const;


	};
}

#endif // PathAnalysis_h__