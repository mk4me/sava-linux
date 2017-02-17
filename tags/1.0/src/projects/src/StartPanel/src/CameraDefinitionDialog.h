#ifndef CAMERADEFINITIONDIALOG_H
#define CAMERADEFINITIONDIALOG_H

#include "ui_CameraDefinitionDialog.h"

#include <QtWidgets/QDialog>

#include <opencv/cv.h>

class CameraDefinitionDialog : public QDialog
{
	Q_OBJECT

public:
	CameraDefinitionDialog(QWidget *parent = 0);
	~CameraDefinitionDialog();

	void setFields(const QString& name, const QString& user, const QString& password, const QString& ip);

	QString getName() const { return ui.m_Name->text(); }
	QString getUser() const { return ui.m_User->text(); }
	QString getPassword() const { return ui.m_Password->text(); }
	QString getIp() const { return ui.m_Ip->text(); }

	const std::string& getMaskData() const { return m_MaskData; }
	void setMaskData(const std::string& maskData) { m_MaskData = maskData; }

	const cv::Mat& getMask() const { return m_Mask; }

private slots:
	void onEditMaskClicked();

private:
	Ui::CameraDefinitionDialog ui;
	std::string m_MaskData;
	cv::Mat m_Mask;
};

#endif // CAMERADEFINITIONDIALOG_H
