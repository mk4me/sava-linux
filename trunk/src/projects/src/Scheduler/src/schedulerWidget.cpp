#include "schedulerWidget.h"
#include "actionTmplDialog.h"
#include "addActionDialog.h"
#include "scheduler.h"

#include <algorithm>

SchedulerWidget::SchedulerWidget(QWidget* parent) :QWidget(parent)
{
 	setupUi(this);
 	setupWindow();

	aq_qtutils::CDataLoader::loadAqScheduler();
	const auto& actions = aq_qtutils::CQtUtils::getInstance()->Scheduler()->getActionsList();
	for (auto it = actions.begin(); it != actions.end(); ++it)
		addEntry(*it);
}

SchedulerWidget::~SchedulerWidget()
{
	if (m_ActionList->rowCount() > 0 && m_ActionList->currentRow() >= 0)
	{
		aq_qtutils::CAction action = m_ActionDefinition->getAction();
		m_ActionList->item(m_ActionList->currentRow(), 0)->setData(Qt::UserRole, QVariant::fromValue(action));
	}

	aq_qtutils::CAqScheduler::actionsList actions;
	for (int i = 0; i < m_ActionList->rowCount(); ++i)
		actions.push_back(m_ActionList->item(i, 0)->data(Qt::UserRole).value<aq_qtutils::CAction>());

	aq_qtutils::CQtUtils::getInstance()->Scheduler()->setActionsList(actions);
		aq_qtutils::CDataLoader::saveAqScheduler();
}

void SchedulerWidget::setupWindow()
{
 	m_AddActionMenu.addAction(actionEmpty);
 	m_AddActionMenu.addAction(actionTemplate);
 	m_AddAction->setMenu(&m_AddActionMenu);
 
 	m_Splitter->setSizes(QList<int>() << 420 << 100);
}

void SchedulerWidget::addActionEmpty()
{
	AddActionDialog dialog(this);
	if (dialog.exec() != QDialog::Accepted)
		return;

	aq_qtutils::CAction action(
		dialog.getActionName().toStdString()
		);

 	addEntry(action);
 	m_ActionList->setCurrentCell(m_ActionList->rowCount() - 1, 0);
}

void SchedulerWidget::addActionTemplate()
{
 	const auto& templates = aq_qtutils::CQtUtils::getInstance()->Scheduler()->getActionsTemplateList();
	QStringList templateNames;
	for (auto it = templates.begin(); it != templates.end(); ++it)
		templateNames.push_back(it->getName().c_str());
	AddActionDialog dialog(this, templateNames);
	if (dialog.exec() != QDialog::Accepted)
		return;

	const auto& newTemplates = aq_qtutils::CQtUtils::getInstance()->Scheduler()->getActionsTemplateList();
	aq_qtutils::CAction action = *std::find_if(newTemplates.begin(), newTemplates.end(), [&dialog](const aq_qtutils::CAction& a) { return a.getName() == dialog.getTemplateName().toStdString(); });
	addEntry(action);
	m_ActionList->setCurrentCell(m_ActionList->rowCount() - 1, 0);
}

void SchedulerWidget::removeAction()
{
	if (m_ActionList->currentRow() < 0)
		return;

	m_ActionList->removeRow(m_ActionList->currentRow());
}

void SchedulerWidget::currentActionChanged(int row, int column, int oldRow, int oldColumn)
{
	if (oldRow >= 0)
		m_ActionList->item(oldRow, 0)->setData(Qt::UserRole, QVariant::fromValue(m_ActionDefinition->getAction()));

	bool isAction = row >= 0;
	m_RemoveAction->setEnabled(isAction);
	m_ActionDefinition->setEnabled(isAction);
	if (!isAction)
		return;

 	m_ActionDefinition->setAction(m_ActionList->item(row, 0)->data(Qt::UserRole).value<aq_qtutils::CAction>());
}

void SchedulerWidget::addEntry(const aq_qtutils::CAction& action)
{
	m_ActionList->setRowCount(m_ActionList->rowCount() + 1);

	int lastRow = m_ActionList->rowCount() - 1;
	for (int i = 0; i < m_ActionList->columnCount(); ++i)
	{
		if (m_ActionList->item(lastRow, i) == nullptr)
		{
			if (i < m_ActionList->columnCount() - 1)
				m_ActionList->setItem(lastRow, i, new QTableWidgetItem());
			else		
				m_ActionList->setCellWidget(lastRow, i, (QWidget*)new QPushButton("Uruchom"));
		}
	}

 	m_ActionList->item(lastRow, 0)->setText(action.getName().c_str());
 	m_ActionList->item(lastRow, 0)->setData(Qt::UserRole, QVariant::fromValue(action));
}

void SchedulerWidget::editAction(int row, int column)
{
	aq_qtutils::CAction action = m_ActionDefinition->getAction();
	AddActionDialog dialog(this, action.getName().c_str());
	if (dialog.exec() != QDialog::Accepted)
		return;

	action.setName(dialog.getActionName().toStdString());
	m_ActionList->item(row, 0)->setText(dialog.getActionName());
	m_ActionList->item(row, 0)->setData(Qt::UserRole, QVariant::fromValue(action));

	m_ActionDefinition->setAction(action);
}
