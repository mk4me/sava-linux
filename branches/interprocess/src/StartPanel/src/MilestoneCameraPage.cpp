#include "MilestoneCameraPage.h"

#include "config/Milestone.h"
#include "utils/CameraEnumerator.h"

#include <QtConcurrent/QtConcurrent>

MilestoneCameraPage::MilestoneCameraPage(QWidget *parent)
	: QWizardPage(parent)
{
	ui.setupUi(this);

	connect(&m_LoadingWatcher, &QFutureWatcher<void>::finished, this, &MilestoneCameraPage::onLoadingFinished);
	m_LoadingWatcher.setFuture(QtConcurrent::run(this, &MilestoneCameraPage::loadCameras));
}

MilestoneCameraPage::~MilestoneCameraPage()
{

}

std::string MilestoneCameraPage::getCameraGuid() const
{
	QListWidgetItem* item = ui.m_Cameras->currentItem();
	if (item == nullptr)
		return "";

	return item->data(Qt::UserRole).toString().toStdString();
}

std::string MilestoneCameraPage::getCameraName() const
{
	QListWidgetItem* item = ui.m_Cameras->currentItem();
	if (item == nullptr)
		return "";

	return item->text().toStdString();
}

void MilestoneCameraPage::onLoadingFinished()
{
	ui.m_Searching->setVisible(false);
	ui.m_Cameras->setEnabled(true);
	
	auto items = ui.m_Cameras->findItems(m_CameraToSelect.c_str(), Qt::MatchCaseSensitive);
	if (items.count() > 0)
		ui.m_Cameras->setCurrentItem(items[0]);
}

void MilestoneCameraPage::loadCameras()
{
	config::Milestone& config = config::Milestone::getInstance();
	config.load();
	utils::camera::CameraEnumerator camEnumerator(utils::camera::MilestoneCredentials(
		config.getIp(),
		config.getUser(),
		config.getPassword()));

	std::vector<utils::camera::MilestoneCameraParameters> cameras;
	camEnumerator.GetCamerasConfig(cameras);

	for (auto camParams : cameras)
	{
		QListWidgetItem* item = new QListWidgetItem(camParams.GetName().c_str());
		item->setData(Qt::UserRole, QString(camParams.GetGUID().c_str()));
		ui.m_Cameras->addItem(item);	
	}
}
