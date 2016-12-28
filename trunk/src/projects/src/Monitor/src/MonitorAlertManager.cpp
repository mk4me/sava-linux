#include "MonitorAlertManager.h"
#include <QtCore/QDir>
#include <QtCore/QFileInfoList>

#include "config/Directory.h"
#include "config/Monitor.h"

MonitorAlertManager::MonitorAlertManager()
	:m_IsEnabled(true)
	, m_NextAlertId(0)
	, m_NextVideoId(0)
	, m_StoreAlertsEnabled(true)
{

	MonitorActionManager::getInstance().addListener(this);
	MonitorRegionsManager::getInstance().addListener(this);

	//read from config
	config::Directory& directoryConfig = config::Directory::getInstance();
	config::Monitor& monitorConfig = config::Monitor::getInstance();
	directoryConfig.load();
	monitorConfig.load();

	m_AlertsDirPath = directoryConfig.getAlertsPath().empty() ? 
		(utils::Filesystem::getUserPath() + "monitor/alerts") :
		directoryConfig.getAlertsPath();

	m_HowLongAcceptedAlertsShouldStay = boost::posix_time::hours((long)monitorConfig.getAlertsLifetime());
	m_MaxAlertsCount = monitorConfig.getMaxAlertsCount();

	//create alerts directory
	boost::filesystem::create_directories(m_AlertsDirPath);

	//load alerts 
	load();
	tryToRemoveAcceptedAlerts();
	tryToRemoveOverflowAlerts();

	//start remove timer
	connect(&m_AcceptedAlertsRemoveTimer, SIGNAL(timeout()), this, SLOT(tryToRemoveAcceptedAlerts()));
	m_AcceptedAlertsRemoveTimer.start(60000);

	m_AlertSaver.setSavePath(m_AlertsDirPath);
	m_AlertSaver.start();
}

MonitorAlertManager::~MonitorAlertManager()
{
	m_AlertSaver.stop();
}

void MonitorAlertManager::load()
{
	if (!m_StoreAlertsEnabled)
		return;

	QDir qDir(getAlertsDirPath().c_str());

	//find all alerts files (with *.alr extension)
	QFileInfoList alertsFiles = qDir.entryInfoList(QStringList() << "*.alr", QDir::Files);
	if (!alertsFiles.empty())
	{
		//iterate thruth all files and create alerts objects
		for (QFileInfo& fileInfo : alertsFiles)
		{
			std::string filePath = fileInfo.absoluteFilePath().toStdString();
			MonitorAlert::EType alertType = static_cast<MonitorAlert::EType>(fileInfo.completeBaseName().right(1).toInt());

			MonitorAlertPtr alert = loadAlert(alertType, filePath);
			if (alert) {
				addAlert(alert);
				//set as saved
				alert->saved();
			}
		}

		//sort alerts by its id
		std::sort(m_AlertsList.begin(), m_AlertsList.end(), [](const MonitorAlertPtr& a1, const MonitorAlertPtr& a2) {
			return a1->getId() > a2->getId();
		});

		//set next alert id
		m_NextAlertId = m_AlertsList.front()->getId() + 1;
	}


	//find all video files to get next video id
	QFileInfoList videosFiles = qDir.entryInfoList(QStringList() << "*.cvs", QDir::Files);
	std::vector<size_t> videosIds;
	for (QFileInfo& fileInfo : videosFiles)
	{
		int nr = fileInfo.completeBaseName().toInt();
		if (m_NextVideoId <= nr)
			m_NextVideoId = nr + 1;
	}
}


MonitorAlertPtr MonitorAlertManager::loadAlert(MonitorAlert::EType type, const std::string& filename) const
{
	switch (type)
	{
		case MonitorAlert::ACTION: return std::make_shared<MonitorActionAlert>(filename);
		case MonitorAlert::REGION: return std::make_shared<MonitorRegionAlert>(filename);
	}

	return nullptr;
}

void MonitorAlertManager::saveAlert(const MonitorAlertPtr& alert) const
{
	alert->save(getAlertFilePath(alert));
}

void MonitorAlertManager::onVideoPreload()
{
	if (!m_StoreAlertsEnabled)
		return;

	if (!m_UnfinishedAlertsList.empty())
	{
		std::string videoName = getVideoAlertFileName();

		for (auto& alert : m_UnfinishedAlertsList)
			alert->addVideo(videoName);
		
		//collect video to save
		m_AlertSaver.add(videoName, MonitorVideoManager::getInstance().getMetaVideo());
	}

	m_NextVideoId++;
}


void MonitorAlertManager::onVideoLoaded()
{
	m_AlertSaver.save();
}


void MonitorAlertManager::saveAlertFiles(const MonitorAlertPtr& alert)
{
	if (!m_StoreAlertsEnabled)
		return;

	std::string videoName = getVideoAlertFileName();
	std::string alertName = getAlertFileName(alert);
	alert->addVideo(videoName);

	m_AlertSaver.add(videoName, MonitorVideoManager::getInstance().getMetaVideo());
	m_AlertSaver.add(alertName, alert);
}


void MonitorAlertManager::addAlert(const MonitorAlertPtr& alert){
	m_AlertsList.push_front(alert);
}

bool MonitorAlertManager::removeAlert(const MonitorAlertPtr& alert)
{
	auto findIt = std::find(m_AlertsList.begin(), m_AlertsList.end(), alert);
	if (findIt != m_AlertsList.end())
	{
		removeAlertFiles(alert);
		m_AlertsList.erase(findIt);
		notifyAlertDelete(alert);
		return true;
	}
	
	return false;
}

size_t MonitorAlertManager::getAlertsCounts() const{
	return m_AlertsList.size();
}

MonitorAlertPtr MonitorAlertManager::getAlertById(size_t index) const
{
	if (index < m_AlertsList.size())
		return m_AlertsList.at(index);

	return nullptr;
}

int MonitorAlertManager::getAlertId(const MonitorAlertPtr& alert) const
{
	for (auto it = m_AlertsList.begin(); it != m_AlertsList.end(); ++it)
		if (alert == *it)
			return std::distance(m_AlertsList.begin(), it);

	return -1;
}

void MonitorAlertManager::addListener(IAlertListener* i_Listener){
	auto foundIt = std::find(m_Listeners.begin(), m_Listeners.end(), i_Listener);
	if (foundIt == m_Listeners.end())
		m_Listeners.push_back(i_Listener);
}


void MonitorAlertManager::removeListener(IAlertListener* i_Listener)
{
	auto foundIt = std::find(m_Listeners.begin(), m_Listeners.end(), i_Listener);
	if (foundIt != m_Listeners.end())
		m_Listeners.erase(foundIt);
}

void MonitorAlertManager::acceptAllFinishedAlerts()
{
	for (auto& alert : m_AlertsList)
		if (alert->isFinished())
			alert->accept();
}

void MonitorAlertManager::finishAllUnfinishedAlerts()
{
	while (!m_UnfinishedAlertsList.empty())
	{
		MonitorAlertPtr alert = m_UnfinishedAlertsList.front();
		alert->finish();
		removeUnfinishedAlert(alert);
		notifyAlertEnd(alert);
	}
}

void MonitorAlertManager::removeAllUnfinishedAlerts()
{
	while (!m_UnfinishedAlertsList.empty())
	{
		MonitorAlertPtr alert = m_UnfinishedAlertsList.front();
		removeUnfinishedAlert(alert);
		removeAlert(alert);
	}
}



void MonitorAlertManager::tryToRemoveAcceptedAlerts()
{
	auto currentTime = boost::posix_time::microsec_clock::local_time();
	std::vector<MonitorAlertPtr> alertsToRemove;

	for (auto it = m_AlertsList.rbegin(); it != m_AlertsList.rend(); ++it)
	{
		if (!(*it)->isAccepted())
			continue;
 
		if (currentTime - (*it)->getAcceptTime() < m_HowLongAcceptedAlertsShouldStay)
			break;

		alertsToRemove.push_back(*it);
	}

	for (MonitorAlertPtr& alert : alertsToRemove)
		removeAlert(alert);
}

void MonitorAlertManager::tryToRemoveOverflowAlerts()
{
	if (m_MaxAlertsCount > 0)
		while (m_AlertsList.size() > m_MaxAlertsCount)
		{
			MonitorAlertPtr lastAlert = m_AlertsList.back();
			removeAlert(lastAlert);
			removeUnfinishedAlert(lastAlert);
		}
}

void MonitorAlertManager::onActionStart(const MonitorVideoManager::ActionPair& i_ActionPair)
{
	if (!m_IsEnabled)
		return;

	MonitorAlertPtr alert = create<MonitorActionAlert>(i_ActionPair);
	startAlert(alert);
}

void MonitorAlertManager::onActionEnd(const MonitorVideoManager::ActionPair& i_ActionPair)
{
	if (!m_IsEnabled)
		return;

	MonitorAlertPtr alert = getUnfinishedAlertByData(i_ActionPair);
	finishAlert(alert);
}


void MonitorAlertManager::onRegionEnter(const MonitorRegionPtr& i_Region)
{
	if (!m_IsEnabled)
		return;

	//hack
	MonitorAlertPtr prevAlert = getUnfinishedAlertByData(i_Region);
	if (prevAlert)
		finishAlert(prevAlert);

	MonitorAlertPtr alert = create<MonitorRegionAlert>(i_Region);
	startAlert(alert);
}

void MonitorAlertManager::onRegionLeave(const MonitorRegionPtr& i_Region)
{
	if (!m_IsEnabled)
		return;

	MonitorAlertPtr alert = getUnfinishedAlertByData(i_Region);
	finishAlert(alert);
}



void MonitorAlertManager::notifyAlertStart(const MonitorAlertPtr& i_Alert)
{
	for (IAlertListener* listener : m_Listeners)
		listener->onAlertStart(i_Alert);
}

void MonitorAlertManager::notifyAlertEnd(const MonitorAlertPtr& i_Alert)
{
	for (IAlertListener* listener : m_Listeners)
		listener->onAlertEnd(i_Alert);
}

void MonitorAlertManager::notifyAlertDelete(const MonitorAlertPtr& i_Alert)
{
	for (IAlertListener* listener : m_Listeners)
		listener->onAlertDelete(i_Alert);
}

void MonitorAlertManager::addUnfinishedAlert(const MonitorAlertPtr& i_Alert)
{
	m_UnfinishedAlertsList.push_back(i_Alert);
}

void MonitorAlertManager::removeUnfinishedAlert(const MonitorAlertPtr& i_Alert)
{
	auto it = std::find(m_UnfinishedAlertsList.begin(), m_UnfinishedAlertsList.end(), i_Alert);
	if (it != m_UnfinishedAlertsList.end())
		m_UnfinishedAlertsList.erase(it);
}

void MonitorAlertManager::startAlert(const MonitorAlertPtr& alert)
{
	if (!alert)
		return;

	alert->start();

	addAlert(alert);
	addUnfinishedAlert(alert);
	notifyAlertStart(alert);

	tryToRemoveOverflowAlerts();
}


void MonitorAlertManager::finishAlert(const MonitorAlertPtr& alert)
{
	if (!alert)
		return;

	alert->finish();
	removeUnfinishedAlert(alert);
	notifyAlertEnd(alert);

	//save finished alert
	saveAlertFiles(alert);
}

void MonitorAlertManager::removeAlertFiles(const MonitorAlertPtr& alert)
{
	//remove alert file
	std::string alertFilePath = getAlertFilePath(alert);
	if (QFile::exists(alertFilePath.c_str()))
		QFile::remove(alertFilePath.c_str());

	//remove video file
	for (std::string s : alert->getVideos())
	{
		bool canRemove = true;

		//check if can remove video
		for (auto& a : m_AlertsList)
		{
			if (a == alert)
				continue;

			auto v = a->getVideos();
			if (std::find(v.begin(), v.end(), s) != v.end())
			{
				canRemove = false;
				break;
			}
		}

		if (canRemove)
		{
			std::string videoFullPath = getAlertsDirPath() + "/" + s;
			QFile::remove(videoFullPath.c_str());
		}
	}
}

std::string MonitorAlertManager::getVideoAlertFileName() const
{
	std::ostringstream v_oss;
	v_oss << m_NextVideoId << ".cvs";
	return v_oss.str();
}

std::string MonitorAlertManager::getAlertFilePath(const MonitorAlertPtr& alert)
{
	return m_AlertsDirPath + "/" + getAlertFileName(alert);
}

std::string MonitorAlertManager::getAlertFileName(const MonitorAlertPtr& alert)
{
	std::ostringstream a_oss;
	a_oss << alert->getId() << "." << alert->getType() << ".alr";
	return a_oss.str();
}

template <typename T, typename ArgType>
MonitorAlertPtr MonitorAlertManager::create(const ArgType& arg) {
	MonitorAlertPtr newAlert = std::make_shared<T>(arg);
	newAlert->setId(m_NextAlertId++);
	return newAlert;
}


template <typename T>
MonitorAlertPtr MonitorAlertManager::getUnfinishedAlertByData(const T& data)
{
	for (MonitorAlertPtr& alert : m_UnfinishedAlertsList)
	{
		std::shared_ptr<MonitorAlertData<T>> dataAlert = std::dynamic_pointer_cast<MonitorAlertData<T>>(alert);
		if (dataAlert && dataAlert->getData() == data)
			return alert;
	}

	return nullptr;
}

template <typename T>
std::shared_ptr<T>
MonitorAlertManager::cast(const MonitorAlertPtr& alert) const{
	return std::dynamic_pointer_cast<T>(alert);
}

std::string MonitorAlertManager::m_AlertsDirPath;

