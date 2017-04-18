#include "AquisitionConfig.h"
#include "ui_AquisitionConfig.h"

#include <config/Aquisition.h>

AquisitionConfig::AquisitionConfig()
	: IConfigItem()
{
	ui = new Ui::AquisitionConfig();
	ui->setupUi(this);
}

AquisitionConfig::~AquisitionConfig()
{
	delete ui;
}

bool AquisitionConfig::load()
{
	config::Aquisition& config = config::Aquisition::getInstance();
	config.load();

	ui->m_SeqLength->setValue(config.getSeqLength());
	ui->m_Fps->setValue(config.getFps());
	ui->m_Compression->setValue(config.getCompression());
		
	return true;
}

bool AquisitionConfig::save()
{
	return config::Aquisition::getInstance().save();
}

void AquisitionConfig::setSeqLength(int seqLength)
{
	config::Aquisition::getInstance().setSeqLength(seqLength);
}

void AquisitionConfig::setFps(int fps)
{
	config::Aquisition::getInstance().setFps(fps);
}

void AquisitionConfig::setCompression(int compression)
{
	config::Aquisition::getInstance().setCompression(compression);
}

