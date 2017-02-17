#ifndef CAMERACONFIG_H
#define CAMERACONFIG_H

#include "IConfigItem.h"
#include "ui_CameraConfig.h"

class CameraConfig : public IConfigItem
{
	Q_OBJECT

public:
	CameraConfig(QWidget *parent = 0);
	~CameraConfig();

	virtual bool load();
	virtual bool save();

private slots:
	void addCamera();
	void editCamera();
	void editCamera(int row, int column);
	void removeCamera();
	void onSelectionChanged(int currentRow, int currentCol, int prevRow, int prewCol);

private:
	Ui::CameraConfig ui;

	void setCamera(int row, const QString& name, const QString& user, const QString& password, const QString& ip);
	void setItem(int row, int column, const QString& text);
};

#endif // CAMERACONFIG_H
