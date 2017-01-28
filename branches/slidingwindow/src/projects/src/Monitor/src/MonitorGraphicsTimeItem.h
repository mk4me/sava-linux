#pragma once

#include "MonitorGraphicsTextItem.h"

/// <summary>
/// Obiekt graficzny, wywietlaj¹cy czas wideo z kamery na scenie.
/// </summary>
/// <seealso cref="MonitorGraphicsTextItem" />
class MonitorGraphicsTimeItem : public MonitorGraphicsTextItem
{

public:
	MonitorGraphicsTimeItem(QGraphicsItem* parent = 0);

	virtual void update(size_t _frame) override;

protected:
	virtual boost::posix_time::ptime getTime(size_t _frame) const;

private:
	QString timeToString(const boost::posix_time::ptime& _time);

};


