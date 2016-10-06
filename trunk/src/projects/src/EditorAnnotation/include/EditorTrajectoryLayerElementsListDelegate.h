#pragma once

#include "EditorLayerElementsListDelegate.h"
#include "EditorTrajectoryGroup.h"
#include "QtGui/QPainter"

class EditorLayerElementsList;

/**
* \class EditorTrajectoryLayerElementsListDelegate
*
* \brief Delegate for EditorTrajectoryLayerElementsList
*
*/
class EditorTrajectoryLayerElementsListDelegate : public EditorLayerElementsListDelegate
{
	Q_OBJECT

public:
	EditorTrajectoryLayerElementsListDelegate(QObject *parent);

protected:
	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
	QRect getTrajSection(const EditorTrajectoryGroup* i_Traj, const QRect& i_PaintRect) const;

private:
	EditorLayerElementsList* m_ParentList;
};

