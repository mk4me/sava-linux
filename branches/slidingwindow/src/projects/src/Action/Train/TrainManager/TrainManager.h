#ifndef TRAINMANAGER_H
#define TRAINMANAGER_H

#include "ui_TrainManager.h"
#include "config\Action.h"

#include <QProcess>

class TrainManager : public QMainWindow
{
	Q_OBJECT

public:
	TrainManager(QWidget *parent = 0);
	~TrainManager();

private slots:
	void processError(QProcess::ProcessError err);
	void processFinished(int exitCode, QProcess::ExitStatus exitStatus);

	void cancel(QAbstractButton* button);
	void stopAll();

signals:
	void error();

private:
	enum TrainState
	{
		GMM,
		FV,
		SPLIT,
		MERGE,
		SVM,
	};

	Ui::TrainManagerClass ui;

	config::Action m_ActionConfig;
	TrainState m_State;
	QVector<QProcess*> m_ProcessList;

	void startProcess();

	virtual void showEvent(QShowEvent *) override;
};

#endif // TRAINMANAGER_H
