#pragma once

#include "MonitorGraphicsTimeItem.h"
#include "sequence/CompressedVideo.h"
#include "sequence/IVideo.h"
#include "ReplayVideo.h"

class ReplayGraphicsTime :public MonitorGraphicsTimeItem
{
public:
	ReplayGraphicsTime(QGraphicsItem* parent = 0);
	~ReplayGraphicsTime();

	virtual void update(size_t _frame) override;

	void setVideo(const ReplayVideoPtr& _video) { m_Video = _video; }

private:
	ReplayVideoPtr m_Video;
};

