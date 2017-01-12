#pragma once

#include "MonitorAlert.h"
#include "sequence/Action.h"
#include "sequence/Cluster.h"

/// <summary>
/// Klasa opisuj¹ca alert akcji.
/// </summary>
/// <seealso cref="MonitorAlertData{MonitorVideoManager::ActionPair}" />
class MonitorActionAlert : public MonitorAlertData < MonitorVideoManager::ActionPair >
{

public:
	MonitorActionAlert();
	MonitorActionAlert(const MonitorVideoManager::ActionPair& i_Data);
	MonitorActionAlert(const std::string& filename);

	virtual ~MonitorActionAlert();

	virtual MonitorAlert::EType getType() const { return ACTION; }

	virtual void start() override;
	virtual void finish() override;

	virtual QPolygonF getRegion(size_t _frame) override;
	virtual QPolygonF getRegion() override;
};

typedef std::shared_ptr<MonitorActionAlert> MonitorActionAlertPtr;

