#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include "ui_ConfigDialog.h"
#include "IConfigItem.h"

#include <QtWidgets/QStackedLayout>

class ConfigDialog : public QDialog
{
	Q_OBJECT

public:
	ConfigDialog(QWidget *parent = 0);
	~ConfigDialog();

private slots:
	void viewConfig(int index);

	void load();
	void apply();

private:
	Ui::ConfigDialog ui;

	QStackedLayout* m_Layout;
	QVector<IConfigItem*> m_ConfigItems;

	/// <summary>
	/// Adds widget to the list with "WindowTitle" property as the name.
	/// </summary>
	/// <param name="widget">The configuration widget.</param>
	void addConfig(IConfigItem* widget);

	virtual void showEvent(QShowEvent *) override;
};

#endif // CONFIGDIALOG_H
