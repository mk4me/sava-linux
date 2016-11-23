#pragma once

#include "EditorSingletons.h"
#include "EditorSceneLayer.h"
#include "EditorSceneTrajectoryPoint.h"
#include "EditorSceneTrajectoryGroup.h"
#include <QtGui/QKeyEvent>

class EditorSceneTrajectory;


/**
 * \class EditorSceneTrajectoryLayer
 *
 * \brief Represents trajectory graphic layer in editor scene. 
 *
 */
class EditorSceneTrajectoryLayer: public EditorSceneLayer, public EditorMessageListener
{
	Q_OBJECT

public:
	EditorSceneTrajectoryLayer(const EditorLayerPtr& i_SourceLayer);
	virtual ~EditorSceneTrajectoryLayer();

public slots:
	void selectCurrentTimePoint();
	void unselectAllPoints();

protected:
	virtual void init() override;
	virtual void redraw() override;

	virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
	virtual void keyPressEvent(QKeyEvent *event) override;

	virtual void createElement(int id) override;
	virtual void selectElement(int id) override;
	virtual void deleteElement(int id) override;

	virtual void onMessage(EditorMessageType i_Message, int i_ExtraParam = -1) override;

private:
	void addNewPoint(int x, int y);
	QPixmap getPrevTrajPixmap(bool& o_PathIdChanged, const QPoint& o_Position = QPoint() ) const;

	EditorSceneTrajectoryGroup* getTrajectoryGroupById(int id);

private:
	//cached trajectory manager pointer
	EditorTrajectoryManager* m_TrajectoryManager;

	//current choosen trajectory group
	EditorSceneTrajectoryGroup* m_CurrentGroup;

	//map with all group (id->group)
	std::map<int, EditorSceneTrajectoryGroup*> m_SceneTrajectoryMap;

	//bie��ca nazwa wy�wietlanej trajektorii
	std::string m_CurrentPartName;



	/**
	* \class TrajectoriePointViewItem
	*
	* \brief Represents previous trajectory point with object on trajectory layer.
	*
	*/
	class TrajectoriePointViewItem : public QGraphicsPixmapItem
	{

	public:
		TrajectoriePointViewItem(QGraphicsItem* parent) : QGraphicsPixmapItem(parent) {}
		void setFixedPos(const QPointF& point)
		{
			setPos(point);
			m_FixedPos = point;
		}

	protected:
		virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override
		{
			QPointF last = event->lastScenePos();
			QPointF curr = event->scenePos();
			QPointF diffrent = curr - last;
			setOffset(offset().x() + diffrent.x(), offset().y() + diffrent.y());

			QGraphicsPixmapItem::mouseMoveEvent(event);

			setPos(m_FixedPos);
		}

		virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override
		{
			EditorSceneTrajectoryLayer* layer = dynamic_cast<EditorSceneTrajectoryLayer*>(parentItem());
			if (layer)
				layer->unselectAllPoints();

			QGraphicsPixmapItem::mousePressEvent(event);
		}

	private:
		QPointF m_FixedPos;
	};


	//graficzny item, na kt�rym rysujemy poprzedni zazanczony punkt wraz z danym obiektem
	TrajectoriePointViewItem* m_PrevTrajectoryPixmapItem;
};