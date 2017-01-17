#pragma once

#include <QtWidgets/QTableView>
#include <QtWidgets/QHeaderView>
#include <QtGui/QMouseEvent>

#include "MonitorAlertsTableModel.h"
#include "MonitorAlertsTableDelegate.h"


/// <summary>
/// Implementuje widok tabeli alertów w oknie Monitora.
/// </summary>
class MonitorAlertsTableView : public QTableView
{
	Q_OBJECT

public:
	MonitorAlertsTableView(QWidget* parent = 0);

	QModelIndex getSelectedIndex() const;
	int getHoverRow() const { return m_HoverRow; }

	void updateColumnWidth();


	struct SPalette
	{
		//alert palette
		QColor item_alert_background_color;
		QPen item_alert_pen;
		QFont item_alert_font;

		//unaccepted palette
		QColor item_unaccepted_background_color;
		QPen item_unaccepted_pen;
		QFont item_unaccepted_font;

		//accepted palette
		QColor item_accepted_background_color;
		QPen item_accepted_pen;
		QFont item_accepted_font;

		//select / hover item
		QColor item_hover_color;
		QColor item_selected_color;

	};
	static SPalette PALETTE;

signals:
	void alertClicked(const MonitorAlertPtr& alert);

private slots:
	void onDoubleClicked(const QModelIndex& index);

protected:
	virtual bool event(QEvent* event) override;

private:
	static SPalette initPallette();

private:
	MonitorAlertsTableModel* m_Model;
	MonitorAlertsTableDelegate* m_Delegate;

	int m_HoverRow;
	bool m_IsHover;

};

