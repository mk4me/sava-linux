#pragma once
#ifndef AbstractMonitor_h__
#define AbstractMonitor_h__

#include <utils/PipeProcess.h>

#include <opencv2/core.hpp>

namespace utils
{
	namespace camera
	{
		class AlertSender;
	}
}

namespace sequence
{
	class IVideo;
	class Cluster;
	class Action;
}

class MonitorLauncher;

class AbstractMonitor : public utils::PipeProcess
{
public:
	AbstractMonitor(const std::string& patternExtension, unsigned inputNumIndices = 1, bool hasOutput = true);
	~AbstractMonitor();

	static void registerParameters(ProgramOptions& programOptions);

	virtual bool loadParameters(const ProgramOptions& options) override;

	void sendMilestoneAlert(sequence::IVideo& video, sequence::Cluster &cluster, sequence::Action &action);
	void sendAlert(const std::string& actionName, boost::posix_time::ptime beginTime, const cv::Rect2d& roi, const std::string& desc);
	bool isMilestoneAvailable() const;

private:
	void processMilestoneAlert(int actionId, boost::posix_time::ptime beginTime, const cv::Rect2d& roi, const std::string& desc);
	void asyncProcessMilestoneAlert(int actionId, boost::posix_time::ptime beginTime, const cv::Rect2d& roi, const std::string& desc);
	void asyncSendAlert(const std::string& actionName, boost::posix_time::ptime beginTime, const cv::Rect2d& roi, const std::string& desc);

private:

	std::string m_Ip;
	std::string m_User;
	std::string m_Password;
	std::string m_CameraGuid;
	std::string m_Mask;
	bool m_MilestoneConfigurationValid;

	std::shared_ptr<utils::camera::AlertSender> m_AlertSender;
	std::shared_ptr<MonitorLauncher> m_Launcher;
};

#endif // AbstractMonitor_h__