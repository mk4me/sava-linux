#include "addActionDialog.h"
#include "actionTmplDialog.h"

AddActionDialog::AddActionDialog(QWidget* parent /*= 0*/)
	: QDialog(parent)
{
	setupUi(this);
 	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
 
 	m_Templates->setVisible(false);
}

AddActionDialog::AddActionDialog(QWidget* parent, const QStringList& templateNames)
{
	setupUi(this);
 	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
 
	m_ActionName->setVisible(false);
	m_NameLabel->setText("Szablon:");
	m_TemplateName->clear();
	m_TemplateName->addItems(templateNames);
	m_ButtonBox->button(QDialogButtonBox::Ok)->setEnabled(!templateNames.empty());
}

AddActionDialog::AddActionDialog(QWidget* parent, const QString& actionName)
{
 	setupUi(this);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	setWindowTitle("Edycja akcji");

	m_Templates->setVisible(false);
	m_ActionName->setText(actionName);
}

AddActionDialog::~AddActionDialog()
{

}

void AddActionDialog::manageTemplates()
{
	ActionTmplDialog dialog;
	if (dialog.exec() == QDialog::Accepted)
	{
		m_TemplateName->clear();
		m_TemplateName->addItems(dialog.getTemplateNames());
		m_ButtonBox->button(QDialogButtonBox::Ok)->setEnabled(m_TemplateName->count() > 0);
	}
}