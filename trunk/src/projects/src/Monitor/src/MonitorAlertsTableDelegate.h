#ifndef MONITORALERTSTABLEDELEGATE_H
#define MONITORALERTSTABLEDELEGATE_H

#include <QtWidgets/QStyledItemDelegate>
#include <QtCore/QEvent>
#include <QtCore/QDebug>
#include "MonitorAlertManager.h"

class MonitorAlertsTableView;

/// <summary>
/// Implementuje sposób graficznego wywietlania wpisu w tabeli alertów.
/// </summary>
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
