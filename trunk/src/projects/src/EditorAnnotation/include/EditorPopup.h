#ifndef EDITORPOPUP_H
#define EDITORPOPUP_H

#include <QtWidgets/QDialog>
#include "QtCore/QTimer"
#include "ui_EditorPopup.h"

/**
* \class EditorPopup
*
* \brief Widget to show editor messages
*
*/
class EditorPopup : public QWidget
{
	Q_OBJECT

public:
	EditorPopup(QWidget *parent = 0);
	~EditorPopup();

public slots:
	/** /brief hide popup*/
	void hideMe();

	/** /brief show popup with message*/
	void show(const QString& msg, int interval_milisec = 2000);

private:
	Ui::EditorPopup ui;
	QTimer* m_Timer;
};

#endif // EDITORPOPUP_H
