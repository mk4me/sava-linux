#include "AquisitionConfig.h"
#include "ui_AquisitionConfig.h"

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
	m_Config.load();

	ui->m_SeqLength->setValue(m_Config.getSeqLength());
	ui->m_Fps->setValue(m_Config.getFps());
	ui->m_Compression->setValue(m_Config.getCompression());
		
	return true;
}

bool AquisitionConfig::save()
{
	return m_Config.save();
}

void AquisitionConfig::setSeqLength(int seqLength)
{
	m_Config.setSeqLength(seqLength);
}

void AquisitionConfig::setFps(int fps)
{
	m_Config.setFps(fps);
}

void AquisitionConfig::setCompression(int compression)
{
	m_Config.setCompression(compression);
}

