#pragma once

#include "EditorSingletons.h"
#include <QtWidgets/QGraphicsItem>
#include "EditorSceneTrajectory.h"
#include "EditorMessageManager.h"
#include "EditorTrajectoryGroup.h"
#include <map>

class EditorSceneTrajectory;

/**
* \class EditorSceneTrajectoryGroup
*
* \brief Graphics item in scene that represents group trajectory.
*
*/
class EditorSceneTrajectoryGroup : public QGraphicsItem
{
public:
	EditorSceneTrajectoryGroup(const EditorTrajectoryGroupPtr& source, QGraphicsItem* parent = 0);
	~EditorSceneTrajectoryGroup();

	void onSelect();
	void onUnselect();

	void selectPart(const std::string& name);
	void addPart(const std::string& name);
	bool existPart(const std::string& name);

	EditorSceneTrajectory* getCurrentTrajectory() const { return m_CurrentSceneTrajectory; }

private:
	void goToStartTime(const EditorSceneTrajectory* traj);

protected:
	virtual QRectF boundingRect() const override;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override {}

private:
	EditorTrajectoryGroupPtr m_Source;
	EditorSceneTrajectory* m_CurrentSceneTrajectory;
	std::map < std::string, EditorSceneTrajectory* > m_SceneTrajectories;
};
