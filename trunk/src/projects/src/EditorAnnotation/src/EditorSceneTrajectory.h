#pragma once

#include <QtWidgets/QGraphicsItem>
#include "EditorSceneTrajectoryPoint.h"
#include "EditorMessageManager.h"

#include <map>
#include "EditorTrajectory.h"

/**
* \class EditorSceneTrajectory
*
* \brief Graphics item in scene that represents trajectory.
*
*/
class EditorSceneTrajectory : public QGraphicsItem, public EditorMessageListener
{
public:
	EditorSceneTrajectory(const EditorTrajectoryPtr& sourceTrajectory, QGraphicsItem* parent = 0);
	~EditorSceneTrajectory();

	EditorSceneTrajectoryPoint* addPoint(int x, int y, float time = 0);
	void deleteSelectedPoint();
	void selectPointByTime(float i_time);
	void unselectAllPoint();

	QColor getColor() const { return m_Color; }

	EditorTrajectoryPtr getSource() const { return m_SourceTrajectory; }

protected:
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;
	virtual QRectF boundingRect() const override;

	void deletePoint(EditorSceneTrajectoryPoint* point);
	void addPoint(EditorSceneTrajectoryPoint* point);

	virtual void onMessage(EditorMessageType i_Message, int i_ExtraParam = -1) override;

private:
	EditorTrajectoryPtr m_SourceTrajectory;
	QColor m_Color;

	std::map<float, EditorSceneTrajectoryPoint*> m_ScenePoints;
};