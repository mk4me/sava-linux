#ifndef GLOSSARYCONFIG_H
#define GLOSSARYCONFIG_H

#include "ui_GlossaryConfig.h"
#include "ui_GlossaryConfig_AddEdit.h"
#include "IConfigItem.h"
#include "utils/Glossary.h"

#include <QDialog>

class GlossaryConfig : public IConfigItem
{
	Q_OBJECT

public:
	GlossaryConfig(QWidget *parent = 0);
	~GlossaryConfig();

	virtual bool load();
	virtual bool save();

private slots:
	void addAnnotation();
	void deleteAnnotation();
	void editAnnotation();

private:
	Ui::GlossaryConfig ui;
	utils::Glossary m_Glossary;	
};

class GlossaryConfig_AddEdit : public QDialog
{
	Q_OBJECT

public:
	enum GlossaryOperation
	{
		Add,
		Edit
	};

	GlossaryConfig_AddEdit(const std::string _windowTitle, const std::string _label, const int _rowId, GlossaryOperation _glossaryOperation, QListWidget* _listWidget);
	~GlossaryConfig_AddEdit();
	
private slots:
	void ok();
	void cancel();

private:
	Ui::GlossaryConfig_AddEdit ui;
	int rowId;
	GlossaryOperation glossaryOperation;
	QListWidget* listWidget;
};

#endif // GLOSSARYCONFIG_H
