#ifndef CLEANUPDIALOG_H
#define CLEANUPDIALOG_H

#include <QtWidgets/QDialog>
#include "ui_CleanupDialog.h"

class CleanupDialog : public QDialog
{
	Q_OBJECT

public:
	CleanupDialog(QWidget *parent = 0);
	~CleanupDialog();

private:
	Ui::CleanupDialog ui;

protected:

	virtual void showEvent(QShowEvent *) override;

};

#endif // CLEANUPDIALOG_H
