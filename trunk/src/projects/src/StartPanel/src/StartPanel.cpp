#include "StartPanel.h"
#include "Aquisition.h"
#include "AquisitionWizard.h"

#include "config/Directory.h"
#include "config/Camera.h"

#include "utils/Filesystem.h"

#include <QtCore/QProcess>
#include <QtWidgets/QMessageBox>

StartPanel::StartPanel(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	m_ConfigDialog = new ConfigDialog(this);
}

StartPanel::~StartPanel()
{

}

void StartPanel::showConfigDialog()
{
	m_ConfigDialog->exec();
}

void StartPanel::runAnnotationEditor()
{
	QProcess::startDetached("EditorAnnotation.exe");
}

void StartPanel::runActionTraining()
{
	if (QMessageBox::warning(this, "Confirmation", "All training data will be lost!\nProceed with new training session?", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
		QProcess::startDetached("TrainManager.exe");
}

void StartPanel::runAquisition()
{
	Aquisition aquisition;
	aquisition.exec();
}

void StartPanel::runMonitor()
{
	AquisitionWizard wizard(this, false);
	wizard.load("monitor_wizard.cfg");
	if (wizard.exec() == QWizard::Rejected)
		return;
	wizard.save("monitor_wizard.cfg");

	config::Directory::getInstance().load();
	std::string path = config::Directory::getInstance().getTemporaryPath();
	utils::Filesystem::removeContents(path);

	std::string command = "Monitor.exe";
	command += " --if \"" + path + '\"';

	if (wizard.getCameraTypePage()->getCameraType() == CameraTypePage::AXIS)
	{
		int index = wizard.getAxisCameraPage()->getCameraIndex();

		config::Camera& cameraConfig = config::Camera::getInstance();
		cameraConfig.load();
		command += " --user " + cameraConfig.getUser(index);
		command += " --pass " + cameraConfig.getPassword(index);
		command += " --ip " + cameraConfig.getIp(index);
		command += " --mask \"" + cameraConfig.getName(index) + '\"';
	}
	else
	{
		command += " --guid " + wizard.getMilestoneCameraPage()->getCameraGuid();
		command += " --mask \"" + wizard.getMilestoneCameraPage()->getCameraName() + '\"';
	}

	QProcess::startDetached(command.c_str());
}
