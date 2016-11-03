#include "MonitorAlertsAcceptDialog.h"
#include "MonitorWindow.h"
#include "QTimer"

#include "Utils\Application.h"
#include "Sequence\Action.h"
#include "Sequence\Cluster.h"
#include <QtWidgets\QMessageBox>

#include <QGlobal.h>
#include <QTime>



MonitorWindow::MonitorWindow(QWidget *parent)
	: QMainWindow(parent)
{
	MonitorConfig::getInstance().load();

	ui.setupUi(this);
	initGui();

	connect(&m_FrameTimer, SIGNAL(frameChanged(size_t)), this, SLOT(onFrameChanged(size_t)));
	connect(&m_FrameTimer, SIGNAL(frameOutOfRange()), this, SLOT(onFrameOutOfRange()));
	connect(&m_VideoManager, SIGNAL(loaded()), this, SLOT(onVideoLoaded()));
	connect(ui.alertsView, SIGNAL(alertClicked(MonitorAlertPtr)), this, SLOT(onAlertClicked(MonitorAlertPtr)));
	connect(m_ReplayWindow, SIGNAL(alertAccepted(MonitorAlertPtr)), this, SLOT(onAlertAccepted(MonitorAlertPtr)));

	m_VideoManager.start();
}

MonitorWindow::~MonitorWindow()
{
	delete m_VideoScene;
}

void MonitorWindow::initGui()
{
	m_VideoScene = new MonitorGraphicsScene(this);
	ui.graphicsView->setScene(m_VideoScene);

	m_AlertDialog = new AlertDialog(this);
	m_ReplayWindow = new ReplayWindow(this);
;
	ui.toolBar->setEnabled(false);
	ui.actionSendToMilestone->setVisible(m_MilestoneAlertSender.isAvailable());
	ui.actionSendToMilestone->setChecked(m_MilestoneAlertSender.isEnabled());
	ui.actionShowClusters->setChecked(m_Config.isClustersVisibled());
}

void MonitorWindow::onFrameChanged(size_t frame) {
	//kolejnosc jest istotna
	m_RegionsManager.update(frame);
	m_ActionManager.update(frame);
	m_VideoScene->update(frame);
}


void MonitorWindow::onFrameOutOfRange(){
	m_ActionManager.onVideoPreload();
	m_AlertManager.onVideoPreload();
	m_VideoScene->onVideoPreload();
	m_VideoManager.load();
}


void MonitorWindow::onVideoLoaded() {

	static bool firstLoaded = false;
	if (!firstLoaded)
	{
		firstLoaded = true;
		onVideoFirstLoaded();
	}

	m_RegionsManager.onVideoLoaded();
	m_ActionManager.onVideoLoaded();
	m_AlertManager.onVideoLoaded();
	m_VideoScene->onVideoLoaded();
	m_FrameTimer.setFramesTimes(m_VideoManager.getVideoTimes()); 
}


void MonitorWindow::onActionShowClusters() {
	m_Config.setClustersVisibled(ui.actionShowClusters->isChecked());
	ui.graphicsView->update();
}


void MonitorWindow::onActionSendToMilestone(){
	m_MilestoneAlertSender.setEnabled(ui.actionSendToMilestone->isChecked());
}


void MonitorWindow::onActionSetAlert(){
	m_AlertDialog->move(pos().x() + 100, pos().y() + 100);
	m_AlertDialog->exec();
	MonitorConfig::getInstance().save();
}


void MonitorWindow::onActionEditAlertRegions()
{
	bool editEnabled = ui.actionEditAlertRegions->isChecked();

	m_VideoScene->setState(editEnabled ? MonitorGraphicsScene::ALERT_REGION_EDITABLE : MonitorGraphicsScene::NONE);
	m_RegionsManager.setCollisionEnabled(!editEnabled);
	m_AlertManager.setEnabled(!editEnabled);
	ui.alertsView->setEnabled(!editEnabled);

	if (editEnabled)
		m_AlertManager.removeAllUnfinishedAlerts();

	if (editEnabled)
	{
		disableActions(ui.actionEditAlertRegions);
	}
	else
	{
		ui.actionEditAlertRegions->setChecked(false);
		enableActions();
		MonitorConfig::getInstance().save();
	}
}


void MonitorWindow::onActionEditMaskRegions()
{
	bool editEnabled = ui.actionEditMaskRegions->isChecked();

	m_VideoScene->setState(editEnabled ? MonitorGraphicsScene::MASK_REGION_EDITABLE : MonitorGraphicsScene::NONE);
	m_RegionsManager.setCollisionEnabled(!editEnabled);
	m_AlertManager.setEnabled(!editEnabled);
	ui.alertsView->setEnabled(!editEnabled);

	if (editEnabled)
		m_AlertManager.removeAllUnfinishedAlerts();

	if (editEnabled)
	{
		disableActions(ui.actionEditMaskRegions);
	}
	else
	{
		ui.actionEditMaskRegions->setChecked(false);
		enableActions();
		MonitorConfig::getInstance().save();
	}
}


void MonitorWindow::onActionSynchronize()
{
	if (m_VideoManager.isSynchronizeNecessary())
	{
		m_FrameTimer.tickDisabled();
		m_ActionManager.onVideoPreload();
		m_AlertManager.onVideoPreload();
		m_VideoScene->onVideoPreload();
		m_VideoScene->setVideoVisible(false);

		m_VideoManager.synchronize();
	}
}

void MonitorWindow::onActionAcceptAlerts()
{
	MonitorAlertsAcceptDialog dialog(this);
	dialog.setWindowFlags(Qt::WindowSystemMenuHint | Qt::WindowTitleHint);

	if (dialog.exec() == QDialog::Accepted)
	{
		m_AlertManager.acceptAllFinishedAlerts();
		ui.alertsView->update();
	}
}

void MonitorWindow::onAlertClicked(const MonitorAlertPtr& alert)
{
	if (alert->isValid())
		m_ReplayWindow->show(alert);
}

void MonitorWindow::onAlertAccepted(const MonitorAlertPtr& alert)
{
	m_AlertManager.saveAlert(alert);
	ui.alertsView->update();
}

void MonitorWindow::onVideoFirstLoaded()
{
	ui.toolBar->setEnabled(true);
	m_VideoScene->init();
}


void MonitorWindow::disableActions(QAction* exludeAction)
{
	QList<QAction*> actionList = ui.toolBar->actions();
	for (QAction* action : actionList)
	{
		if (action != exludeAction)
		{
			action->setEnabled(false);
			action->setVisible(false);
		}
	}
}

void MonitorWindow::enableActions()
{
	QList<QAction*> actionList = ui.toolBar->actions();
	for (QAction* action : actionList)
	{
		if (action == ui.actionSendToMilestone && !m_MilestoneAlertSender.isAvailable())
			continue;

		action->setEnabled(true);
		action->setVisible(true);
	}
}


void MonitorWindow::resizeEvent(QResizeEvent *e)
{
	int w_view1 = width() * 0.80;
	int w_view2 = width() * 0.20;
	ui.splitter->setSizes(QList<int>() << w_view1 << w_view2);

	ui.alertsView->updateColumnWidth();

	QMainWindow::resizeEvent(e);
}


void MonitorWindow::keyPressEvent(QKeyEvent *e)
{
	switch (e->key())
	{

	case Qt::Key_Escape:
		MonitorConfig::getInstance().save();
		utils::Application::getInstance()->exit();
		break;

	case Qt::Key_S:
		MonitorConfig::getInstance().save();
		break;
	}
}


void MonitorWindow::closeEvent(QCloseEvent *e)
{
	MonitorConfig::getInstance().save();
	utils::Application::getInstance()->exit();
	QMainWindow::closeEvent(e);
}




