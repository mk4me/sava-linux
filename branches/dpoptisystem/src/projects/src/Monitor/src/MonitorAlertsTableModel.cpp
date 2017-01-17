#include "MonitorAlertsTableModel.h"
#include "MonitorAlertsTableView.h"


MonitorAlertsTableModel::MonitorAlertsTableModel(QObject *parent)
	:QAbstractTableModel(parent)
{
	m_Alerts = MonitorAlertManager::getPointer();
	
	MonitorAlertManager::getInstance().addListener(this);
}


int MonitorAlertsTableModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
	return (int)m_Alerts->getAlertsCounts();
}

int MonitorAlertsTableModel::columnCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
	return 3;
}

QVariant MonitorAlertsTableModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const
{
	if (!index.isValid())
		return QVariant();

	if (role == Qt::TextAlignmentRole) 
	{
		return int(Qt::AlignCenter);
	}
	else if (role == Qt::DisplayRole)
	{
		MonitorAlertPtr alert = m_Alerts->getAlertById(index.row());
		switch (index.column())
		{
			case ALERT_NAME:	return alert->getName().c_str();
			case START_TIME:	return alert->isStarted() ? MonitorAlert::timeToString(alert->getStartTime()).c_str() : "";
			case END_TIME:		return alert->isFinished() ? MonitorAlert::timeToString(alert->getEndTime()).c_str() : "";
		}
	}

	return QVariant();
}

QVariant MonitorAlertsTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
	{
		switch (section)
		{
			case ALERT_NAME:	return "Alert";
			case START_TIME:	return "Start Time";
			case END_TIME:		return "End Time";
		}
	}

	return QVariant();
}

void MonitorAlertsTableModel::onAlertStart(const MonitorAlertPtr& i_Alert)
{
	beginInsertRows(QModelIndex(), 0, 0);
	endInsertRows();
}

void MonitorAlertsTableModel::onAlertEnd(const MonitorAlertPtr& i_Alert)
{
	int row = MonitorAlertManager::getInstance().getAlertId(i_Alert);
	if (row >= 0)
		emit dataChanged(index(row, ALERT_NAME), index(row, END_TIME));
}

void MonitorAlertsTableModel::onAlertDelete(const MonitorAlertPtr& i_Alert)
{
	beginResetModel();
	endResetModel();
}

