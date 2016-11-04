#ifndef DIRECTORYCONFIG_H
#define DIRECTORYCONFIG_H

#include "ui_DirectoryConfig.h"
#include "IConfigItem.h"

class DirectoryConfig : public IConfigItem
{
	Q_OBJECT

public:
	DirectoryConfig(QWidget *parent = 0);
	~DirectoryConfig();

	virtual bool load();
	virtual bool save();

private slots:
	void selectVideoPath();
	void onVideoPathChanged();

	void selectTempPath();
	void onTempPathChanged();

	void selectAlertsPath();
	void onAlertsPathChanged();

private:
	Ui::DirectoryConfig ui;
};

#endif // DIRECTORYCONFIG_H
