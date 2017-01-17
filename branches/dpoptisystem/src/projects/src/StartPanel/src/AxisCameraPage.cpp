#include "AxisCameraPage.h"

#include "config/Camera.h"

AxisCameraPage::AxisCameraPage(QWidget *parent)
	: QWizardPage(parent)
{
	ui.setupUi(this);

	config::Camera& camera = config::Camera::getInstance();
	camera.load();
	
	for (int i = 0; i < camera.getNumCameras(); ++i)
		ui.m_Cameras->addItem((camera.getName(i) + " (" + camera.getIp(i) + ")").c_str());
	if (ui.m_Cameras->count() > 0)
		ui.m_Cameras->setCurrentRow(0);
}

AxisCameraPage::~AxisCameraPage()
{

}
