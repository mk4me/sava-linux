#include "ProcessConfig.h"

#include "config/Process.h"

ProcessConfig::ProcessConfig(QWidget *parent)
{
	ui.setupUi(this);

	ui.m_Aquisition->setMaxNodes(1);
	ui.m_PathAnalysis->setMaxNodes(1);
}

ProcessConfig::~ProcessConfig()
{

}

bool ProcessConfig::load()
{
	config::Process& config = config::Process::getInstance();
	config.load();

	ui.m_Timeout->setValue(config.getIdleTimeout());

	ui.m_Aquisition->setNodes(config.getAquisitionNodes());
	ui.m_Compression->setNodes(config.getCompressionNodes());
	ui.m_PathDetection->setNodes(config.getPathDetectionNodes());
	ui.m_PathAnalysis->setNodes(config.getPathAnalysisNodes());
	ui.m_Gbh->setNodes(config.getGbhNodes());
	ui.m_Mbh->setNodes(config.getMbhNodes());
	ui.m_Svm->setNodes(config.getSvmNodes());

	return true;
}

bool ProcessConfig::save()
{
	config::Process& config = config::Process::getInstance();

	config.setIdleTimeout(ui.m_Timeout->value());

	config.setAquisitionNodes(ui.m_Aquisition->getNodes());
	config.setCompressionNodes(ui.m_Compression->getNodes());
	config.setPathDetectionNodes(ui.m_PathDetection->getNodes());
	config.setPathAnalysisNodes(ui.m_PathAnalysis->getNodes());
	config.setGbhNodes(ui.m_Gbh->getNodes());
	config.setMbhNodes(ui.m_Mbh->getNodes());
	config.setSvmNodes(ui.m_Svm->getNodes());

	return config.save();
}
