#pragma once

#include "MonitorGraphicsTimeItem.h"
#include "sequence/CompressedVideo.h"
#include "sequence/IVideo.h"
#include "ReplayVideo.h"

class ReplayGraphicsTime :public MonitorGraphicsTimeItem
{
public:
	ReplayGraphicsTime(QGraphicsItem* parent = 0);
	//~ReplayGraphicsTime();

	void setVideo(const ReplayVideoPtr& _video) { m_Video = _video; }

protected:
	virtual boost::posix_time::ptime getTime(size_t _frame) const override;

private:
	ReplayVideoPtr m_Video;
};

