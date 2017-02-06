#ifndef PROCESSTAB_H
#define PROCESSTAB_H

#include <QtWidgets/QWidget>
#include "ui_ProcessTab.h"

#include <memory>
#include <QtCore/QProcess>

#include "Logger.h"

class ProcessTab : public QWidget
{
	Q_OBJECT

public:
	//! Constructor.
	/*!
	\param parent of this window.
	\param _proc reference to process from which console should be captured.
	\param _exePathWithArgs reference to full file path with all parameters.
	\param _logFilePath reference to full file path for gathering log.
	*/
	ProcessTab(QWidget *parent, std::shared_ptr<QProcess> _proc, const QString& _exePathWithArgs, 
		const QString& _logFilePath);
	//! Destructor.
	~ProcessTab();
	
	/*!
	\return shared pointer to QProcess.
	*/
	std::shared_ptr<QProcess> getProcPtr();
	//! Writes exit code of process to log.
	/*!
	\param _exitCode of process.
	*/
	void WriteExitCode(int _exitCode);

private:
	//! Object that holds ui configuration.
	Ui::ProcessTab ui;
	//! Object that logs.
	Logger logger_;
	//! Shared pointer to QProcess.
	std::shared_ptr<QProcess> proc_;

private slots:
	//! Fires when process triggers cout.
 	void handleReadStandardOutput();
	//! Fires when process triggers cerr.
 	void handleReadStandardError();
};

#endif // PROCESSTAB_H
