#include "MilestoneAlertSender.h"
#include "MonitorFrames.h"

MilestoneAlertSender::MilestoneAlertSender()
	:m_IsEnabled(true)
{
	if (isAvailable())
	{
		MonitorAlertManager::getInstance().addListener(this);
		MonitorConfig::getInstance().addListener(this);
	}
	else
	{
		m_IsEnabled = false;
	}
}


MilestoneAlertSender::~MilestoneAlertSender()
{
}


bool MilestoneAlertSender::isAvailable() const {
	return MonitorPipe::getInstance().isMilestoneAvailable();
}

void MilestoneAlertSender::onAlertStart(const MonitorAlertPtr& i_Alert)
{
	if (!m_IsEnabled)
		return;

	if (i_Alert->getType() == MonitorAlert::ACTION)
	{
		MonitorActionAlertPtr actionAlert = std::dynamic_pointer_cast<MonitorActionAlert>(i_Alert);
		if (actionAlert)
		{
			auto video = MonitorVideoManager::getInstance().getVideo();
			auto cluster = actionAlert->getData().first;
			auto action = actionAlert->getData().second;

			MonitorPipe::getInstance().sendMilestoneAlert(*video.get(), *cluster.get(), *action.get());
		}
	}
	else if (i_Alert->getType() == MonitorAlert::REGION)
	{
		QRectF qRect = i_Alert->getRegion().boundingRect();

		cv::Rect2d roi(qRect.x(), qRect.y(), qRect.width(), qRect.height());
		roi = normalize(roi);

		std::ostringstream oss;
		oss << "SAVA Monitoring System - Enter Collision Region  \n";
		oss << "Position: " << roi << "\n";
		std::string desc = oss.str();

		MonitorPipe::getInstance().sendAlert("ManEnter", i_Alert->getStartTime(), roi, desc);
		
	}
}

void MilestoneAlertSender::onAlertEnd(const MonitorAlertPtr& i_Alert)
{
	if (!m_IsEnabled)
		return;

	if (i_Alert->getType() == MonitorAlert::REGION)
	{
		QRectF qRect = i_Alert->getRegion().boundingRect();

		cv::Rect2d roi(qRect.x(), qRect.y(), qRect.width(), qRect.height());
		roi = normalize(roi);

		std::ostringstream oss;
		oss << "SAVA Monitoring System - Leave Collision Region  \n";
		oss << "Position: " << roi << "\n";
		std::string desc = oss.str();

		MonitorPipe::getInstance().sendAlert("ManLeave", i_Alert->getStartTime(), roi, desc);
	}
}

void MilestoneAlertSender::onLoad(MonitorConfig* config)
{
	m_IsEnabled = config->isMilestoneEnabled();
}

void MilestoneAlertSender::onSave(MonitorConfig* config)
{
	config->setMilestoneEnabled(m_IsEnabled);
}

cv::Rect2d MilestoneAlertSender::normalize(const cv::Rect2d& i_Rect)
{
	cv::Mat frame = MonitorVideoManager::getInstance().getVideo()->getFrameImage(0);
	cv::Size2d frameSize(frame.cols, frame.rows);
	cv::Rect2d roi(i_Rect.x / frameSize.width, i_Rect.y / frameSize.height, 
		i_Rect.width / frameSize.width, i_Rect.height / frameSize.height);

	return roi;
}
