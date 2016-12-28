#include "AlertDialog.h"
#include <QtCore/QDebug>
#include <QtWidgets/QCheckBox>


AlertDialog::AlertDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	Qt::WindowFlags flags = windowFlags();
	Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
	flags = flags & (~helpFlag);
	setWindowFlags(flags);

	init();
}


AlertDialog::~AlertDialog()
{

}


void AlertDialog::init()
{
	auto actions = MonitorActionManager::getInstance().getActionsNames();
	for (std::string s : actions) 
	{
		auto actionIds = MonitorActionManager::getInstance().getActionIds(s);
		QList<QVariant> qActionIds;
		for (int id : actionIds)
			qActionIds.push_back(id);

		QListWidgetItem* item = new QListWidgetItem();
		item->setSizeHint(QSize(item->sizeHint().width(), 25));

		QCheckBox* box = new QCheckBox(s.c_str());
		box->setProperty("row", qActionIds);
		box->setStyleSheet("QCheckBox { font-size: 10pt; } ");

		ui.actionListWidget->addItem(item);
		ui.actionListWidget->setItemWidget(item, box);

		connect(box, SIGNAL(stateChanged(int)), this, SLOT(onItemStateChanged(int)));
	}

	select();
}


void AlertDialog::select()
{
	std::set<size_t> selectedActionIds = MonitorActionManager::getInstance().getAlertsIds();

	for (int i = 0; i < ui.actionListWidget->count(); i++)
	{
		bool checked = false;

		QCheckBox* box = static_cast<QCheckBox*>(ui.actionListWidget->itemWidget(ui.actionListWidget->item(i)));

		if (!selectedActionIds.empty())
		{
			QList<QVariant> ids = box->property("row").toList();
			for (QVariant qV : ids)
			{
				auto it = selectedActionIds.find(qV.toInt());
				if (it != selectedActionIds.end())
				{
					checked = true;
					selectedActionIds.erase(it);
				}
			}
		}

		box->setCheckState(checked ? Qt::Checked : Qt::Unchecked);
	}

	update_AllCheckBox();
}

void AlertDialog::onItemStateChanged(int state)
{
	QCheckBox* box = static_cast<QCheckBox*>(sender());
	QList<QVariant> ids = box->property("row").toList();

	for (QVariant qV : ids)
	{
		if (state == Qt::Checked)
			MonitorActionManager::getInstance().addAlert(qV.toInt());
		else
			MonitorActionManager::getInstance().removeAlert(qV.toInt());
	}

	update_AllCheckBox();
}


void AlertDialog::update_AllCheckBox()
{
	disconnect(ui.selectAllCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onSelectAllChanged(int)));

	bool checked = true;
	for (int i = 0; i < ui.actionListWidget->count(); i++)
	{
		QCheckBox* box = static_cast<QCheckBox*>(ui.actionListWidget->itemWidget(ui.actionListWidget->item(i)));
		checked = box->isChecked();
		if (!checked)
			break;
	}

	ui.selectAllCheckBox->setCheckState(checked ? Qt::Checked : Qt::Unchecked);

	connect(ui.selectAllCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onSelectAllChanged(int)));
}

void AlertDialog::onSelectAllChanged(int state)
{
	for (int i = 0; i < ui.actionListWidget->count(); i++) {
		auto item = ui.actionListWidget->item(i);
		QCheckBox* box = static_cast<QCheckBox*>(ui.actionListWidget->itemWidget(item));
		box->setCheckState(state ? Qt::Checked : Qt::Unchecked);
	}
}
