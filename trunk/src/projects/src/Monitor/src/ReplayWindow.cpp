#include "ReplayWindow.h"
#include "utils/ImageConverter.h"
#include <QtCore/QDebug>

ReplayWindow::ReplayWindow(QWidget *parent)
	: QMainWindow(parent)
	, m_CurrentAlert(nullptr)
	, m_CurrentVideo(new ReplayVideo())
{
	ui.setupUi(this);

	m_Scene = new ReplayGraphicsScene(this);
	m_Timer = new ReplayTimer(this);

	ui.graphicsView->setScene(m_Scene);

	connect(m_Timer, SIGNAL(started()), this, SLOT(onVideoStarted()));
	connect(m_Timer, SIGNAL(stopped()), this, SLOT(onVideoStopped()));
	connect(m_Timer, SIGNAL(frameChanged(size_t)), this, SLOT(onVideoFrameChanged(size_t)));
	connect(m_Timer, SIGNAL(frameOutOfRange()), this, SLOT(onVideoFinished()));

	connect(ui.videoSlider, SIGNAL(valueChanged(int)), this, SLOT(onSliderValueChanged(int)));

	m_PlayIcon = QIcon(":/icons_64/play.png");
	m_StopIcon = QIcon(":/icons_64/stop.png");
	m_ReplayIcon = QIcon(":/icons_64/replay.png");

	m_Scene->init();
}

ReplayWindow::~ReplayWindow()
{

}

void ReplayWindow::show(const MonitorAlertPtr& alert)
{
	//first stop the clock timer
	m_Timer->stop();

	//set current alert
	m_CurrentAlert = alert;

	//reload alert videos
	m_CurrentVideo->reload(m_CurrentAlert->getVideos());

	//init timer with frames times
	m_Timer->init(m_CurrentVideo->getFramesTimes(), alert->getStartTime(), alert->getEndTime());

	//reload scene
	m_Scene->reload();

	//init slider
	ui.videoSlider->setRange((int)m_Timer->getFirstFrame(), (int)m_Timer->getLastFrame());
	ui.videoSlider->setValue((int)m_Timer->getFirstFrame());

	//hide accept button
	ui.acceptButton->setVisible(false);

	//set action name
	ui.alertName->setText(QString::fromStdString(alert->getName()).toUpper());

	//unzoom graphics view
	ui.graphicsView->fitScene();

	//start the frame timer
	m_Timer->start();

	//show window
	QMainWindow::show();
}

void ReplayWindow::onPlay(){

	if (m_Timer->isPlaying())
		m_Timer->stop();
	else if (m_Timer->isFinished())
		m_Timer->replay();
	else 
		m_Timer->start();
}


void ReplayWindow::onAccept()
{
	m_CurrentAlert->accept();
	ui.acceptButton->setVisible(false);

	emit alertAccepted(m_CurrentAlert);
}

void ReplayWindow::onSliderValueChanged(int value)
{
	m_Timer->stop();

	if (value == m_Timer->getLastFrame())
		onVideoFinished();
	else
		ui.playButton->setIcon(m_PlayIcon);

	m_Timer->setCurrentFrame(value);
}

void ReplayWindow::onVideoStarted()
{
	ui.playButton->setIcon(m_StopIcon);
}

void ReplayWindow::onVideoStopped()
{
	ui.playButton->setIcon(m_PlayIcon);
}

void ReplayWindow::onVideoFrameChanged(size_t frame)
{
	disconnect(ui.videoSlider, SIGNAL(valueChanged(int)), this, SLOT(onSliderValueChanged(int)));

	m_Scene->update(frame);
	ui.videoSlider->setValue((int)frame);

	connect(ui.videoSlider, SIGNAL(valueChanged(int)), this, SLOT(onSliderValueChanged(int)));
}

void ReplayWindow::onVideoFinished()
{
	if (!m_CurrentAlert->isAccepted())
		ui.acceptButton->setVisible(true);

	ui.playButton->setIcon(m_ReplayIcon);
}


void ReplayWindow::hideEvent(QHideEvent *e)
{
	m_Timer->stop();
	QMainWindow::hideEvent(e);
}
