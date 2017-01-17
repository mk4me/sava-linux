#ifndef TRAINMANAGER_H
#define TRAINMANAGER_H

#include "ui_TrainManager.h"

#include <QtCore/QProcess>

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

	void setTrainingFinished();

signals:
	void error();

private:
	enum TrainState
	{
		DATABASE,
		GMM,
		FV,
		SPLIT,
		MERGE,
		SVM,
	};

	Ui::TrainManagerClass ui;

	TrainState m_State;
	QVector<QProcess*> m_ProcessList;

	void startProcess();
	void clean();

	void printError(QProcess* process);
	void log(const QString& message);

	virtual void showEvent(QShowEvent *) override;
};

#endif // TRAINMANAGER_H
