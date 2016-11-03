#pragma once

#include <QAbstractTableModel>
#include "MonitorAlertManager.h"

class MonitorAlertsTableModel : public QAbstractTableModel, public IAlertListener
{

	enum EColumnID
	{
		ALERT_NAME,
		START_TIME,
		END_TIME
	};

public:
	MonitorAlertsTableModel(QObject *parent = 0);

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;

protected:
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

	virtual void onAlertStart(const MonitorAlertPtr& i_Alert) override;
	virtual void onAlertEnd(const MonitorAlertPtr& i_Alert) override;
	virtual void onAlertDelete(const MonitorAlertPtr& i_Alert) override;

private:
	MonitorAlertManager* m_Alerts;
};

