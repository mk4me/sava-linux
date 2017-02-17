#include "ProcessTab.h"
#include "config/Process.h"
#include <QtCore/QDateTime>

ProcessTab::ProcessTab(QWidget *parent, std::shared_ptr<QProcess> _proc, const QString& _exePathWithArgs, 
	const QString& _logFilePath)
	: QWidget(parent), logger_(this), proc_(_proc)
{
	ui.setupUi(this);
	ui.verticalLayout->addWidget(&logger_);
	logger_.setFilePath(_logFilePath);

	QObject::connect(proc_.get(), SIGNAL(readyReadStandardOutput()),
		this, SLOT(handleReadStandardOutput()), Qt::DirectConnection);
	QObject::connect(proc_.get(), SIGNAL(readyReadStandardError()),
		this, SLOT(handleReadStandardError()), Qt::DirectConnection);

	ui.labelPathWithArgs->setText(_exePathWithArgs);
}

ProcessTab::~ProcessTab()
{
	disconnect();
}

std::shared_ptr<QProcess> ProcessTab::getProcPtr()
{
	return proc_;
}

void ProcessTab::WriteExitCode(int _exitCode)
{
	logger_.log("Process crashed with exit code: " + QString::number(_exitCode), Qt::red);
}

void ProcessTab::handleReadStandardOutput()
{	
	logger_.log(proc_->readAllStandardOutput());
}

void ProcessTab::handleReadStandardError()
{
	logger_.log(proc_->readAllStandardError(), Qt::red);
}
