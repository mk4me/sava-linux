#ifndef MASKCONFIG_H
#define MASKCONFIG_H

#include "ui_MaskDialog.h"
#include "MaskRegions.h"

#include "utils/ICameraRawReader.h"

class MaskDialog : public QDialog
{
	Q_OBJECT

public:

	/*
		@param camera - camera to take background image from.
		@param maskData - string serialized mask data (mask polygons).
	*/
	MaskDialog(
		const std::shared_ptr<utils::camera::ICameraRawReader>& camera,
		const std::string& maskData,
		QWidget *parent = 0);

	~MaskDialog();

	std::string getMaskData() const;
	cv::Mat getMask() const;

private:
	void initScene();

	void log(const std::string& msg);

private slots:
	void updateGui();

	void onEditClicked();
	void onAddClicked();
	void onRemoveClicked();
	void onClearClicked();

	void setBackground(const QPixmap& background);

signals:
	void backgroundChanged(const QPixmap& background);

private:
	static const int c_CameraWidth;
	static const int c_CameraHeight;

	Ui::MaskDialog ui;

	MaskScene m_Scene;
	QGraphicsPixmapItem m_MaskBackground;
	MaskRegions m_MaskRegions;

	std::string m_MaskData;
	QTimer* m_LogTimer;

	std::shared_ptr<utils::camera::ICameraRawReader> m_Camera;

	void getBackgroundThread();
};

#endif // MASKCONFIG_H
