#include "DiagnosticConfig.h"

#include "config/Diagnostic.h"

DiagnosticConfig::DiagnosticConfig(QWidget *parent)
{
	ui.setupUi(this);
}

DiagnosticConfig::~DiagnosticConfig()
{

}

bool DiagnosticConfig::load()
{
	config::Diagnostic& config = config::Diagnostic::getInstance();
	config.load();

	ui.m_DebugHistory->setValue(config.getDebugHistory());
	
	ui.m_LogToFile->setChecked(config.getLogToFile());
	ui.m_LogMemoryUsage->setChecked(config.getLogMemoryUsage());
	ui.m_LogMonitorQueue->setChecked(config.getLogMonitorQueue());
	ui.m_ShowMonitorDiagnostics->setChecked(config.getShowMonitorDiagnostics());

	return true;
}

bool DiagnosticConfig::save()
{
	config::Diagnostic& config = config::Diagnostic::getInstance();
	
	config.setDebugHistory(ui.m_DebugHistory->value());

	config.setLogToFile(ui.m_LogToFile->isChecked());
	config.setLogMemoryUsage(ui.m_LogMemoryUsage->isChecked());
	config.setLogMonitorQueue(ui.m_LogMonitorQueue->isChecked());
	config.setShowMonitorDiagnostics(ui.m_ShowMonitorDiagnostics->isChecked());

	return config.save();
}
