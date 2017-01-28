#include "CameraDefinitionDialog.h"
#include "MaskDialog.h"

#include "utils/AxisRawReader.h"

CameraDefinitionDialog::CameraDefinitionDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	Qt::WindowFlags flags = windowFlags();
	Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
	flags = flags & (~helpFlag);
	setWindowFlags(flags);

	QString ipRange = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
	QRegExp ipRegex("^" + ipRange + "\\." + ipRange	+ "\\." + ipRange + "\\." + ipRange + "$");
	QRegExpValidator *ipValidator = new QRegExpValidator(ipRegex, this);
	ui.m_Ip->setValidator(ipValidator);
}

CameraDefinitionDialog::~CameraDefinitionDialog()
{

}

void CameraDefinitionDialog::setFields(const QString& name, const QString& user, const QString& password, const QString& ip)
{
	ui.m_Name->setText(name);
	ui.m_User->setText(user);
	ui.m_Password->setText(password);
	ui.m_Ip->setText(ip);
}

void CameraDefinitionDialog::onEditMaskClicked()
{
	MaskDialog maskDialog(std::make_shared<utils::camera::AxisRawReader>(ui.m_Ip->text().toStdString(), (ui.m_User->text() + ":" + ui.m_Password->text()).toStdString()), m_MaskData, this);
	if (maskDialog.exec() == QDialog::Accepted)
	{
		m_MaskData = maskDialog.getMaskData();
		m_Mask = maskDialog.getMask();
	}
}
