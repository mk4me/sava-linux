#ifndef MONITORWINDOW_H
#define MONITORWINDOW_H

#include <QtWidgets/QMainWindow>
#include "ui_MonitorWindow.h"

#include <QtGui/QKeyEvent>
#include <QtWidgets/QFileDialog>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtWidgets/QColorDialog>

#include "MonitorVideoManager.h"
#include "MonitorFrames.h"
#include "MonitorGraphicsScene.h"
#include "MonitorActionManager.h"
#include "AlertDialog.h"
#include "MilestoneAlertSender.h"
#include "MonitorRegionsManager.h"
#include "MonitorAlertManager.h"

#include "utils/Filesystem.h"
#include "ReplayWindow.h"


class MonitorWindow : public QMainWindow
{
    Q_OBJECT

public:
    MonitorWindow(QWidget *parent = 0);
    ~MonitorWindow();

private slots:
    void onFrameChanged(size_t frame);
    void onFrameOutOfRange();
    void onVideoLoaded();

    void onActionShowClusters();
    void onActionSendToMilestone();
    void onActionSetAlert();
    void onActionEditAlertRegions();
    void onActionEditMaskRegions();
    void onActionSynchronize();
    void onActionAcceptAlerts();

    void onAlertClicked(const MonitorAlertPtr& alert);
    void onAlertAccepted(const MonitorAlertPtr& alert);

protected:
    virtual void keyPressEvent(QKeyEvent *) override;
    virtual void resizeEvent(QResizeEvent *) override;
    virtual void closeEvent(QCloseEvent *) override;

private:
    void initGui();
    void onVideoFirstLoaded();

    void disableActions(QAction* exludeAction = nullptr);
    void enableActions();

private:
    Ui::MonitorWindow ui;

    //kolejnosc jest ważna (singletony)
    MonitorConfig m_Config;
    MonitorVideoManager m_VideoManager;
    MonitorActionManager m_ActionManager;
    MonitorFrames m_FrameTimer;
    MonitorRegionsManager m_RegionsManager;
    MonitorAlertManager m_AlertManager;
    MilestoneAlertSender m_MilestoneAlertSender;

    //gui
    MonitorGraphicsScene* m_VideoScene;
    AlertDialog* m_AlertDialog;
    ReplayWindow* m_ReplayWindow;

};

#endif // MONITORWINDOW_H
