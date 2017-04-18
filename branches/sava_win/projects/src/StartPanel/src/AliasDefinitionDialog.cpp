#include "AliasDefinitionDialog.h"

AliasDefinitionDialog::AliasDefinitionDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	Qt::WindowFlags flags = windowFlags();
	Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
	flags = flags & (~helpFlag);
	setWindowFlags(flags);
}

AliasDefinitionDialog::~AliasDefinitionDialog()
{

}

void AliasDefinitionDialog::setFields(const QString& baseName, const QString& alias)
{
	ui.m_BaseName->setText(baseName);
	ui.m_Alias->setText(alias);
}

QString AliasDefinitionDialog::getBaseName() const
{
	return ui.m_BaseName->text();
}

QString AliasDefinitionDialog::getAlias() const
{
	return ui.m_Alias->text();
}
