#include "MaskDialog.h"

#include "utils/ImageConverter.h"

#include <QtCore/QDebug>
#include <QtGui/QImageReader>
#include <QtCore/QTimer>
#include <QtConcurrent/QtConcurrent>

#include <assert.h>

const int MaskDialog::c_CameraWidth = 1920;
const int MaskDialog::c_CameraHeight = 1080;

MaskDialog::MaskDialog(const std::shared_ptr<utils::camera::ICameraRawReader>& camera, const std::string& maskData, QWidget *parent)
	: QDialog(parent)
	, m_MaskData(maskData)
	, m_LogTimer(nullptr)
	, m_Camera(camera)
{
	ui.setupUi(this);

	Qt::WindowFlags flags = windowFlags();
	flags &= ~Qt::WindowContextHelpButtonHint;
	flags |= Qt::WindowMaximizeButtonHint;
	setWindowFlags(flags);

	initScene();

	connect(&m_MaskRegions, SIGNAL(selectionChanged()), this, SLOT(updateGui()));
	connect(&m_MaskRegions, SIGNAL(actionChanged()), this, SLOT(updateGui()));
	connect(this, SIGNAL(backgroundChanged(const QPixmap&)), this, SLOT(setBackground(const QPixmap&)), Qt::QueuedConnection);

	updateGui();

	QtConcurrent::run(this, &MaskDialog::getBackgroundThread);
}

MaskDialog::~MaskDialog()
{
	m_Camera.reset();
}

void MaskDialog::getBackgroundThread()
{
	utils::camera::MJPGFrame frame;
	if (!m_Camera->popRawFrame(frame))
	{
		m_Camera.reset();
		return;
	}

	QPixmap image;
	if (image.loadFromData(frame.m_JPEGFrame.data(), frame.m_JPEGFrame.size()))
		emit backgroundChanged(image);

	m_Camera.reset();
}

std::string MaskDialog::getMaskData() const
{
	std::string maskData;
	if (m_MaskRegions.save(maskData))
		return maskData;

	return std::string();
}

cv::Mat MaskDialog::getMask() const
{
	return m_MaskRegions.getCvMask();
}

void MaskDialog::setBackground(const QPixmap& background)
{
	m_MaskBackground.setPixmap(background);
}

void MaskDialog::initScene()
{
	//load background image
	QPixmap bImage(c_CameraWidth, c_CameraHeight);
	bImage.fill(Qt::darkCyan);
	m_MaskBackground.setPixmap(bImage);

	//load mask regions from mask config
	if (!m_MaskData.empty())
		m_MaskRegions.load(m_MaskData);
	m_MaskRegions.setEnabled(false);

	//init scene with graphics items
	m_Scene.addItem(&m_MaskBackground);
	m_Scene.addItem(&m_MaskRegions);
	m_Scene.setSceneRect(bImage.rect());
	ui.graphicsView->setScene(&m_Scene);
}

void MaskDialog::updateGui()
{
	bool editEnable = m_MaskRegions.isEnabled();
	MaskRegions::EAction action = m_MaskRegions.getAction();
	int regionsCount = m_MaskRegions.getRegions().size();
	int selectedCount = m_MaskRegions.selectedRegions().size();

	ui.addButton->setVisible(editEnable);
	ui.removeButton->setVisible(editEnable);
	ui.clearButton->setVisible(editEnable);

	if (editEnable)
	{
		ui.addButton->setEnabled(action != MaskRegions::CREATING);
		ui.removeButton->setEnabled(action == MaskRegions::CREATING || selectedCount != 0);
		ui.clearButton->setEnabled(regionsCount != 0);
	}
}

void MaskDialog::onEditClicked() {
	bool editEnabled = ui.editButton->isChecked();

	if (!editEnabled && m_MaskRegions.getAction() == MaskRegions::CREATING)
		m_MaskRegions.removeSelected();

	m_MaskRegions.setEnabled(editEnabled);

	updateGui();

	if (editEnabled)
		log("Edit enabled.");
	else
		log("Edit disabled.");
}

void MaskDialog::onAddClicked() {
	m_MaskRegions.startCreatingRegion();
	updateGui();

	log("Action: Add");
}

void MaskDialog::onRemoveClicked() {
	m_MaskRegions.removeSelected();
	updateGui();

	log("Action: Remove");
}

void MaskDialog::onClearClicked(){
	m_MaskRegions.removeAll();
	updateGui();

	log("Action: Clear");
}


void MaskDialog::log(const std::string& msg) {
	if (m_LogTimer == nullptr)
	{
		m_LogTimer = new QTimer(this);
		m_LogTimer->setInterval(2000);
		m_LogTimer->setSingleShot(true);
		connect(m_LogTimer, SIGNAL(timeout()), ui.infoLabel, SLOT(clear()));
	}

	m_LogTimer->start();

	ui.infoLabel->setText(msg.c_str());
}
