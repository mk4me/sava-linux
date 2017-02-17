#include "EditorPopup.h"
#include <QtCore/QDebug>

/*--------------------------------------------------------------------*/
EditorPopup::EditorPopup(QWidget *parent)
: QWidget(parent)
{
	ui.setupUi(this);
	ui.messageText->setWordWrap(true);

	m_Timer = new QTimer(this);
	m_Timer->setSingleShot(true);
	connect(m_Timer, SIGNAL(timeout()), this, SLOT(hideMe()));

	hide();
}

/*--------------------------------------------------------------------*/
EditorPopup::~EditorPopup(){
}

/*--------------------------------------------------------------------*/
void EditorPopup::show(const QString& msg, int interval_milisec)
{
	ui.messageText->setText(msg);
	move(parentWidget()->width() - width() - 10, parentWidget()->height() - height() - 10);
	QWidget::show();

	m_Timer->stop();
	m_Timer->start(interval_milisec);
}

/*--------------------------------------------------------------------*/
void EditorPopup::hideMe()
{
	m_Timer->stop();
	hide();
}
