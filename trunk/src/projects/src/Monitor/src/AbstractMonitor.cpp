#include "AbstractMonitor.h"

#include "MonitorLauncher.h"

#include <config/Glossary.h>
#include <config/Milestone.h>

#include <sequence/Action.h>
#include <sequence/Cluster.h>
#include <sequence/IVideo.h>

#include <utils/AlertSender.h>

AbstractMonitor::AbstractMonitor(const std::string& patternExtension, unsigned inputNumIndices /*= 1*/, bool hasOutput /*= true*/) 
	: utils::PipeProcess(patternExtension, inputNumIndices, hasOutput)
	, m_MilestoneConfigurationValid(false)	
{
	config::Glossary::getInstance().load();
	config::Milestone::getInstance().load();
}

AbstractMonitor::~AbstractMonitor()
{

}

void AbstractMonitor::registerParameters(ProgramOptions& programOptions)
{
	static bool registered = false;
	if (!registered)
	{
		utils::PipeProcess::registerParameters(programOptions);

		programOptions.add<std::string>("ip", "  camera IP string e.g. \"1.2.3.4\" (\"http://\" is added automatically)");
		programOptions.add<std::string>("user", "  camera user name");
		programOptions.add<std::string>("pass", "  camera password");
		programOptions.add<std::string>("guid", "  camera guid");
		programOptions.add<std::string>("mask", "  camera mask");
		registered = true;
	}
}

bool AbstractMonitor::loadParameters(const ProgramOptions& options)
{
	if (!utils::PipeProcess::loadParameters(options))
		return false;

	config::Milestone& milestoneConfig = config::Milestone::getInstance();
	if (!options.get<std::string>("user", m_User))
		m_User = milestoneConfig.getUser();
	if (!options.get<std::string>("pass", m_Password))
		m_Password = milestoneConfig.getPassword();
	if (!options.get<std::string>("ip", m_Ip))
		m_Ip = milestoneConfig.getIp();
	options.get<std::string>("guid", m_CameraGuid);
	if (!m_User.empty() && !m_Password.empty() && !m_Ip.empty() && !m_CameraGuid.empty())
	{
		utils::camera::MilestoneCredentials credentials(m_Ip, m_User, m_Password);
		m_AlertSender = std::make_shared<utils::camera::AlertSender>(credentials, m_CameraGuid);
	}
	options.get<std::string>("mask", m_Mask);
	m_Launcher = std::make_shared<MonitorLauncher>(m_Ip, m_User, m_Password, m_CameraGuid, m_Mask);
	m_Launcher->launchPipe();

	return true;
}

void AbstractMonitor::sendMilestoneAlert(sequence::IVideo& video, sequence::Cluster &cluster, sequence::Action &action)
{
	if (cluster.getStartFrame() == sequence::Cluster::NPOS || cluster.getEndFrame() == sequence::Cluster::NPOS)
	{
		// empty cluster file, skip
		return;
	}
	cv::Rect roi = cluster.getFrameAt(cluster.getStartFrame());
	boost::posix_time::ptime frameTime = video.getFrameTime(cluster.getStartFrame());
	cv::Mat frame = video.getFrameImage(cluster.getStartFrame());
	cv::Size2d frameSize(frame.cols, frame.rows);
	for (auto i = cluster.getStartFrame() + 1; i < cluster.getEndFrame(); ++i)
	{
		roi = roi | cluster.getFrameAt(i);
	}

	cv::Rect2d aoi(roi.x / frameSize.width, roi.y / frameSize.height, roi.width / frameSize.width, roi.height / frameSize.height);

	const std::string& actionName = config::Glossary::getInstance().getTrainedActionName(action.getActionId());
	if (!actionName.empty())
	{
		std::ostringstream oss;
		oss << "SAVA Monitoring System - Detected Action [" << actionName << "]\n";
		oss << "Action id: " << action.getActionId() << "\n";
		oss << "Position: " << roi << "\n";
		std::string desc = oss.str();

		asyncProcessMilestoneAlert(action.getActionId(), frameTime, aoi, desc);
	}
	else
	{
		std::cerr << "Can't get name for action id " << action.getActionId() << std::endl;
	}
}

void AbstractMonitor::processMilestoneAlert(int actionId, boost::posix_time::ptime beginTime, const cv::Rect2d& roi, const std::string& desc)
{
	if (!isMilestoneAvailable())
	{
		std::cerr << "Milestone isn't configured" << std::endl;
		return;
	}
	const std::string& actionName = config::Glossary::getInstance().getTrainedActionName(actionId);
	if (actionName.empty())
	{
		std::cerr << "Can't get name for action id " << actionId << std::endl;
		return;
	}
	if (!m_AlertSender->SendAlert(actionName, beginTime, desc, roi, "Gliwice"))
	{
		std::cerr << "Alert [" << actionName << "] sending error: " << m_AlertSender->GetLastErrorDesc() << std::endl;
	}
	else
	{
		std::cout << "Alarm " << actionName << " sent" << std::endl;
	}
}


void AbstractMonitor::asyncProcessMilestoneAlert(int actionId, boost::posix_time::ptime beginTime, const cv::Rect2d& roi, const std::string& desc)
{
	boost::thread t(&AbstractMonitor::processMilestoneAlert, this, actionId, beginTime, roi, desc);
}




void AbstractMonitor::sendAlert(const std::string& actionName, boost::posix_time::ptime beginTime, const cv::Rect2d& roi, const std::string& desc)
{
	boost::thread t(&AbstractMonitor::asyncSendAlert, this, actionName, beginTime, roi, desc);
}


void AbstractMonitor::asyncSendAlert(const std::string& actionName, boost::posix_time::ptime beginTime, const cv::Rect2d& roi, const std::string& desc)
{
	if (!isMilestoneAvailable())
	{
		std::cerr << "Milestone isn't configured" << std::endl;
		return;
	}

	m_AlertSender->SendAlert(actionName, beginTime, desc, roi, "Gliwice");
}

bool AbstractMonitor::isMilestoneAvailable() const
{
	return (bool)m_AlertSender;
}
