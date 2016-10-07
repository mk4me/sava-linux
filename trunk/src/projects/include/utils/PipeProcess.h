#pragma once
#ifndef PipeProcess_h__
#define PipeProcess_h__

#include "FileLock.h"
#include "IAppModule.h"

#include <QtCore/QFileSystemWatcher>

#include <boost/thread.hpp>
#include <boost/filesystem.hpp>

//! Global utils namespace
namespace utils
{
	class PipeProcess : public QObject, public utils::IAppModule
	{
		Q_OBJECT

	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="inputExtension">File extension for pattern searching.</param>
		/// <param name="inputNumIndices">Number of indices in file for pattern searching. Default 1.</param>
		explicit PipeProcess(const std::string& patternExtension, unsigned inputNumIndices = 1, bool hasOutput = true);

		/// <summary>
		/// Constructor. File pattern isn't specified.
		/// </summary>
		/// <param name="inputNumIndices">Number of indices in file for pattern searching. Default 1.</param>
		explicit PipeProcess(unsigned inputNumIndices = 1, bool hasOutput = true);
		virtual ~PipeProcess();		

		static void registerParameters(ProgramOptions& programOptions);
		
		virtual bool start() override;
		virtual void stop() override;
		virtual bool loadParameters(const ProgramOptions& options) override;

	protected:
		typedef boost::filesystem::path FilesystemPath;
		enum ProcessState
		{
			SPECIFY_PATTERN,
			INIT,
			RESERVE_FILE,
			PROCESS,
			FINALIZE,
			EXIT,
		};

		ProcessState getState();
		void setState(ProcessState state);

		void waitForFile();
				
		/// <summary>
		/// Gets the wait for file timeout in seconds. 0 means infinity.
		/// </summary>
		/// <returns></returns>
		int getWaitTimeout() const { return m_WaitTimeout; }	

		/// <summary>
		/// Sets the wait for file timeout in seconds. 0 means infinity.
		/// </summary>
		/// <param name="value">The timeout in seconds.</param>
		void setWaitTimeout(int value) { m_WaitTimeout = value; }

		virtual void init();
		virtual void reserve();
		virtual void process();
		virtual void finalize();
	

		FilesystemPath getInputFolder() const { return m_InputFolder; }
		FilesystemPath getOutputFolder() const { assert(m_HasOutput); return m_OutputFolder; }
		std::string getFilePattern() const { return m_FilePattern; }		
		
	private slots:
		void onInputDirChanged(const QString& path);
		void onFinished();

	signals:
		void finished();

	private:
		ProcessState m_State;
		boost::mutex m_StateMutex;
		boost::thread m_StateLoopThread;
		boost::condition_variable m_WaitCondition;
		int m_WaitTimeout;

		QFileSystemWatcher m_FileSystemWatcher;
		bool m_FileSystemChanged;

		FilesystemPath m_InputFolder;
		FilesystemPath m_OutputFolder;

		std::string m_PatternExtension;
		const unsigned m_InputNumIndices;
		std::string m_FilePattern;

		bool m_HasOutput;

		void stateLoop();

		void specifyPattern();		
	};
}

#endif // PipeProcess_h__