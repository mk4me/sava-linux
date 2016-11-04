#include "MilestoneConfig.h"

#include "config/Milestone.h"

MilestoneConfig::MilestoneConfig(QWidget *parent)
{
	ui.setupUi(this);

	QString ipRange = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
	QRegExp ipRegex("^" + ipRange + "\\." + ipRange + "\\." + ipRange + "\\." + ipRange + "$");
	QRegExpValidator *ipValidator = new QRegExpValidator(ipRegex, this);
	ui.m_Ip->setValidator(ipValidator);
}

MilestoneConfig::~MilestoneConfig()
{

}

bool MilestoneConfig::load()
{
	config::Milestone& config = config::Milestone::getInstance();
	config.load();

	ui.m_Ip->blockSignals(true);
	ui.m_Ip->setText(config.getIp().c_str());
	ui.m_Ip->blockSignals(false);

	ui.m_User->blockSignals(true);
	ui.m_User->setText(config.getUser().c_str());
	ui.m_User->blockSignals(false);

	ui.m_Password->blockSignals(true);
	ui.m_Password->setText(config.getPassword().c_str());
	ui.m_Password->blockSignals(false);

	return true;
}

bool MilestoneConfig::save()
{
	config::Milestone& config = config::Milestone::getInstance();
	config.setIp(ui.m_Ip->text().toStdString());
	config.setUser(ui.m_User->text().toStdString());
	config.setPassword(ui.m_Password->text().toStdString());

	return config.save();
}
