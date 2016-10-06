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
		explicit PipeProcess(const std::string& inputExtension, unsigned inputNumIndices = 1);

		/// <summary>
		/// Constructor. File pattern isn't specified.
		/// </summary>
		/// <param name="inputNumIndices">Number of indices in file for pattern searching. Default 1.</param>
		explicit PipeProcess(unsigned inputNumIndices = 1);
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

		virtual void init();
		virtual void reserve();
		virtual void process();
		virtual void finalize();
	

		FilesystemPath getInputFolder() const { return m_InputFolder; }
		FilesystemPath getOutputFolder() const { return m_OutputFolder; }
		std::string getFilePattern() const { return m_FilePattern; }

		
		
	private slots:
		void onInputDirChanged(const QString& path);

	private:
		ProcessState m_State;
		boost::mutex m_StateMutex;
		boost::thread m_StateLoopThread;
		boost::condition_variable m_WaitCondition;

		QFileSystemWatcher m_FileSystemWatcher;
		bool m_FileSystemChanged;

		FilesystemPath m_InputFolder;
		FilesystemPath m_OutputFolder;

		const std::string m_InputExtension;
		const unsigned m_InputNumIndices;
		std::string m_FilePattern;

		void stateLoop();

		void specifyPattern();
	};
}

#endif // PipeProcess_h__