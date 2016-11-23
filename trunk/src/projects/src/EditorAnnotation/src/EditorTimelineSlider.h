#pragma once
#ifndef ANNOTATIONDEDITORTIMELINESLIDER_H
#define ANNOTATIONDEDITORTIMELINESLIDER_H

#include <QtWidgets/QSlider>
#include <QtGui/QMouseEvent>
#include "EditorMessageManager.h"
#include "EditorSingletons.h"

/**
* \class EditorTimelineSlider
*
* \brief Widget represents video time slider.
*
*/
class EditorTimelineSlider : public QSlider, public EditorMessageListener
{
	Q_OBJECT

public:
	EditorTimelineSlider(QWidget *parent);
	~EditorTimelineSlider();

public slots:
	void onValueChanged(int i_Value);

protected:
	virtual void mousePressEvent(QMouseEvent *) override;
	virtual void keyPressEvent(QKeyEvent *)  override;
	virtual void keyReleaseEvent(QKeyEvent *)  override;
	virtual void mouseMoveEvent(QMouseEvent *) override;

	virtual void onMessage(EditorMessageType i_Message, int i_ExtraParam = -1) override;

};

#endif // ANNOTATIONDEDITORTIMELINESLIDER_H
