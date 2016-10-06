#include "EditorTrajectoryWidget.h"
#include "QtCore/QDebug"
#include "EditorSceneLayerManager.h"
#include "EditorSceneLayerManager.h"
#include "EditorSceneTrajectoryLayer.h"

/*-------------------------------------------------------------------*/
EditorTrajectoryWidget::EditorTrajectoryWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ui.showAllCheckBox->setChecked(EditorConfig::getInstance().isTrajectoryPartsVisibled());
	ui.showHelpViewCheckBox->setChecked(EditorConfig::getInstance().isTrajectoryHelpViewVisibled());

	EditorMessageManager::registerMessage(MESSAGE_TRAJECTORY_CHANGED, this);
	EditorMessageManager::registerMessage(MESSAGE_CONFIG_CHANGED, this);

	reloadPartsWidget();
}

/*--------------------------------------------------------------------*/
EditorTrajectoryWidget::~EditorTrajectoryWidget()
{
	EditorMessageManager::unregisterMessages(this);
}

/*--------------------------------------------------------------------*/
void EditorTrajectoryWidget::onMessage(EditorMessageType i_Message, int i_ExtraParam /*= -1*/)
{
	switch (i_Message)
	{
		case MESSAGE_TRAJECTORY_CHANGED:
			update();
			break;

		case MESSAGE_CONFIG_CHANGED:
			reloadPartsWidget();
			ui.showAllCheckBox->setChecked(EditorConfig::getInstance().isTrajectoryPartsVisibled());
			ui.showHelpViewCheckBox->setChecked(EditorConfig::getInstance().isTrajectoryHelpViewVisibled());
			break;
	}
}

/*--------------------------------------------------------------------*/
void EditorTrajectoryWidget::reloadPartsWidget()
{
	QLayout* layout = ui.partsBox->layout();
	if (!layout) 
		return;

	//temp fields
	QString checkedName;
	QRadioButton* checkedButton = nullptr;
	std::vector<QRadioButton*> radioButtons;
	
	//first delete old buttons
	while (layout->count() > 0)
	{
		QLayoutItem* item = layout->takeAt(0);
		QRadioButton* radioButton = static_cast<QRadioButton*>(item->widget());
		if (radioButton->isChecked())
			checkedName = radioButton->text();

		delete item->widget();
		delete item;
	}

	//create new radioButtons in layout
	EditorTrajectoryParts parts = EditorConfig::getInstance().getTrajectoryParts();
	for (int i = 0; i < parts.count(); i++)
	{
		TrajectoryPartPtr part = parts.get(i);

		TrajectoryPartRadioButton* radioButton = new TrajectoryPartRadioButton(ui.partsBox);
		radioButton->setText(part->getName().c_str());
		radioButton->setColor(part->getColor());
		layout->addWidget(radioButton);

		radioButtons.push_back(radioButton);

		connect(radioButton, SIGNAL(clicked()), this, SLOT(onPartRadioButtonClicked()));
	}


	//set previously radio button
	if (checkedName.isEmpty() && !radioButtons.empty())
		checkedButton = radioButtons[0];
		
	if (!checkedButton)
		for each(QRadioButton* button in radioButtons)
		{
			if (button->text() == checkedName)
			{
				checkedButton = button;
				break;
			}
		}
	
	if (checkedButton)
	{
		checkedButton->setChecked(true);
		onPartRadioButtonClicked(checkedButton);
	}
}

/*--------------------------------------------------------------------*/
void EditorTrajectoryWidget::onPartRadioButtonClicked(QRadioButton* radioButton /*= nullptr*/)
{
	QRadioButton* button = radioButton ? radioButton : qobject_cast<QRadioButton*>(sender());
	if (button)
	{
		//notify that trajectory part has changed
		//std::string partName = button->text().toStdString();
		//EditorMessageManager::sendMessage(MESSAGE_TRAJECTORY_PART_CHANGED, partName);
	}
}

/*-------------------------------------------------------------------*/
void EditorTrajectoryWidget::onTrajectoryListUpdate()
{
	bool bIsItemSelected = ui.listView->currentIndex().isValid();
	ui.partsBox->setEnabled(bIsItemSelected);
}

/*-------------------------------------------------------------------*/
void EditorTrajectoryWidget::keyPressEvent(QKeyEvent *e)
{
	bool isArrowKey = (e->type() == Qt::Key_Up) || (e->type() == Qt::Key_Down);
	if (!isArrowKey)
		QWidget::keyPressEvent(e);
}

/*-------------------------------------------------------------------*/
void EditorTrajectoryWidget::keyReleaseEvent(QKeyEvent *e)
{
	bool isArrowKey = (e->type() == Qt::Key_Up) || (e->type() == Qt::Key_Down);
	if (!isArrowKey)
		QWidget::keyReleaseEvent(e);
}

/*-------------------------------------------------------------------*/
void EditorTrajectoryWidget::onShowAllStateChanged(int state) {
	EditorConfig::getInstance().setTrajectoryPartsVisibled(state == Qt::Checked);
}

/*-------------------------------------------------------------------*/
void EditorTrajectoryWidget::onShowHelpViewChanged(int state){
	EditorConfig::getInstance().setTrajectoryHelpViewVisibled(state == Qt::Checked);
}

/*------------------------- TAJECTORY RADIO BUTTON ------------------------------------------*/
EditorTrajectoryWidget::TrajectoryPartRadioButton::TrajectoryPartRadioButton(QWidget* parent /*= 0*/)
	:QRadioButton(parent)
{
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
}

/*-------------------------------------------------------------------*/
void EditorTrajectoryWidget::TrajectoryPartRadioButton::setColor(QColor c)
{
	QString sColor = "rgb(" + QString::number(c.red()) + "," + QString::number(c.green()) + "," + QString::number(c.blue()) + ")";
	setStyleSheet("QRadioButton { background-color: " + sColor + "; } QRadioButton::disabled { background-color: gray; }" );
}
