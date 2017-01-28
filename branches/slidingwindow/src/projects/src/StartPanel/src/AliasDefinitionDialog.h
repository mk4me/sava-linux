#ifndef ALIASDEFINITIONDIALOG_H
#define ALIASDEFINITIONDIALOG_H

#include <QtWidgets/QDialog>
#include "ui_AliasDefinitionDialog.h"

class AliasDefinitionDialog : public QDialog
{
	Q_OBJECT

public:
	AliasDefinitionDialog(QWidget *parent = 0);
	~AliasDefinitionDialog();

	void setFields(const QString& baseName, const QString& alias);

	QString getBaseName() const;
	QString getAlias() const;

private:
	Ui::AliasDefinitionDialog ui;
};

#endif // ALIASDEFINITIONDIALOG_H
