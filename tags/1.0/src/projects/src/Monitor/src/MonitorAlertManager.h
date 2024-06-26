#pragma once

#include <vector>
#include <map>
#include <deque>

#include "utils/Singleton.h"
#include "config/Glossary.h"
#include "sequence/Action.h"

#include "MonitorAlert.h"
#include "MonitorActionAlert.h"
#include "MonitorRegionAlert.h"
#include "MonitorActionManager.h"
#include "MonitorVideoManager.h"
#include "MonitorRegionsManager.h"
#include "MonitorFrames.h"
#include "MonitorAlertSaver.h"


class IAlertListener {
public:
	virtual void onAlertStart(const MonitorAlertPtr& i_Alert) = 0;
	virtual void onAlertEnd(const MonitorAlertPtr& i_Alert) = 0;
	virtual void onAlertDelete(const MonitorAlertPtr& i_Alert) = 0;
};


/// <summary>
/// Klasa zarz¹dzaj¹ca wszystkimi alertami w Monitorze.
/// </summary>
/// <seealso cref="Singleton{MonitorAlertManager}" />
/// <seealso cref="IActionListener" />
/// <seealso cref="IRegionListener" />
class MonitorAlertManager : public QObject, public Singleton<MonitorAlertManager>, public IActionListener, public IRegionListener
{
	Q_OBJECT

	typedef sequence::IVideo::Timestamp Timestamp;

public:

	MonitorAlertManager();
	~MonitorAlertManager();

	void load();
	void save() const {}

	void onVideoPreload();
	void onVideoLoaded();

	MonitorAlertPtr loadAlert(MonitorAlert::EType type, const std::string& filename) const;
	void saveAlert(const MonitorAlertPtr& alert) const;

	void addAlert(const MonitorAlertPtr& alert);
	bool removeAlert(const MonitorAlertPtr& alert);
	int getAlertId(const MonitorAlertPtr& alert) const;
	size_t getAlertsCounts() const;
	MonitorAlertPtr getAlertById(size_t index) const;

	void addListener(IAlertListener* i_Listener);
	void removeListener(IAlertListener* i_Listener);

	void setEnabled(bool i_Enabled) { m_IsEnabled = i_Enabled; }
	bool isEnabled() const { return m_IsEnabled; }

	void acceptAllFinishedAlerts();
	void finishAllUnfinishedAlerts();
	void removeAllUnfinishedAlerts();

	static std::string getAlertFilePath(const MonitorAlertPtr& alert);
	static std::string getAlertFileName(const MonitorAlertPtr& alert);
	static std::string getAlertsDirPath() { return m_AlertsDirPath; }


private slots:
	void tryToRemoveAcceptedAlerts();
	void tryToRemoveOverflowAlerts();

protected:
	virtual void onActionStart(const MonitorVideoManager::ActionPair& i_ActionPair) override;
	virtual void onActionEnd(const MonitorVideoManager::ActionPair& i_ActionPair) override;

	virtual void onRegionEnter(const MonitorRegionPtr& i_Region) override;
	virtual void onRegionLeave(const MonitorRegionPtr& i_Region) override;

private:
	void saveAlertFiles(const MonitorAlertPtr& alert);
	void removeAlertFiles(const MonitorAlertPtr& alert);

	void notifyAlertStart(const MonitorAlertPtr& i_Alert);
	void notifyAlertEnd(const MonitorAlertPtr& i_Alert);
	void notifyAlertDelete(const MonitorAlertPtr& i_Alert);

	void addUnfinishedAlert(const MonitorAlertPtr& i_Alert);
	void removeUnfinishedAlert(const MonitorAlertPtr& i_Alert);

	void startAlert(const MonitorAlertPtr& data);
	void finishAlert(const MonitorAlertPtr& data);

	std::string getVideoAlertFileName() const;

	template <typename T, typename ArgType>
	MonitorAlertPtr create(const ArgType& arg);

	template <typename T> 
	MonitorAlertPtr getUnfinishedAlertByData(const T& data);

	template <typename T>
	std::shared_ptr<T> cast(const MonitorAlertPtr& alert) const;

private:
	//alerts enabled
	bool m_IsEnabled;

	//vector of alerts listeners
	std::vector<IAlertListener*> m_Listeners;

	//list of all alerts
	std::deque<MonitorAlertPtr> m_AlertsList;

	//list of unfinished actions
	std::vector<MonitorAlertPtr> m_UnfinishedAlertsList;

	//monitor alert saver object
	MonitorAlertSaver m_AlertSaver;

	mutable size_t m_NextAlertId;
	mutable size_t m_NextVideoId;

	size_t m_MaxAlertsCount;
	QTimer m_AcceptedAlertsRemoveTimer;
	boost::posix_time::time_duration m_HowLongAcceptedAlertsShouldStay;

	bool m_StoreAlertsEnabled;

	static std::string m_AlertsDirPath;
};


