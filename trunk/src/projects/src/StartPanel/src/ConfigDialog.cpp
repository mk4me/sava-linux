#include "ConfigDialog.h"
#include "ActionConfig.h"
#include "AquisitionConfig.h"
#include "CompressionConfig.h"
#include "PathDetectionConfig.h"
#include "PathAnalysisConfig.h"
#include "CameraConfig.h"
#include "MilestoneConfig.h"
#include "ProcessConfig.h"
#include "NetworkConfig.h"
#include "DirectoryConfig.h"
#include "MonitorConfig.h"
#include "DiagnosticConfig.h"

#include "utils/Filesystem.h"

#include <QtCore/QDir>

ConfigDialog::ConfigDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	Qt::WindowFlags flags = windowFlags();
	Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
	flags = flags & (~helpFlag);
	setWindowFlags(flags);

	ui.m_Splitter->setStretchFactor(0, 1);
	ui.m_Splitter->setStretchFactor(1, 4);

	m_Layout = new QStackedLayout(ui.m_ConfigWidget);
	
	// add config widgets here
	addConfig(new DirectoryConfig());
	addConfig(new ActionConfig());
	addConfig(new CameraConfig());
	addConfig(new MilestoneConfig());
	addConfig(new AquisitionConfig());
	addConfig(new CompressionConfig());
	addConfig(new PathDetectionConfig());
	addConfig(new PathAnalysisConfig());
	addConfig(new NetworkConfig());
	addConfig(new ProcessConfig());
	addConfig(new MonitorConfig());
	addConfig(new DiagnosticConfig());
}

ConfigDialog::~ConfigDialog()
{
	for (IConfigItem* item : m_ConfigItems)
		delete item;
}

void ConfigDialog::viewConfig(int index)
{
	m_Layout->setCurrentIndex(index);
}

void ConfigDialog::load()
{
	for (IConfigItem* item : m_ConfigItems)
		item->load();
}

void ConfigDialog::apply()
{
	QDir dir;
	dir.mkpath(utils::Filesystem::getConfigPath().c_str());

	for (IConfigItem* item : m_ConfigItems)
		item->save();
	accept();
}

void ConfigDialog::addConfig(IConfigItem* item)
{
	m_ConfigItems.append(item);	
	m_Layout->addWidget(item);
	ui.m_ItemList->addItem(item->windowTitle());
}

void ConfigDialog::showEvent(QShowEvent *)
{
	load();
}
