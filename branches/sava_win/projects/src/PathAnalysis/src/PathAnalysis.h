#pragma once
#ifndef PathAnalysis_h__
#define PathAnalysis_h__

#include <utils/PipeProcess.h>

#include <memory>

#include <boost/timer/timer.hpp>

#include <QtCore/QTimer>

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
	class IVisualizer;
	class ConfigurationWindow;
	class PathAnalyzer;

	/// <summary>
	/// klasa implementuj¹ca szkielet modu³u. Zawiera funkcjonalnoæ ustawiania parametrów ,przetwarzania plików potoku oraz nadzorowania ca³ego procesu.
	/// </summary>
	/// <seealso cref="utils::PipeProcess" />
	class PathAnalysis : public utils::PipeProcess
	{
		Q_OBJECT
	public:
		virtual ~PathAnalysis();

		static utils::IAppModule* create();
		static void registerParameters(ProgramOptions& programOptions);

		virtual bool loadParameters(const ProgramOptions& options) override;

	protected:
		virtual void reserve() override;
		virtual void process() override;

		void save();

		void cleanup();

	private slots:
		void show();

	private:
		PathAnalysis();

		static const std::string INPUT_FILE_EXTENSION;
		static const std::string OUTPUT_FILE_EXTENSION;
				
		std::shared_ptr<utils::FileLock> m_FileLock;
		std::shared_ptr<utils::FileLock> m_VideoFileLock;
		std::string m_FileName;
		int m_FileNumber;

		std::string getInFileName() const;
		std::string getVideoFileName() const;
		std::string getOutFileName(unsigned subId) const;

		
		bool m_Visualize;
		QTimer m_VisualizationTimer;
		FilesystemPath m_VideoFolder;

		std::shared_ptr<PathAnalyzer> m_PathAnalyzer;

		void startVisualize();
	};
}

#endif // PathAnalysis_h__