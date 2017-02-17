#include "EditorTimelineSlider.h"
#include "QtCore/QDebug"
#include "EditorTimeline.h"
#include "QtWidgets/QApplication"

/*--------------------------------------------------------------------*/
EditorTimelineSlider::EditorTimelineSlider(QWidget *parent)
	: QSlider(parent)
{
	connect(this, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged(int)));

	EditorMessageManager::registerMessage(MESSAGE_FRAME_CHANGED, this);
	EditorMessageManager::registerMessage(MESSAGE_SEQUENCE_LOADED, this);
}

/*--------------------------------------------------------------------*/
EditorTimelineSlider::~EditorTimelineSlider()
{
	EditorMessageManager::unregisterMessages(this);
}

/*--------------------------------------------------------------------*/
void EditorTimelineSlider::mousePressEvent(QMouseEvent * event)
{
	if (event->button() == Qt::LeftButton)
	{
		int newValue = minimum() + ((maximum() - minimum()) * event->x()) / width();
		setValue(newValue);
	}	
}

/*--------------------------------------------------------------------*/
void EditorTimelineSlider::mouseMoveEvent(QMouseEvent *event)
{
	if (QApplication::mouseButtons() & Qt::LeftButton)
	{
		int newValue = minimum() + ((maximum() - minimum()) * event->x()) / width();
		setValue(newValue);
	}

	QSlider::mouseMoveEvent(event);
}

/*--------------------------------------------------------------------*/
void EditorTimelineSlider::keyPressEvent(QKeyEvent *event){
	QWidget::keyPressEvent(event);
}

/*--------------------------------------------------------------------*/
void EditorTimelineSlider::keyReleaseEvent(QKeyEvent *event){
	QWidget::keyReleaseEvent(event);
}

/*--------------------------------------------------------------------*/
void EditorTimelineSlider::onValueChanged(int i_Value){
	EditorTimeline::getInstance().setCurrentFrame(i_Value);
}

/*--------------------------------------------------------------------*/
void EditorTimelineSlider::onMessage(EditorMessageType i_Message, int i_ExtraParam /*= -1*/)
{
	blockSignals(true);

	switch (i_Message)
	{
		case MESSAGE_SEQUENCE_LOADED:
			setRange(0, EditorTimeline::getInstance().getFrameCount() - 1);
			break;

		case MESSAGE_FRAME_CHANGED:
			setValue(EditorTimeline::getInstance().getCurrentFrame());
			break;
	}

	blockSignals(false);
}
