#pragma once

#include "MonitorAlert.h"
#include "MonitorRegion.h"

class MonitorRegionAlert : public MonitorAlertData<MonitorRegionPtr>
{
public:
	MonitorRegionAlert();
	MonitorRegionAlert(const MonitorRegionPtr& i_Data);
	MonitorRegionAlert(const std::string& filename);

	virtual ~MonitorRegionAlert();

	virtual MonitorAlert::EType getType() const { return REGION; }

	virtual void start() override;
	virtual void finish() override;

	virtual QPolygonF getRegion(size_t _frame) override;
	virtual QPolygonF getRegion() override;

private:
	void initRegion();

	MonitorRegion m_Region;

};

typedef std::shared_ptr<MonitorRegionAlert> MonitorRegionAlertPtr;
