#ifndef ACTIONCONFIG_H
#define ACTIONCONFIG_H

#include "ui_ActionConfig.h"
#include "IConfigItem.h"
#include "config\Action.h"

class ActionConfig : public IConfigItem
{
	Q_OBJECT

public:
	ActionConfig(QWidget *parent = 0);
	~ActionConfig();

	virtual bool load();
	virtual bool save();

private slots:
	void selectDatabasePath();
	void databasePathChanged();
	void setDescriptorType(int type);

private:
	Ui::ActionConfig ui;
	config::Action m_Config;
};

#endif // ACTIONCONFIG_H
