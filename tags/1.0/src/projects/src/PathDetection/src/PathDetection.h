#pragma once
#ifndef PathDetection_h__
#define PathDetection_h__

#include <utils/PipeProcess.h>

#include <QtCore/QTimer>

namespace clustering
{
	class ISequenceDetector;

	/// <summary>
	/// klasa implementuj¹ca szkielet modu³u. Zawiera funkcjonalnoæ ustawienia parametrów, przetwarzania plików potoku oraz nadzorowania ca³ego procesu.
	/// </summary>
	/// <seealso cref="utils::PipeProcess" />
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
		int m_LastIndex;

		std::unique_ptr<ISequenceDetector> m_Detector;

		bool m_Visualize;
		QTimer m_Timer;

		void startVisualize();
	};
}

#endif // PathDetection_h__