#include "EditorVideoSaver.h"
#include "EditorConfig.h"


EditorVideoSaver::EditorVideoSaver(EditorGraphicsView* view)
	: QObject(view)
	, m_SceneImage(nullptr)
	, m_ScenePainter(nullptr)
	, m_StartFrame(-1)
	, m_EndFrame(-1)
	, m_SaveFrame(-1)
{
	m_GraphicsScene = dynamic_cast<EditorScene*>(view->scene());

	m_Box = new QMessageBox(view);
	m_Box->setWindowFlags(m_Box->windowFlags() | Qt::FramelessWindowHint);
	m_Box->setStandardButtons(0);
	m_BoxPushButton = m_Box->addButton("Ok", QMessageBox::AcceptRole);
}

EditorVideoSaver::~EditorVideoSaver()
{
	release();
}

void EditorVideoSaver::save()
{
	if (init())
	{
		onStarted();
		save_frame();
	}		
}

bool EditorVideoSaver::init()
{
	assert(m_GraphicsScene);

	release();

	//select file location for vidoe 
	QString videoFilePath = EditorConfig::getInstance().getLastVideoAVIPath().c_str();
	if (videoFilePath.isEmpty())
		videoFilePath = QString("%1%2").arg(utils::Filesystem::getAppPath().c_str()).arg("untitled.avi");

	m_VideoSaveFilePath = QFileDialog::getSaveFileName(nullptr, "Save Video File", videoFilePath, "Video (*.avi)");
	if (m_VideoSaveFilePath.isEmpty())
		return false;

	EditorConfig::getInstance().setLastVideoAVIPath(m_VideoSaveFilePath.toStdString());

	//take video size from graphics scene
	m_VideoSize = m_GraphicsScene->sceneRect().size().toSize();

	//open video writer 
	m_VideoWriter.open(m_VideoSaveFilePath.toStdString(), CV_FOURCC('P', 'I', 'M', '1'), 25, cv::Size(m_VideoSize.width(), m_VideoSize.height()), true);
	if (!m_VideoWriter.isOpened())
		return false;

	m_SceneImage = new QImage(m_VideoSize, QImage::Format_RGB888);
	m_ScenePainter = new QPainter(m_SceneImage);
	m_ScenePainter->setRenderHint(QPainter::Antialiasing);

	m_StartFrame = 0;
	m_EndFrame = EditorTimeline::getInstance().getFrameCount() - 1;
	m_SaveFrame = EditorTimeline::getInstance().getCurrentFrame();

	return true;
}

void EditorVideoSaver::check_next_frame()
{
	m_StartFrame++;
	if (m_StartFrame < m_EndFrame)
	{
		onProgress(m_StartFrame * 100 / m_EndFrame);
		m_Timer.singleShot(1, this, SLOT(save_frame()));
	}
	else
	{
		onFinished();
	}
}

void EditorVideoSaver::save_frame()
{
	assert(m_GraphicsScene);

	EditorTimeline::getInstance().setCurrentFrame(m_StartFrame);
	//m_GraphicsScene->redraw(m_StartFrame);
	m_GraphicsScene->render(m_ScenePainter);
	m_VideoWriter << utils::image_converter::QImageToCvMat(*m_SceneImage);

	check_next_frame();
}

void EditorVideoSaver::release()
{
	//release this
	if (m_ScenePainter) {
		delete m_ScenePainter;
		m_ScenePainter = nullptr;
	}

	if (m_SceneImage) {
		delete m_SceneImage;
		m_SceneImage = nullptr;
	}

	m_VideoWriter.release();
}



void EditorVideoSaver::onStarted()
{
	m_BoxPushButton->setVisible(false);	
	onProgress(0);
	m_Box->show();
}

void EditorVideoSaver::onFinished()
{
	m_Timer.stop();

	if (m_SaveFrame >= 0)
		EditorTimeline::getInstance().setCurrentFrame(m_SaveFrame);

	QString msg = QString("\nFile saved  %1    ").arg(m_VideoSaveFilePath);
	m_Box->setText(msg);
	m_BoxPushButton->setVisible(true);

	release();
}

void EditorVideoSaver::onProgress(int progress)
{
	QString msg = QString("\nSaving file %1  (%2%)   ").arg(m_VideoSaveFilePath).arg(progress);
	m_Box->setText(msg);
}

