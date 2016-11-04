#include "NetworkDefinitionDialog.h"

NetworkDefinitionDialog::NetworkDefinitionDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	Qt::WindowFlags flags = windowFlags();
	Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
	flags = flags & (~helpFlag);
	setWindowFlags(flags);

	QString ipRange = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
	QRegExp ipRegex("^" + ipRange + "\\." + ipRange + "\\." + ipRange + "\\." + ipRange + "$");
	QRegExpValidator *ipValidator = new QRegExpValidator(ipRegex, this);
	ui.m_Ip->setValidator(ipValidator);
}

NetworkDefinitionDialog::~NetworkDefinitionDialog()
{

}

void NetworkDefinitionDialog::setFields(const QString& name, const QString& ip)
{
	ui.m_Name->setText(name);
	ui.m_Ip->setText(ip);
}
