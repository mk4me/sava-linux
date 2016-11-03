#ifndef MONITORLAUNCHER_H
#define MONITORLAUNCHER_H

#include <QtCore/QObject>

#include "network/ProcessServer.h"

class MonitorLauncher : public QObject
{
	Q_OBJECT

public:
	MonitorLauncher(const std::string& ip, const std::string& user, const std::string& password, const std::string& guid, const std::string& mask, QObject *parent = nullptr);
	~MonitorLauncher();
	
	bool launchPipe();

private:
	std::string m_Ip;
	std::string m_User;
	std::string m_Password;
	std::string m_Guid;
	std::string m_Mask;

	Network::ProcessServer m_ProcessServer;

	bool startPipe();
	bool runCommand(const QString& description, const std::string& command, const std::string& tag, const std::vector<__int64>& nodes);

	std::string getCompressVideoCommand() const;

	std::string getDetectPathsCommand() const;
	std::string getDetectClustersCommand() const;

	std::string getComputeGbhCommand() const;
	std::string getComputeMbhCommand() const;
	std::string getRecognizeActionsCommand() const;

	std::string getMilestoneCommand() const;
	std::string getAxisCommand() const;
};

#endif // MONITORLAUNCHER_H
