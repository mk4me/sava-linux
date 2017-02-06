#include "MonitorAlertsTableView.h"


MonitorAlertsTableView::SPalette MonitorAlertsTableView::PALETTE = MonitorAlertsTableView::initPallette();

MonitorAlertsTableView::MonitorAlertsTableView(QWidget* parent)
	:QTableView(parent)
	, m_HoverRow(-1)
	, m_IsHover(false)
{
	m_Model = new MonitorAlertsTableModel(this);
	m_Delegate = new MonitorAlertsTableDelegate(this);

	setModel(m_Model);
	setItemDelegate(m_Delegate);

	setMouseTracking(true);	

	connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onDoubleClicked(QModelIndex)));
}
 
void MonitorAlertsTableView::updateColumnWidth()
{
	int w = horizontalHeader()->width();

	setColumnWidth(0, 0.4 * w);
	setColumnWidth(1, 0.3 * w);
	setColumnWidth(2, 0.3 * w);
}


void MonitorAlertsTableView::onDoubleClicked(const QModelIndex& index)
{
	if (index.isValid())
	{
		MonitorAlertPtr alert = MonitorAlertManager::getInstance().getAlertById(index.row());
		if (alert)
			emit alertClicked(alert);
	}
}


QModelIndex MonitorAlertsTableView::getSelectedIndex() const
{
	auto indexes = selectedIndexes();
	return indexes.empty() ? QModelIndex() : indexes.front();
}

bool MonitorAlertsTableView::event(QEvent* e)
{

	switch (e->type())
	{
		case QEvent::HoverLeave:
			m_IsHover = false;
			m_HoverRow = -1;
			break;

		case QEvent::HoverEnter:
			m_IsHover = true;

		case QEvent::Paint:
		case QEvent::Wheel:
		case QEvent::HoverMove:
			if (m_IsHover)
				m_HoverRow = rowAt(viewport()->mapFromGlobal(QCursor::pos()).y());
			break;
	}

	return QTableView::event(e);
}

MonitorAlertsTableView::SPalette MonitorAlertsTableView::initPallette()
{
	SPalette palette;

	//alert palette
	palette.item_alert_background_color = QColor(85, 2, 2);
	palette.item_alert_pen = QPen(Qt::white);
	palette.item_alert_font = QFont("Segoe UI", 8, QFont::Bold);

	//unaccepted palette
	palette.item_unaccepted_background_color = QModelIndex().data(Qt::BackgroundRole).value<QColor>();
	palette.item_unaccepted_pen = QPen(Qt::white);
	palette.item_unaccepted_font = QFont("Segoe UI", 8, QFont::Bold);

	//accepted palette
	palette.item_accepted_background_color = QModelIndex().data(Qt::BackgroundRole).value<QColor>();
	palette.item_accepted_pen = QPen(Qt::gray);
	palette.item_accepted_font = QFont("Segoe UI", 8, QFont::Normal, true);

	//select / hover palette
	palette.item_hover_color = QColor(255, 255, 255, 50);
	palette.item_selected_color = QColor(255, 255, 255, 100);

	return palette;
}