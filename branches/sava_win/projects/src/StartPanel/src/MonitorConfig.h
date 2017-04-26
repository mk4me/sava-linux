#ifndef MONITORCONFIG_H
#define MONITORCONFIG_H

#include "ui_MonitorConfig.h"
#include "IConfigItem.h"

#include "config\Monitor.h"

class MonitorConfig : public IConfigItem
{
	Q_OBJECT

public:
	MonitorConfig(QWidget *parent = 0);
	~MonitorConfig();

	virtual bool load();
	virtual bool save();

private slots:
	void onMinQueueChanged(int value);
	void onMaxQueueChanged(int value);

	void addAlias();
	void editAlias();
	void editAlias(int row, int column);
	void removeAlias();

	void onSelectionChanged(int currentRow, int currentCol, int prevRow, int prewCol);	

private:
	Ui::MonitorConfig ui;

	void setAlias(int row, const QString& baseName, const QString& alias);
	void setItem(int row, int column, const QString& text);

	void loadDecorationType(config::Monitor::DecorationType decorationType);
};

#endif // MONITORCONFIG_H
