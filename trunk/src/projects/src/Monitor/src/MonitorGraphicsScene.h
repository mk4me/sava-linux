#ifndef VIDEOGRAPHICSSCENE_H
#define VIDEOGRAPHICSSCENE_H

#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGraphicsPixmapItem>
#include "QtGui/QPainter"

#include "utils/ImageConverter.h"
#include "MonitorVideoManager.h"
#include "MonitorGraphicsTimeItem.h"

#include "MonitorGraphicsRegionGroup.h"
#include "MonitorGraphicsClusters.h"
#include "QtWidgets/QStyleOption"
#include "MonitorGraphicsBackground.h"
#include "MonitorGraphicsWaitItem.h"


class MonitorGraphicsScene : public QGraphicsScene
{
	Q_OBJECT

public:

	enum EState
	{
		NONE,
		ALERT_REGION_EDITABLE,
		MASK_REGION_EDITABLE,
	};

	MonitorGraphicsScene(QObject *parent = 0);
	~MonitorGraphicsScene();

	void init();
	void onVideoPreload();
	void onVideoLoaded();
	void update(size_t frame);

	void setState(EState _state);
	EState getState() const { return m_State; }
	
	void setVideoVisible(bool i_Visible);

protected:
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
	virtual void keyPressEvent(QKeyEvent *event) override;
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
	MonitorVideoManager* m_CachedVideoManger;

	//item to draw background
	MonitorGraphicsBackground m_BackgroundImage;

	//item of all clusters
	MonitorGraphicsClusters m_ClustersItem;

	//item to draw frame time
	MonitorGraphicsTimeItem m_TimeItem;
	
	//wait item
	MonitorGraphicsWaitItem m_WaitItem;

	//alert regions group object
	MonitorGraphicsRegionGroup* m_AlertGroupRegion;

	//mask regions group object
	MonitorGraphicsRegionGroup* m_MaskGroupRegion;

	//scene state
	EState m_State;

	//is video visible
	bool m_IsVideoVisible;

};

#endif // VIDEOGRAPHICSSCENE_H
