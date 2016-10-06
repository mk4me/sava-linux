#include "ConfigDialog.h"
#include "ActionConfig.h"
#include "GlossaryConfig.h"
#include "AquisitionConfig.h"
#include "CompressionConfig.h"
#include "PathDetectionConfig.h"
#include "PathAnalysisConfig.h"

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
	addConfig(new ActionConfig());
	addConfig(new GlossaryConfig());
	addConfig(new AquisitionConfig());
	addConfig(new CompressionConfig());
	addConfig(new PathDetectionConfig());
	addConfig(new PathAnalysisConfig());
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
