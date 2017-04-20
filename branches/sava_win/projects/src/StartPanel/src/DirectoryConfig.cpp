#include "DirectoryConfig.h"

#include "config/Directory.h"

#include <QtWidgets/QFileDialog>

DirectoryConfig::DirectoryConfig(QWidget *parent)
{
	ui.setupUi(this);
}

DirectoryConfig::~DirectoryConfig()
{

}

bool DirectoryConfig::load()
{
	config::Directory& directoryConfig = config::Directory::getInstance();
	directoryConfig.load();

	ui.m_VideosPath->blockSignals(true);
	ui.m_VideosPath->setText(directoryConfig.getVideosPath().c_str());
	ui.m_VideosPath->blockSignals(false);

	ui.m_TempPath->blockSignals(true);
	ui.m_TempPath->setText(directoryConfig.getTemporaryPath().c_str());
	ui.m_TempPath->blockSignals(false);

	ui.m_AlertsPath->blockSignals(true);
	ui.m_AlertsPath->setText(directoryConfig.getAlertsPath().c_str());
	ui.m_AlertsPath->blockSignals(false);

	ui.m_BackupPath->blockSignals(true);
	ui.m_BackupPath->setText(directoryConfig.getBackupPath().c_str());
	ui.m_BackupPath->blockSignals(false);

	return true;
}

bool DirectoryConfig::save()
{
	return config::Directory::getInstance().save();
}

void DirectoryConfig::selectVideoPath()
{
	QString path = QFileDialog::getExistingDirectory(this, "Select videos path", ui.m_VideosPath->text());
	if (!path.isEmpty())
	{
		ui.m_VideosPath->setText(path);
		onVideoPathChanged();
	}
}

void DirectoryConfig::onVideoPathChanged()
{
	config::Directory::getInstance().setVideosPath(ui.m_VideosPath->text().toStdString());

}

void DirectoryConfig::selectTempPath()
{
	QString path = QFileDialog::getExistingDirectory(this, "Select temporary path", ui.m_TempPath->text());
	if (!path.isEmpty())
	{
		ui.m_TempPath->setText(path);
		onTempPathChanged();
	}
}

void DirectoryConfig::onTempPathChanged()
{
	config::Directory::getInstance().setTemporaryPath(ui.m_TempPath->text().toStdString());
}

void DirectoryConfig::selectAlertsPath()
{
	QString path = QFileDialog::getExistingDirectory(this, "Select alerts path", ui.m_AlertsPath->text());
	if (!path.isEmpty())
	{
		ui.m_AlertsPath->setText(path);
		onAlertsPathChanged();
	}
}

void DirectoryConfig::onAlertsPathChanged()
{
	config::Directory::getInstance().setAlertsPath(ui.m_AlertsPath->text().toStdString());
}

void DirectoryConfig::selectBackupPath()
{
	QString path = QFileDialog::getExistingDirectory(this, "Select backup path", ui.m_BackupPath->text());
	if (!path.isEmpty())
	{
		ui.m_BackupPath->setText(path);
		onBackupPathChanged();
	}
}

void DirectoryConfig::onBackupPathChanged()
{
	config::Directory::getInstance().setBackupPath(ui.m_BackupPath->text().toStdString());
}
