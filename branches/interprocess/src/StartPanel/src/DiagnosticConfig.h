#ifndef DIAGNOSTICCONFIG_H
#define DIAGNOSTICCONFIG_H

#include "ui_DiagnosticConfig.h"
#include "IConfigItem.h"

class DiagnosticConfig : public IConfigItem
{
	Q_OBJECT

public:
	DiagnosticConfig(QWidget *parent = 0);
	~DiagnosticConfig();

	virtual bool load();
	virtual bool save();

private:
	Ui::DiagnosticConfig ui;
};

#endif // DIAGNOSTICCONFIG_H
