#ifndef EDITORTRAJECTORYWIDGET_H
#define EDITORTRAJECTORYWIDGET_H

#include <QtWidgets/QWidget>
#include "ui_EditorTrajectoryWidget.h"
#include "EditorMessageManager.h"
#include <QtWidgets/QRadioButton>

/**
* \class EditorTrajectoryWidget
*
* \brief Gui to manager all editor trajectory system functionality.
*
*/
class EditorTrajectoryWidget : public QWidget, public EditorMessageListener
{
	Q_OBJECT

public:
	EditorTrajectoryWidget(QWidget *parent = 0);
	~EditorTrajectoryWidget();

public slots:
	void onPartRadioButtonClicked(QRadioButton* radioButton = nullptr);
	void onTrajectoryListUpdate();
	void onShowAllStateChanged(int state);
	void onShowHelpViewChanged(int state);
	
protected:
	virtual void onMessage(EditorMessageType i_Message, int i_ExtraParam = -1) override;
	virtual void keyPressEvent(QKeyEvent *) override;
	virtual void keyReleaseEvent(QKeyEvent *) override;

private:
	void reloadPartsWidget();

	Ui::EditorTrajectoryWidget ui;

	/*---------------------------- RADIO BUTTON REPRESENTS TRAJECTORY PARTS ---------------------------------------*/
	class TrajectoryPartRadioButton : public QRadioButton
	{
		public:
			TrajectoryPartRadioButton(QWidget* parent = 0);
			void setColor(QColor color);
	};

};



#endif // EDITORTRAJECTORYWIDGET_H
