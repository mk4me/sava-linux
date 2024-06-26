#pragma once

#include <QtCore/QDebug>

#include "MonitorVideoManager.h"
#include "MonitorConfig.h"
#include "MonitorAlertManager.h"

/// <summary>
/// Klasa zarz¹dzaj¹ca komunikacj¹ miêdzy Monitorem,  a systemem Milestone. 
/// Sprawdza czy system Milestone jest dostêpny, oraz wysy³a do niego komunikaty o wykrytych alertach.
/// </summary>
/// <seealso cref="IAlertListener" />
/// <seealso cref="IConfigListener" />
class MilestoneAlertSender : public IAlertListener, public IConfigListener
{

public:
	MilestoneAlertSender();
	~MilestoneAlertSender();

	bool isAvailable() const;
	bool isEnabled() const { return m_IsEnabled; }
	void setEnabled(bool i_Enabled) { m_IsEnabled = i_Enabled; }

protected:
	virtual void onLoad(MonitorConfig* config) override;
	virtual void onSave(MonitorConfig* config) override;

private:
	cv::Rect2d normalize(const cv::Rect2d& i_Rect);

	virtual void onAlertStart(const MonitorAlertPtr& i_Alert) override;
	virtual void onAlertEnd(const MonitorAlertPtr& i_Alert) override;
	virtual void onAlertDelete(const MonitorAlertPtr& i_Alert) override {}

private:
	bool m_IsEnabled;

};

