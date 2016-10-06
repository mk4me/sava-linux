#pragma once
#ifndef PathDetection_h__
#define PathDetection_h__

#include <utils/PipeProcess.h>

#include <QtCore/QTimer>

namespace clustering
{
	class Detector;
	class PathDetection : public utils::PipeProcess
	{
		Q_OBJECT
	public:
		virtual ~PathDetection();

		static utils::IAppModule* create();
		static void registerParameters(ProgramOptions& programOptions);

		virtual bool loadParameters(const ProgramOptions& options) override;

	protected:
		virtual void reserve() override;
		virtual void process() override;

	private slots:
		void show();

	private:
		PathDetection();

		static const std::string INPUT_FILE_EXTENSION;
		static const std::string OUTPUT_FILE_EXTENSION;

		std::unique_ptr<utils::FileLock> m_FileLock;
		std::string m_FileName;

		std::unique_ptr<Detector> m_Detector;

		bool m_Visualize;
		QTimer m_Timer;

		void startVisualize();
	};
}

#endif // PathDetection_h__