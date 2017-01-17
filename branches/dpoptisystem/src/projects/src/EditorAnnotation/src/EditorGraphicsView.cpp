#include "EditorGraphicsView.h"

double EditorGraphicsView::SCALE_FACTOR = 1.15;

/*--------------------------------------------------------------------*/
EditorGraphicsView::EditorGraphicsView(QWidget *parent)
	: QGraphicsView(parent),
	m_Scale(1)
{

	//init editor scene
	m_EditorScene = new EditorScene(this);
	setScene(m_EditorScene);

	//init context menu
	setContextMenuPolicy(Qt::CustomContextMenu);
	m_SceneContextMenu = new EditorSceneContextMenu(this);

	setTransformationAnchor(AnchorUnderMouse);

	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onContextMenuRequest(QPoint)));
	connect(m_SceneContextMenu, SIGNAL(actionDone()), this, SIGNAL(updateRequest()));
}

/*--------------------------------------------------------------------*/
EditorGraphicsView::~EditorGraphicsView()
{
}

/*--------------------------------------------------------------------*/
void EditorGraphicsView::redraw(){
	m_EditorScene->redraw();
}

/*--------------------------------------------------------------------*/
void EditorGraphicsView::resizeEvent(QResizeEvent * event)
{
	if (m_Scale == 1)
		fitScene();
}

/*--------------------------------------------------------------------*/
void EditorGraphicsView::showEvent(QShowEvent *){
	fitScene();
}

/*--------------------------------------------------------------------*/
void EditorGraphicsView::onContextMenuRequest(const QPoint& point){
	m_SceneContextMenu->exec(mapToGlobal(point));
}

/*--------------------------------------------------------------------*/
void EditorGraphicsView::wheelEvent(QWheelEvent *event)
{
	//trzeba to przerobiæ w przysz³oœci (mog¹ byæ b³êdy w mno¿eniu double)
	double scaleFactor = SCALE_FACTOR;
	if (event->delta() < 0) {
		if (m_Scale <= 1) return;
		scaleFactor = 1.0 / scaleFactor;
	}

	m_Scale *= scaleFactor;
	if (m_Scale < SCALE_FACTOR) {
		m_Scale = 1;
		fitScene();
	}
	else {
		scale(scaleFactor, scaleFactor);
	}


	setDragMode(m_Scale > 1 ? ScrollHandDrag : NoDrag);

	
	event->ignore();
	//ovveride event, we want only to call children wheelEvent
	QWheelEvent overridEvent(QPoint(0, 0), 0, Qt::NoButton, Qt::NoModifier);
	QGraphicsView::wheelEvent(&overridEvent);
	
}

/*--------------------------------------------------------------------*/
void EditorGraphicsView::fitScene() {
	fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
	setDragMode(NoDrag);
	setCursor(Qt::ArrowCursor);
	m_Scale = 1;
}

/*--------------------------------------------------------------------*/
void EditorGraphicsView::keyReleaseEvent(QKeyEvent* e){


	bool isArrowKey = (e->key() == Qt::Key_Up) || (e->key() == Qt::Key_Down) ||
				   (e->key() == Qt::Key_Left) || (e->key() == Qt::Key_Right);

	if (isArrowKey){
		QWidget::keyReleaseEvent(e);
		return;
	}

	QGraphicsView::keyReleaseEvent(e);
}

/*--------------------------------------------------------------------*/
void EditorGraphicsView::mousePressEvent(QMouseEvent *e)
{
	if (e->buttons() == Qt::MiddleButton)
		fitScene();

	QGraphicsView::mousePressEvent(e);
}

/*--------------------------------------------------------------------*/
void EditorGraphicsView::keyPressEvent(QKeyEvent *e){

	bool isArrowKey = (e->key() == Qt::Key_Up) || (e->key() == Qt::Key_Down) ||
		(e->key() == Qt::Key_Left) || (e->key() == Qt::Key_Right);

	if (isArrowKey){
		QWidget::keyPressEvent(e);
		return;
	}

	QGraphicsView::keyPressEvent(e);
}

