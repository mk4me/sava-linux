#ifndef PROCESSCONFIG_H
#define PROCESSCONFIG_H

#include "IConfigItem.h"
#include "ui_ProcessConfig.h"

class ProcessConfig : public IConfigItem
{
	Q_OBJECT

public:
	ProcessConfig(QWidget *parent = 0);
	~ProcessConfig();

	virtual bool load();
	virtual bool save();

private:
	Ui::ProcessConfig ui;
};

#endif // PROCESSCONFIG_H
