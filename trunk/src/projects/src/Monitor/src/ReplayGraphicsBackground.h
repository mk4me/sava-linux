#pragma once

#include "MonitorGraphicsBackground.h"
#include "ReplayVideo.h"


/// <summary>
/// Klasa wywietlaj¹ca obraz z kamery w oknie podgl¹du alertów.
/// </summary>
/// <seealso cref="MonitorGraphicsBackground" />
class ReplayGraphicsBackground : public MonitorGraphicsBackground
{

public:
	ReplayGraphicsBackground(QGraphicsItem *parent = 0);
	~ReplayGraphicsBackground();

	virtual void update(size_t _frame) override;

	void setVideo(const ReplayVideoPtr& _video) { m_Video = _video; }

private:
	ReplayVideoPtr m_Video;
};

