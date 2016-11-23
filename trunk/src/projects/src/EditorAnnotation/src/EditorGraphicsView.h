#pragma once
#ifndef ANNOTATIONEDITORVIEW_H
#define ANNOTATIONEDITORVIEW_H

#include "QtWidgets/QGraphicsView"
#include "EditorScene.h"
#include "EditorSceneContextMenu.h"

/**
* \class EditorGraphicsView
*
* \brief View in editor window to show video scene (EditorScene).
*
*/
class EditorGraphicsView : public QGraphicsView
{
	Q_OBJECT

	static double SCALE_FACTOR;

public:
	EditorGraphicsView(QWidget *parent);
	~EditorGraphicsView();

	/** \brief redraw all graphics view content*/
	void redraw();

	/**
		\brief get view scale
		\return view scale
	*/
	double getScale() const { return m_Scale; }

signals:
	void updateRequest();

private slots:
	void onContextMenuRequest(const QPoint& point);
	//REMOVE void onSectionChanged(CSection* i_Section);
	void fitScene();

protected:
	virtual void resizeEvent(QResizeEvent *) override;
	virtual void showEvent(QShowEvent *) override;
	virtual void wheelEvent(QWheelEvent *) override;
	virtual void keyPressEvent(QKeyEvent *) override;
	virtual void keyReleaseEvent(QKeyEvent *) override;
	virtual void mousePressEvent(QMouseEvent *) override;

private:
	//editor scene (for render all visual content)
	EditorScene* m_EditorScene;

	//scene contex
	EditorSceneContextMenu* m_SceneContextMenu;

	//scene scale
	double m_Scale;

};

#endif // ANNOTATIONEDITORVIEW_H
