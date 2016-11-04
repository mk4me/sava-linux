#include "CompressionConfig.h"
#include "ui_CompressionConfig.h"

#include <config/Compression.h>

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
	config::Compression& config = config::Compression::getInstance();
	config.load();

	ui->m_FilesInPackage->setValue(config.getFilesInPackage());
	ui->m_ImageCompression->setValue(config.getImageCompression());
	ui->m_BackgroundHistory->setValue(config.getBackgroundHistory());
	ui->m_DifferenceThreshold->setValue(config.getDifferenceThreshold());
	ui->m_NewBackgroundMinPixels->setValue(config.getNewBackgroundMinPixels());
	ui->m_MinCrumbleArea->setValue(config.getMinCrumbleArea());
	ui->m_MergeCrumblesIterations->setValue(config.getMergeCrumblesIterations());

	ui->m_CompressionMethod->setCurrentIndex(config.getCompressionMethod());

	return true;
}

bool CompressionConfig::save()
{
	return config::Compression::getInstance().save();
}

void CompressionConfig::setFilesInPackage(int val)
{
	config::Compression::getInstance().setFilesInPackage(val);
}

void CompressionConfig::setImageCompression(int val)
{
	config::Compression::getInstance().setImageCompression(val);
}

void CompressionConfig::setBackgroundHistory(int val)
{
	config::Compression::getInstance().setBackgroundHistory(val);
}

void CompressionConfig::setDifferenceThreshold(int val)
{
	config::Compression::getInstance().setDifferenceThreshold(val);
}

void CompressionConfig::setNewBackgroundMinPixels(double val)
{
	config::Compression::getInstance().setNewBackgroundMinPixels(val);
}

void CompressionConfig::setMinCrumbleArea(int val)
{
	config::Compression::getInstance().setMinCrumbleArea(val);
}

void CompressionConfig::setMergeCrumblesIterations(int val)
{
	config::Compression::getInstance().setMergeCrumblesIterations(val);
}

void CompressionConfig::setCompressionMethod(int val)
{
	config::Compression::getInstance().setCompressionMethod(val);
}

