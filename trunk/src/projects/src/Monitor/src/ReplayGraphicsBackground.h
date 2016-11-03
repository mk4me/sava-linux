#pragma once

#include "MonitorGraphicsBackground.h"
#include "ReplayVideo.h"


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

