#include "CompressionConfig.h"
#include "ui_CompressionConfig.h"

CompressionConfig::CompressionConfig()
	: IConfigItem()
{
	ui = new Ui::CompressionConfig();
	ui->setupUi(this);
}

CompressionConfig::~CompressionConfig()
{
	delete ui;
}

bool CompressionConfig::load()
{
	m_Config.load();

	ui->m_FilesInPackage->setValue(m_Config.getFilesInPackage());
	ui->m_ImageCompression->setValue(m_Config.getImageCompression());
	ui->m_BackgroundHistory->setValue(m_Config.getBackgroundHistory());
	ui->m_DifferenceThreshold->setValue(m_Config.getDifferenceThreshold());
	ui->m_NewBackgroundMinPixels->setValue(m_Config.getNewBackgroundMinPixels());
	ui->m_MinCrumbleArea->setValue(m_Config.getMinCrumbleArea());
	ui->m_MergeCrumblesIterations->setValue(m_Config.getMergeCrumblesIterations());

	return true;
}

bool CompressionConfig::save()
{
	return m_Config.save();
}

void CompressionConfig::setFilesInPackage(int val)
{
	m_Config.setFilesInPackage(val);
}

void CompressionConfig::setImageCompression(int val)
{
	m_Config.setImageCompression(val);
}

void CompressionConfig::setBackgroundHistory(int val)
{
	m_Config.setBackgroundHistory(val);
}

void CompressionConfig::setDifferenceThreshold(int val)
{
	m_Config.setDifferenceThreshold(val);
}

void CompressionConfig::setNewBackgroundMinPixels(double val)
{
	m_Config.setNewBackgroundMinPixels(val);
}

void CompressionConfig::setMinCrumbleArea(int val)
{
	m_Config.setMinCrumbleArea(val);
}

void CompressionConfig::setMergeCrumblesIterations(int val)
{
	m_Config.setMergeCrumblesIterations(val);
}

