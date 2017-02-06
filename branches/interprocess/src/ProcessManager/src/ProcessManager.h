#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include <QtWidgets/QMainWindow>
#include "ui_ProcessManager.h"

#include "Logger.h"

#include "network/ProcessManagerLib.h"
#include "config/Network.h"

class ProcessManager : public QMainWindow
{
	Q_OBJECT

public:
	//! Constructor.
	/*!
	\param parent of this window.
	*/
	ProcessManager(QWidget *parent = 0);
	//! Destructor.
	~ProcessManager();	

private:
	//! Object that holds ui configuration.
	Ui::ProcessManagerClass ui;
	//! Object that logs.
	Logger logger_;
	//! Library for process managing.
	Network::ProcessManagerLib pmLib_;
	//! Folder name for gathering logs.
	static const std::string logFolderName_;
	//! File file for gathering logs.
	static const std::string logFileName_;
	//! Folder path for gathering logs.
	std::string logFolderPath_;

	//! Holds how many times particular process was started from beginning.
	QMap<QString, int> procHistory_;

private slots:
	//! Fires new log comes from process.
	/*!
	\param _log reference message that comes from process.
	\param _isError is this error message?
	*/
	void logReceive(const QString& _log, bool _isError);
	//! Fires when Enter is pressed in LineEdit widget.
	void handleLineEditPM_ReturnPressed();
	//! Fires when new process is created and adds tab to main window.
	/*!
	\param _newProc reference to new created process.
	\param _exePathWithArgs reference to full file path with all parameters.
	*/
	void handleCreateNewProcess(std::shared_ptr<QProcess> _newProc, const QString& _exePathWithArgs);
	//! Fires when process is closed or killed and removes tab from main window.
	/*!
	\param _proc reference to finished process.
	\param _exitCode exit code for finished process.
	\param _killedByCommand was killed by user?
	\param _killedByDisconnection was killed by app disconnection?
	*/
	void handleFinishProcess(std::shared_ptr<QProcess> _proc, int _exitCode, bool _killedByCommand, bool _killedByDisconnection);
	//! Removes tab from widget and delete its instance.
	/*!
	\param _index of tab in widget.
	*/
	void closeTab(int _index);
};

#endif // PROCESSMANAGER_H
