#pragma once
#include <QtWidgets/QGraphicsScene>
#include "ReplayGraphicsBackground.h"
#include "ReplayGraphicsAlert.h"
#include "ReplayGraphicsTime.h"

class ReplayWindow;

class ReplayGraphicsScene : public QGraphicsScene
{
public:
	ReplayGraphicsScene(QObject* parent = 0);
	~ReplayGraphicsScene();

	void init();
	void reload();
	void update(size_t _frame);

private:
	ReplayGraphicsBackground m_GraphicsBackground;
	ReplayGraphicsAlert m_GraphicsAlert;
	ReplayGraphicsTime m_GraphicsTime;

	ReplayWindow* m_ParentWindow;
};

