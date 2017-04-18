#ifndef ACTIONCONFIG_H
#define ACTIONCONFIG_H

#include "ui_ActionConfig.h"
#include "IConfigItem.h"

#include "config/Glossary.h"

class ActionConfig : public IConfigItem
{
	Q_OBJECT

public:
	ActionConfig(QWidget *parent = 0);
	~ActionConfig();

	virtual bool load();
	virtual bool save();

private slots:
	void setDescriptorType(int type);
	void setMinFrames(int minFrames);
	void setTrainLength(int length);
	void setTestLength(int length);
	void setMaxVideoWidth(int width);
	void setMaxVideoHeight(int height);

	void onGlossarySelChanged(int row);
	void editGlossaryItem();
	void editGlossaryItem(QListWidgetItem* item);
	void addGlossaryItem();
	void removeGlossaryItem();

private:
	Ui::ActionConfig ui;

	static QListWidgetItem* createItem(const config::Glossary::Action& action);
	static void setItem(QListWidgetItem* item, const QString& name, bool training);
};

#endif // ACTIONCONFIG_H
