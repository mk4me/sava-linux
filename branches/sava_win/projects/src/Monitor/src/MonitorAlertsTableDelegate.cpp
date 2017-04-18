#include "MonitorAlertsTableDelegate.h"
#include "MonitorAlertsTableView.h"

MonitorAlertsTableDelegate::MonitorAlertsTableDelegate(QObject *parent)
	: QStyledItemDelegate(parent)
{
	m_View = dynamic_cast<MonitorAlertsTableView*>(parent);
	m_Alerts = MonitorAlertManager::getPointer();
}

MonitorAlertsTableDelegate::~MonitorAlertsTableDelegate()
{

}

void MonitorAlertsTableDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	MonitorAlertPtr alert = m_Alerts->getAlertById(index.row());
	QModelIndex selectedIndex = m_View->getSelectedIndex();

	bool isSelected = selectedIndex.isValid() ? index.row() == selectedIndex.row() : false;
	bool isHover = index.row() == m_View->getHoverRow();


	QColor backgroundColor;
	QPen textPen;
	QFont textFont;
	switch (alert->getState())
	{
		case MonitorAlert::STARTED:
			backgroundColor = MonitorAlertsTableView::PALETTE.item_alert_background_color;
			textPen = MonitorAlertsTableView::PALETTE.item_alert_pen;
			textFont = MonitorAlertsTableView::PALETTE.item_alert_font;
			break;

		case MonitorAlert::FINISHED_UNACCEPTED:
			backgroundColor = MonitorAlertsTableView::PALETTE.item_unaccepted_background_color;
			textPen = MonitorAlertsTableView::PALETTE.item_unaccepted_pen;
			textFont = MonitorAlertsTableView::PALETTE.item_unaccepted_font;
			break;


		case MonitorAlert::FINISHED_ACCEPTED:
			backgroundColor = MonitorAlertsTableView::PALETTE.item_accepted_background_color;
			textPen = MonitorAlertsTableView::PALETTE.item_accepted_pen;
			textFont = MonitorAlertsTableView::PALETTE.item_accepted_font;
			break;
	}

	//draw background 
	painter->fillRect(option.rect, backgroundColor);

	//set text pen
	painter->setPen(textPen);

	//set text font
	painter->setFont(textFont);

	//draw text
	painter->drawText(option.rect,
		index.data(Qt::TextAlignmentRole).toInt(), 
		index.data(Qt::DisplayRole).toString());

	//paint select/hover rectangle
	if (isSelected)
	{
		painter->fillRect(option.rect, MonitorAlertsTableView::PALETTE.item_selected_color);
	}
	else if (isHover)
	{
		painter->fillRect(option.rect, MonitorAlertsTableView::PALETTE.item_hover_color);
	}
}