#include "AlertDialog.h"
#include <QDebug>
#include <QCheckBox>


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
	const std::vector<std::string>& actions = config::Glossary::getInstance().getTrainedActions();
	for (std::string s : actions) 
	{
		QListWidgetItem* item = new QListWidgetItem();
		item->setSizeHint(QSize(item->sizeHint().width(), 25));

		QCheckBox* box = new QCheckBox(s.c_str());
		box->setProperty("row", ui.actionListWidget->count());
		box->setStyleSheet("QCheckBox { font-size: 10pt; } ");

		ui.actionListWidget->addItem(item);
		ui.actionListWidget->setItemWidget(item, box);

		connect(box, SIGNAL(stateChanged(int)), this, SLOT(onItemStateChanged(int)));
	}

	select();
}


void AlertDialog::select()
{
	for (int i = 0; i < ui.actionListWidget->count(); i++)
	{
		bool checked = MonitorActionManager::getInstance().isAlert(i);
		QCheckBox* box = static_cast<QCheckBox*>(ui.actionListWidget->itemWidget(ui.actionListWidget->item(i)));
		box->setCheckState(checked ? Qt::Checked : Qt::Unchecked);
	}

	update_AllCheckBox();
}

void AlertDialog::onItemStateChanged(int state)
{
	QCheckBox* box = static_cast<QCheckBox*>(sender());
	int row = box->property("row").toInt();

	if (state == Qt::Checked)
		MonitorActionManager::getInstance().addAlert(row);
	else
		MonitorActionManager::getInstance().removeAlert(row);

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
