#ifndef AQUISITION_H
#define AQUISITION_H

#include "ui_Aquisition.h"
#include "network/ProcessServer.h"

#include <vector>
#include <inttypes.h>

class AquisitionWizard;
class QPushButton;

class Aquisition : public QDialog
{
	Q_OBJECT

public:
	Aquisition(QWidget *parent = 0);
	~Aquisition();

	virtual int exec() override;

private slots:
	void stopRecord();
	void killPipe();

	void onProcessFinished(const QString& ip, const QString& tag, int exitCode, int exitStatus);

private:
	enum Status
	{
		RECORD,
		FINISH,
	};

	Ui::Aquisition ui;

	Network::ProcessServer m_ProcessServer;
	AquisitionWizard* m_Wizard;
	std::string m_OutDir;
	Status m_Status;

	QPushButton* m_CancelButton;
	QPushButton* m_AbortButton;

	virtual void closeEvent(QCloseEvent* event) override;

	bool startAll();

	bool recordVideo();
	bool compressVideo();
	bool detectObjects();
	bool predictActions();

	bool runMultiNode(const QString& description, const std::string& command, const std::string& tag, const std::vector<int64_t>& nodes);
	
	std::string getMilestoneCommand();
	std::string getAxisCommand();
	std::string getCameraName() const;

	std::string getDetectPathsCommand() const;
	bool detectClusters();

	std::string getComputeGbhCommand() const;
	std::string getComputeMbhCommand() const;
	std::string getRecognizeActionsCommand() const;

	static std::string getDateString();
};

#endif // AQUISITION_H
