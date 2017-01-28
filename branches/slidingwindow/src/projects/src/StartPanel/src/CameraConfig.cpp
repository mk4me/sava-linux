#include "CameraConfig.h"
#include "CameraDefinitionDialog.h"

#include "config/Camera.h"
#include "opencv2/highgui.hpp"

CameraConfig::CameraConfig(QWidget *parent)
{
	ui.setupUi(this);
}

CameraConfig::~CameraConfig()
{

}

bool CameraConfig::load()
{
	config::Camera& config = config::Camera::getInstance();
	config.load();

	for (int i = 0; i < config.getNumCameras(); ++i)
		setCamera(i, config.getName(i).c_str(), config.getUser(i).c_str(), config.getPassword(i).c_str(), config.getIp(i).c_str());

	return true;
}

bool CameraConfig::save()
{
	return config::Camera::getInstance().save();
}

void CameraConfig::addCamera()
{
	CameraDefinitionDialog dialog;
	if (dialog.exec() == QDialog::Rejected)
		return;

	int index = config::Camera::getInstance().addCamera(
		dialog.getUser().toStdString(),
		dialog.getPassword().toStdString(),
		dialog.getIp().toStdString(),
		dialog.getName().toStdString());

	if (index >= 0)
		setCamera(index, dialog.getName(), dialog.getUser(), dialog.getPassword(), dialog.getIp());
}

void CameraConfig::editCamera()
{
	config::Camera& config = config::Camera::getInstance();
	int index = ui.m_CameraList->currentRow();
	if (index < 0 || index >= config.getNumCameras())
		return;

	CameraDefinitionDialog dialog;
	dialog.setFields(
		config.getName(index).c_str(),
		config.getUser(index).c_str(),
		config.getPassword(index).c_str(),
		config.getIp(index).c_str());
	dialog.setMaskData(config.getMaskData(index));
	if (dialog.exec() == QDialog::Rejected)
		return;

	if (!config.setName(index, dialog.getName().toStdString())) return;
	config.setUser(index, dialog.getUser().toStdString());
	config.setPassword(index, dialog.getPassword().toStdString());
	config.setIp(index, dialog.getIp().toStdString());
	config.setMaskData(index, dialog.getMaskData());
	config.setMask(index, dialog.getMask());

	setCamera(index,
		config.getName(index).c_str(),
		config.getUser(index).c_str(),
		config.getPassword(index).c_str(),
		config.getIp(index).c_str());
}

void CameraConfig::editCamera(int row, int column)
{
	editCamera();
}

void CameraConfig::removeCamera()
{
	int index = ui.m_CameraList->currentRow();

	if (!config::Camera::getInstance().removeCamera(index))
		return;

	ui.m_CameraList->removeRow(index);
}

void CameraConfig::onSelectionChanged(int currentRow, int currentCol, int prevRow, int prewCol)
{
	ui.m_EditButton->setEnabled(currentRow >= 0);
	ui.m_RemoveButton->setEnabled(currentRow >= 0);
}

void CameraConfig::setCamera(int row, const QString& name, const QString& user, const QString& password, const QString& ip)
{
	ui.m_CameraList->blockSignals(true);

	if (ui.m_CameraList->rowCount() <= row)
		ui.m_CameraList->setRowCount(row + 1);

	setItem(row, 0, name);
	setItem(row, 1, user);
	setItem(row, 2, password);
	setItem(row, 3, ip);

	ui.m_CameraList->blockSignals(false);
}

void CameraConfig::setItem(int row, int column, const QString& text)
{
	QTableWidgetItem* item = ui.m_CameraList->item(row, column);
	if (item == nullptr)
	{
		item = new QTableWidgetItem();
		ui.m_CameraList->setItem(row, column, item);
	}
	item->setText(text);
}
