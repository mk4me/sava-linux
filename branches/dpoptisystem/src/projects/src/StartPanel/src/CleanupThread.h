#ifndef CLEANUPTHREAD_H
#define CLEANUPTHREAD_H

#include <QtCore/QThread>

class CleanupThread : public QThread
{
	Q_OBJECT

public:
	CleanupThread(QObject *parent);
	~CleanupThread();

	virtual void run() override;

private:
	
};

#endif // CLEANUPTHREAD_H
