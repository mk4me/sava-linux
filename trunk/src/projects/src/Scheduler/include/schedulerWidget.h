#ifndef schedulerWidget_h__
#define schedulerWidget_h__

#include "ui_schedulerWidget.h"
#include "QtWidgets/QMenu"

class SchedulerWidget : public QWidget, public Ui::SchedulerWidgetUI
{
    Q_OBJECT

public:
    SchedulerWidget( QWidget* parent = 0 );
	~SchedulerWidget();
		
private slots:
	void addActionEmpty();
	void addActionTemplate();
	void removeAction();
	void editAction(int row, int column);
	void currentActionChanged(int row, int column, int oldRow, int oldColumn);

private:
	QMenu m_AddActionMenu;

	void setupWindow();
	void addEntry(const aq_qtutils::CAction& action);
};

#endif // schedulerWidget_h__
