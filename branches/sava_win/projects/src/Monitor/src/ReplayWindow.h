#ifndef REPLAYWINDOW_H
#define REPLAYWINDOW_H

#include <QtWidgets/QMainWindow>
#include "ui_ReplayWindow.h"
#include "ReplayTimer.h"
#include "ReplayGraphicsScene.h"
#include "MonitorAlert.h"
#include "ReplayVideo.h"

/// <summary>
/// Klasa g³ównego okna podgl¹du alertów.
/// </summary>
class ReplayWindow : public QMainWindow
{
	Q_OBJECT

public:
	ReplayWindow(QWidget *parent = 0);
	~ReplayWindow();

	void show(const MonitorAlertPtr& alert);

	ReplayVideoPtr getVideo() const { return m_CurrentVideo; }
	MonitorAlertPtr getAlert() const { return m_CurrentAlert; }

signals:
	void alertAccepted(const MonitorAlertPtr& alert);

public slots:
	void onPlay();
	void onAccept();
	void onSliderValueChanged(int);
	
	void onVideoStarted();
	void onVideoStopped();
	void onVideoFrameChanged(size_t frame);
	void onVideoFinished();

protected:
	virtual void hideEvent(QHideEvent *) override;

private:
	Ui::ReplayWindow ui;

	ReplayTimer* m_Timer;
	ReplayGraphicsScene* m_Scene;

	ReplayVideoPtr m_CurrentVideo;
	MonitorAlertPtr m_CurrentAlert;

	QIcon m_PlayIcon;
	QIcon m_StopIcon;
	QIcon m_ReplayIcon;
};

#endif // REPLAYWINDOW_H
