#include "actionTmplDialog.h"
#include "scheduler.h"

#include "QtWidgets\QMessageBox"
#include "QtCore\QTextCodec"
#include "QtCore\QVariant"


ActionTmplDialog::ActionTmplDialog(QWidget *parent)
	: QDialog(parent)
{
	setupUi(this);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
	
	auto templates = aq_qtutils::CQtUtils::getInstance()->Scheduler()->getActionsTemplateList();
	for(auto it = templates.begin(); it != templates.end(); ++it)
	{
		QListWidgetItem* item = new QListWidgetItem(it->getName().c_str());
		item->setData(Qt::UserRole, QVariant::fromValue(*it));
		m_TemplateList->addItem(item);
	}
}

ActionTmplDialog::~ActionTmplDialog()
{

}

void ActionTmplDialog::addTemplate()
{
	if(m_TemplateName->text().isEmpty())
		return;

	if(!m_TemplateList->findItems(m_TemplateName->text(), Qt::MatchFixedString).empty())
	{
		QMessageBox::warning(this, "Błąd", "Szablon o podanej nazwie już istnieje!");
		return;
	}
	
	QListWidgetItem* item = new QListWidgetItem(m_TemplateName->text());
	item->setData(Qt::UserRole, QVariant::fromValue(aq_qtutils::CAction(m_TemplateName->text().toStdString())));
	m_TemplateList->addItem(item);
	m_TemplateList->setCurrentItem(item);

	m_TemplateName->clear();
}

void ActionTmplDialog::removeTemplate()
{
	if(m_TemplateList->currentItem() != nullptr)
		delete m_TemplateList->takeItem(m_TemplateList->currentRow());
}

void ActionTmplDialog::currentTemplateChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
	if(previous != nullptr)
		previous->setData(Qt::UserRole, QVariant::fromValue(m_ActionWidget->getAction()));

	m_RemoveTemplate->setEnabled(current != nullptr);
	m_ActionWidget->setEnabled(current != nullptr);
	if(current == nullptr)
		return;

	m_ActionWidget->setAction(current->data(Qt::UserRole).value<aq_qtutils::CAction>());
}

void ActionTmplDialog::apply()
{
	if(m_TemplateList->currentItem() != nullptr)
		m_TemplateList->currentItem()->setData(Qt::UserRole, QVariant::fromValue(m_ActionWidget->getAction()));

	aq_qtutils::CAqScheduler::actionsTemplateList templates;
	for(int i = 0; i < m_TemplateList->count(); ++i)
		templates.push_back(m_TemplateList->item(i)->data(Qt::UserRole).value<aq_qtutils::CAction>());
	aq_qtutils::CQtUtils::getInstance()->Scheduler()->setActionsTemplateList(templates);
	aq_qtutils::CDataLoader::saveAqScheduler();

	accept();
}

QStringList ActionTmplDialog::getTemplateNames() const
{
	QStringList names;
	for (int i = 0; i < m_TemplateList->count(); ++i)
		names.push_back(m_TemplateList->item(i)->text());
	return names;
}
