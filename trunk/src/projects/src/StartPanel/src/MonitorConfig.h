#ifndef MONITORCONFIG_H
#define MONITORCONFIG_H

#include "ui_MonitorConfig.h"
#include "IConfigItem.h"

class MonitorConfig : public IConfigItem
{
	Q_OBJECT

public:
	MonitorConfig(QWidget *parent = 0);
	~MonitorConfig();

	virtual bool load();
	virtual bool save();

private slots:
void onAlertsTimeChanged(int time);
void onMaxAlertsChanged(int count);

private:
	Ui::MonitorConfig ui;
};

#endif // MONITORCONFIG_H
