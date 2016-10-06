#include "actionDefinitionWidget.h"
#include "QtWidgets\QFileDialog"
#include "QtCore\QTextCodec"
#include "QtCore\QDir"
#include "QtWidgets\QMessageBox"

ActionDefinitionWidget::ActionDefinitionWidget(QWidget *parent)
	: QWidget(parent)
{
 	setupUi(this);
 	//QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
 	m_MachineIp->setValidator(new QRegExpValidator(QRegExp("\\b(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b"), this));
}

ActionDefinitionWidget::~ActionDefinitionWidget()
{

}

void ActionDefinitionWidget::addProcess()
{
	QString file = QFileDialog::getOpenFileName(this, "Wybierz program", QDir::currentPath(), "Programy (*.exe)");
	if(file.isNull())
		return;

	QString relativePath = QDir::current().relativeFilePath(file);
	m_Action.addProcess(aq_qtutils::CActionProcess(relativePath.toStdString()));

	QListWidgetItem* item = new QListWidgetItem(relativePath);
	m_ProcessList->addItem(item);
	m_ProcessList->setCurrentItem(item);
}

void ActionDefinitionWidget::duplicateProcess()
{
	aq_qtutils::CActionProcess proc = m_Action.process(m_ProcessList->currentRow());
	m_Action.addProcess(proc);

	QListWidgetItem* item = new QListWidgetItem(proc.getApplication().c_str());
	m_ProcessList->addItem(item);
	m_ProcessList->setCurrentItem(item);
}

void ActionDefinitionWidget::removeProcess()
{
	if(m_ProcessList->currentItem() != nullptr)
	{
		int index = m_ProcessList->currentRow();
		m_ProcessList->blockSignals(true);
		m_Action.removeProcess(index);
		delete m_ProcessList->takeItem(index);
		m_ProcessList->blockSignals(false);

		currentProcessChanged(m_ProcessList->currentItem(), nullptr);
	}
}

void ActionDefinitionWidget::currentProcessChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
	m_RemoveProcess->setEnabled(current != nullptr);
	m_DuplicateProcess->setEnabled(current != nullptr);
	m_ProcessParameters->setEnabled(current != nullptr);
	if (current == nullptr)
	{
		m_ParameterList->setRowCount(0);
		return;
	}

	m_ParameterList->blockSignals(true);
	m_ParameterList->setRowCount(0);
	aq_qtutils::CActionProcess& proc = m_Action.process(m_ProcessList->row(current));
	m_ParameterList->setRowCount(proc.parameterCount());
	for(int i = 0; i < proc.parameterCount(); ++i)
	{
		for(int j = 0; j < 2; ++j)
		{
			if(m_ParameterList->item(i, j) == nullptr)
				m_ParameterList->setItem(i, j, new QTableWidgetItem());
		}

		m_ParameterList->item(i, 0)->setText(proc.parameter(i).first.c_str());
		m_ParameterList->item(i, 1)->setText(proc.parameter(i).second.c_str());
	}
	m_ParameterList->blockSignals(false);

	m_MachineIp->setText(proc.getMachineIp().c_str());
}

void ActionDefinitionWidget::setAction(const aq_qtutils::CAction& action)
{
	m_Action = action;

	m_ProcessList->clear();
	for(auto it = m_Action.processBegin(); it != m_Action.processEnd(); ++it)
		m_ProcessList->addItem(it->getApplication().c_str());

	if (m_ProcessList->count() > 0)
		m_ProcessList->setCurrentRow(0);
}

void ActionDefinitionWidget::addParameter()
{
	m_Action.process(m_ProcessList->currentRow()).addParameter("", "");

	m_ParameterList->setRowCount(m_ParameterList->rowCount() + 1);

	int row = m_ParameterList->rowCount() - 1;
	if (m_ParameterList->item(row, 0) == nullptr)
		m_ParameterList->setItem(row, 0, new QTableWidgetItem());
	if (m_ParameterList->item(row, 1) == nullptr)
		m_ParameterList->setItem(row, 1, new QTableWidgetItem());

	m_ParameterList->setCurrentCell(row, 0);
}

void ActionDefinitionWidget::removeParameter()
{
	m_Action.process(m_ProcessList->currentRow()).removeParameter(m_ParameterList->currentRow());
	m_ParameterList->removeRow(m_ParameterList->currentRow());
}

void ActionDefinitionWidget::parameterChanged(int row, int column)
{
	m_RemoveParameter->setEnabled(m_ProcessList->currentItem() != nullptr);
	if(m_ProcessList->currentItem() == nullptr)
		return;

	if(column == 0)
		m_Action.process(m_ProcessList->currentRow()).parameter(row).first = m_ParameterList->item(row, column)->text().toStdString();
	else
		m_Action.process(m_ProcessList->currentRow()).parameter(row).second = m_ParameterList->item(row, column)->text().toStdString();
}

void ActionDefinitionWidget::currentParameterChanged(QTableWidgetItem* current, QTableWidgetItem* previous)
{
	m_RemoveParameter->setEnabled(current != nullptr);
}

void ActionDefinitionWidget::machineIpChanged()
{
	if (m_ProcessList->currentItem() == nullptr)
		return;

	m_Action.process(m_ProcessList->currentRow()).setMachineIp(m_MachineIp->text().toStdString());
}
