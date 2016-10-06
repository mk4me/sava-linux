#include "PathDetectionConfig.h"
#include "ui_PathDetectionConfig.h"

PathDetectionConfig::PathDetectionConfig()
	: IConfigItem()
{
	ui = new Ui::PathDetectionConfig();
	ui->setupUi(this);
}

PathDetectionConfig::~PathDetectionConfig()
{
	delete ui;
}

bool PathDetectionConfig::load()
{
	m_Config.load();

	ui->m_MaxCostThresh->setValue(m_Config.getMaxCostThresh());
	ui->m_MaxPathLength->setValue(m_Config.getMaxPathLength());
	ui->m_MaxMissedFramesInPath->setValue(m_Config.getMaxMissedFramesInPath());
	ui->m_CurrMaxDistFromPredictedNextWorldPoint->setValue(m_Config.getCurrMaxDistFromPredictedNextWorldPoint());

	ui->m_DescCostModifier->setValue(m_Config.getDescCostModifier());
	ui->m_DistanceModifier->setValue(m_Config.getDistanceModifier());
	ui->m_AngleModifier->setValue(m_Config.getAngleModifier());

	ui->m_SIFTnfeatures->setValue(m_Config.getSIFTnfeatures());
	ui->m_SIFTnOctaveLayers->setValue(m_Config.getSIFTnOctaveLayers());
	ui->m_SIFTContrastThreshold->setValue(m_Config.getSIFTContrastThreshold());
	ui->m_SIFTEdgeThreshold->setValue(m_Config.getSIFTEdgeThreshold());
	ui->m_SIFTSigma->setValue(m_Config.getSIFTSigma());

	ui->m_SURFHessianThreshold->setValue(m_Config.getSURFHessianThreshold());
	ui->m_SURFnOctaves->setValue(m_Config.getSURFnOctaves());
	ui->m_SURFnOctaveLayers->setValue(m_Config.getSURFnOctaveLayers());
	
	int detectorId = m_Config.getDetectorId();
	if (detectorId == 0)
		ui->m_SIFTDetector->toggle();
	else if (detectorId == 1)
		ui->m_SURFDetector->toggle();

	return true;
}

bool PathDetectionConfig::save()
{
	return m_Config.save();
}

void PathDetectionConfig::setMaxCostThresh(int val)
{
	m_Config.setMaxCostThresh(val);
}

void PathDetectionConfig::setMaxPathLength(int val)
{
	m_Config.setMaxPathLength(val);
}

void PathDetectionConfig::setMaxMissedFramesInPath(int val)
{
	m_Config.setMaxMissedFramesInPath(val);
}

void PathDetectionConfig::setCurrMaxDistFromPredictedNextWorldPoint(double val)
{
	m_Config.setCurrMaxDistFromPredictedNextWorldPoint(val);
}

void PathDetectionConfig::setDescCostModifier(int val)
{
	m_Config.setDescCostModifier(val);
}

void PathDetectionConfig::setDistanceModifier(int val)
{
	m_Config.setDistanceModifier(val);
}

void PathDetectionConfig::setAngleModifier(int val)
{
	m_Config.setAngleModifier(val);
}

void PathDetectionConfig::setSIFTDetector(bool toggle)
{
	if (toggle)
		m_Config.setDetectorId(0);

	ui->m_SIFTnfeatures->setEnabled(toggle);
	ui->m_SIFTnOctaveLayers->setEnabled(toggle);
	ui->m_SIFTContrastThreshold->setEnabled(toggle);
	ui->m_SIFTEdgeThreshold->setEnabled(toggle);
	ui->m_SIFTSigma->setEnabled(toggle);

	ui->m_SIFTnfeaturesLabel->setEnabled(toggle);
	ui->m_SIFTnOctaveLayersLabel->setEnabled(toggle);
	ui->m_SIFTContrastThresholdLabel->setEnabled(toggle);
	ui->m_SIFTEdgeThresholdLabel->setEnabled(toggle);
	ui->m_SIFTSigmaLabel->setEnabled(toggle);
}

void PathDetectionConfig::setSURFDetector(bool toggle)
{
	if (toggle)
		m_Config.setDetectorId(1);

	ui->m_SURFHessianThreshold->setEnabled(toggle);
	ui->m_SURFnOctaves->setEnabled(toggle);
	ui->m_SURFnOctaveLayers->setEnabled(toggle);

	ui->m_SURFHessianThresholdLabel->setEnabled(toggle);
	ui->m_SURFnOctavesLabel->setEnabled(toggle);
	ui->m_SURFnOctaveLayersLabel->setEnabled(toggle);
}

void PathDetectionConfig::setSIFTnfeatures(int val)
{
	m_Config.setSIFTnfeatures(val);
}

void PathDetectionConfig::setSIFTnOctaveLayers(int val)
{
	m_Config.setSIFTnOctaveLayers(val);
}

void PathDetectionConfig::setSIFTContrastThreshold(double val)
{
	m_Config.setSIFTContrastThreshold(val);
}

void PathDetectionConfig::setSIFTEdgeThreshold(double val)
{
	m_Config.setSIFTEdgeThreshold(val);
}

void PathDetectionConfig::setSIFTSigma(double val)
{
	m_Config.setSIFTSigma(val);
}

void PathDetectionConfig::setSURFHessianThreshold(double val)
{
	m_Config.setSURFHessianThreshold(val);
}

void PathDetectionConfig::setSURFnOctaves(int val)
{
	m_Config.setSURFnOctaves(val);
}

void PathDetectionConfig::setSURFnOctaveLayers(int val)
{
	m_Config.setSURFnOctaveLayers(val);
}

