#ifndef MILESTONECONFIG_H
#define MILESTONECONFIG_H

#include "ui_MilestoneConfig.h"
#include "IConfigItem.h"


class MilestoneConfig : public IConfigItem
{
	Q_OBJECT

public:
	MilestoneConfig(QWidget *parent = 0);
	~MilestoneConfig();

	virtual bool load();
	virtual bool save();

private:
	Ui::MilestoneConfig ui;
};

#endif // MILESTONECONFIG_H
