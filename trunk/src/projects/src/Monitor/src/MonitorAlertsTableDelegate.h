#ifndef MONITORALERTSTABLEDELEGATE_H
#define MONITORALERTSTABLEDELEGATE_H

#include <QStyledItemDelegate>
#include <QEvent>
#include <QDebug>
#include "MonitorAlertManager.h"

class MonitorAlertsTableView;

class MonitorAlertsTableDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	MonitorAlertsTableDelegate(QObject *parent = 0);
	~MonitorAlertsTableDelegate();

protected:
	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;	

private:
	MonitorAlertsTableView* m_View;
	MonitorAlertManager* m_Alerts;
};

#endif // MONITORALERTSTABLEDELEGATE_H
