#pragma once

#include <QtCore/QObject>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include "EditorGraphicsView.h"
#include "EditorScene.h"

#include "utils/ImageConverter.h"
#include "utils/Filesystem.h"
#include "EditorTimeline.h"

#include <opencv2/videoio.hpp>
#include <opencv2/videoio/videoio_c.h>


/// <summary>
/// Llasa pozwalaj¹ca zapisaæ opisywan¹ sekwencjê jako film .avi
/// </summary>
class EditorVideoSaver : public QObject
{
	Q_OBJECT

public:
	EditorVideoSaver(EditorGraphicsView* view);
	~EditorVideoSaver();

public slots:
	void save();

private slots:
	void save_frame();

private:
	bool init();
	void check_next_frame();

	void onStarted();
	void onFinished();
	void onProgress(int progress);

	void release();
	
private:
	EditorScene* m_GraphicsScene;
	QMessageBox* m_Box;
	QPushButton* m_BoxPushButton;

	QString m_VideoSaveFilePath;
	QSize m_VideoSize;
	cv::VideoWriter m_VideoWriter;

	QImage* m_SceneImage;
	QPainter* m_ScenePainter;

	int m_StartFrame;
	int m_EndFrame;
	int m_SaveFrame;

	QTimer m_Timer;

};

