#include "RecordOptionsPage.h"

RecordOptionsPage::RecordOptionsPage(QWidget *parent)
	: QWizardPage(parent)
{
	ui.setupUi(this);
}

RecordOptionsPage::~RecordOptionsPage()
{

}

void RecordOptionsPage::onCompressVideo(int state)
{
	if (state == Qt::Unchecked)
		ui.m_DetectObjects->setChecked(false);
}

void RecordOptionsPage::onDetectObjects(int state)
{
	if (state == Qt::Unchecked)
		ui.m_PredictActions->setChecked(false);
	else
		ui.m_CompressVideo->setChecked(true);
}

void RecordOptionsPage::onPredictActions(int state)
{
	if (state == Qt::Checked)
		ui.m_DetectObjects->setChecked(true);
}
