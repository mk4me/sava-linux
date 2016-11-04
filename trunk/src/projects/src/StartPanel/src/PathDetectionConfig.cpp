#include "PathDetectionConfig.h"
#include "ui_PathDetectionConfig.h"

#include <config/PathDetection.h>

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
	config::PathDetection& config = config::PathDetection::getInstance();
	config.load();

	ui->m_MaxCostThresh->setValue(config.getMaxCostThresh());
	ui->m_MaxPathLength->setValue(config.getMaxPathLength());
	ui->m_MaxMissedFramesInPath->setValue(config.getMaxMissedFramesInPath());
	ui->m_CurrMaxDistFromPredictedNextWorldPoint->setValue(config.getCurrMaxDistFromPredictedNextWorldPoint());

	ui->m_DescCostModifier->setValue(config.getDescCostModifier());
	ui->m_DistanceModifier->setValue(config.getDistanceModifier());
	ui->m_AngleModifier->setValue(config.getAngleModifier());

	ui->m_MaxProcessTime->setValue(config.getMaxProcessTime());

	ui->m_SIFTnfeatures->setValue(config.getSIFTnfeatures());
	ui->m_SIFTnOctaveLayers->setValue(config.getSIFTnOctaveLayers());
	ui->m_SIFTContrastThreshold->setValue(config.getSIFTContrastThreshold());
	ui->m_SIFTEdgeThreshold->setValue(config.getSIFTEdgeThreshold());
	ui->m_SIFTSigma->setValue(config.getSIFTSigma());

	ui->m_SURFHessianThreshold->setValue(config.getSURFHessianThreshold());
	ui->m_SURFnOctaves->setValue(config.getSURFnOctaves());
	ui->m_SURFnOctaveLayers->setValue(config.getSURFnOctaveLayers());
	
	int detectorId = config.getDetectorId();
	if (detectorId == 0)
		ui->m_SIFTDetector->toggle();
	else if (detectorId == 1)
		ui->m_SURFDetector->toggle();


	if (config.getAlgorithm() == config::PathDetection::ALGORITHM_OPTFLOW)
	{
		ui->m_OpticalFlow->toggle();
		ui->tabWidget->setTabEnabled(1, false);
		ui->tabWidget->setTabEnabled(2, false);
	}
	else if (config.getAlgorithm() == config::PathDetection::ALGORITHM_DEFAULT)
	{
		ui->m_Hungarian->toggle();
		ui->tabWidget->setTabEnabled(1, true);
		ui->tabWidget->setTabEnabled(2, true);
	}

	ui->m_MaxPathsCount->setValue(config.getMaxPathsCount());

	return true;
}

bool PathDetectionConfig::save()
{
	return config::PathDetection::getInstance().save();
}

void PathDetectionConfig::setMaxCostThresh(int val)
{
	config::PathDetection::getInstance().setMaxCostThresh(val);
}

void PathDetectionConfig::setMaxPathLength(int val)
{
	config::PathDetection::getInstance().setMaxPathLength(val);
}

void PathDetectionConfig::setMaxMissedFramesInPath(int val)
{
	config::PathDetection::getInstance().setMaxMissedFramesInPath(val);
}

void PathDetectionConfig::setCurrMaxDistFromPredictedNextWorldPoint(double val)
{
	config::PathDetection::getInstance().setCurrMaxDistFromPredictedNextWorldPoint(val);
}

void PathDetectionConfig::setDescCostModifier(int val)
{
	config::PathDetection::getInstance().setDescCostModifier(val);
}

void PathDetectionConfig::setDistanceModifier(int val)
{
	config::PathDetection::getInstance().setDistanceModifier(val);
}

void PathDetectionConfig::setAngleModifier(int val)
{
	config::PathDetection::getInstance().setAngleModifier(val);
}

void PathDetectionConfig::setMaxProcessTime(int val)
{
	config::PathDetection::getInstance().setMaxProcessTime(val);
}

void PathDetectionConfig::setSIFTDetector(bool toggle)
{
	if (toggle)
		config::PathDetection::getInstance().setDetectorId(0);

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
		config::PathDetection::getInstance().setDetectorId(1);

	ui->m_SURFHessianThreshold->setEnabled(toggle);
	ui->m_SURFnOctaves->setEnabled(toggle);
	ui->m_SURFnOctaveLayers->setEnabled(toggle);

	ui->m_SURFHessianThresholdLabel->setEnabled(toggle);
	ui->m_SURFnOctavesLabel->setEnabled(toggle);
	ui->m_SURFnOctaveLayersLabel->setEnabled(toggle);
}

void PathDetectionConfig::setSIFTnfeatures(int val)
{
	config::PathDetection::getInstance().setSIFTnfeatures(val);
}

void PathDetectionConfig::setSIFTnOctaveLayers(int val)
{
	config::PathDetection::getInstance().setSIFTnOctaveLayers(val);
}

void PathDetectionConfig::setSIFTContrastThreshold(double val)
{
	config::PathDetection::getInstance().setSIFTContrastThreshold(val);
}

void PathDetectionConfig::setSIFTEdgeThreshold(double val)
{
	config::PathDetection::getInstance().setSIFTEdgeThreshold(val);
}

void PathDetectionConfig::setSIFTSigma(double val)
{
	config::PathDetection::getInstance().setSIFTSigma(val);
}

void PathDetectionConfig::setSURFHessianThreshold(double val)
{
	config::PathDetection::getInstance().setSURFHessianThreshold(val);
}

void PathDetectionConfig::setSURFnOctaves(int val)
{
	config::PathDetection::getInstance().setSURFnOctaves(val);
}

void PathDetectionConfig::setSURFnOctaveLayers(int val)
{
	config::PathDetection::getInstance().setSURFnOctaveLayers(val);
}

void PathDetectionConfig::setOpticalFlow(bool val)
{
	if (val)
		config::PathDetection::getInstance().setAlgorithm(config::PathDetection::ALGORITHM_OPTFLOW);
}

void PathDetectionConfig::setHungarian(bool val)
{
	if (val)
		config::PathDetection::getInstance().setAlgorithm(config::PathDetection::ALGORITHM_DEFAULT);

	ui->tabWidget->setTabEnabled(1, val);
	ui->tabWidget->setTabEnabled(2, val);
}

void PathDetectionConfig::setMaxPathsCount(int val)
{
	config::PathDetection::getInstance().setMaxPathsCount(val);
}

