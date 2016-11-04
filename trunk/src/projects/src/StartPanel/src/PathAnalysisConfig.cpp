#include "PathAnalysisConfig.h"
#include "ui_PathAnalysisConfig.h"
#include "PartitionAnalyzerConfig.h"

#include <config/PathAnalysis.h>

PathAnalysisConfig::PathAnalysisConfig()
	: IConfigItem()
{
	ui = new Ui::PathAnalysisConfig();
	ui->setupUi(this);

	m_PartitionConfig = new PartitionAnalyzerConfig(this);
	ui->m_TabWidget->addTab(m_PartitionConfig, "Partition");
}

PathAnalysisConfig::~PathAnalysisConfig()
{
	delete ui;
}

bool PathAnalysisConfig::load()
{
	config::PathAnalysis& config = config::PathAnalysis::getInstance();
	config.load();

	// general tab
	ui->m_Analyzer->setCurrentText(config.getAnalyzer().c_str());
	ui->m_TabWidget->setTabEnabled(2, false);
	ui->m_TabWidget->setTabEnabled(3, false);

	ui->m_UseOpenMP->setChecked(config.isUseOpenMP());
	ui->m_Alpha->setValue(config.getAlpha());
	ui->m_MinimumPathLength->setValue(config.getMinimumPathLength());

	ui->m_EnableMergingCount->setValue(config.getEnableMerging());
	ui->m_EnableMerging->setChecked(config.getEnableMerging() != 0);

	ui->m_EnableClusterRemovingCount->setValue(config.getEnableClusterRemoving());
	ui->m_EnableClusterRemoving->setChecked(config.getEnableClusterRemoving() != 0);

	ui->m_EnableMovingPathsCount->setValue(config.getEnableMovingPaths());
	ui->m_EnableMovingPaths->setChecked(config.getEnableMovingPaths() != 0);

	ui->m_EnableMergingGapsCount->setValue(config.getEnableMergingGaps());
	ui->m_EnableMergingGaps->setChecked(config.getEnableMergingGaps() != 0);

	// distance tab
	ui->m_WeightX->setValue(config.getWeightX());
	ui->m_WeightY->setValue(config.getWeightY());
	ui->m_WeightTau->setValue(config.getWeightTau());

	ui->m_PathThresholdsTotal->setValue(config.getPathThresholds().getTotal());
	ui->m_PathThresholdsPositional->setValue(config.getPathThresholds().getPositional());
	ui->m_PathThresholdsTangent->setValue(config.getPathThresholds().getTangent());

	ui->m_ClusterThresholdsTotal->setValue(config.getClusterThresholds().getTotal());
	ui->m_ClusterThresholdsPositional->setValue(config.getClusterThresholds().getPositional());
	ui->m_ClusterThresholdsTangent->setValue(config.getClusterThresholds().getTangent());

	// paths moving tab
	ui->m_PathsPercentageMovesPerFrame->setValue(config.getPathsPercentageMovesPerFrame());
	ui->m_MinPathsMovesPerFrame->setValue(config.getMinPathsMovesPerFrame());
	ui->m_CreateNewCluster->setChecked(config.isCreateNewCluster());
	ui->m_TryMovingOnlyWhenCurrentIsTooFar->setChecked(config.isTryMovingOnlyWhenCurrentIsTooFar());

	// merging gaps tab
	ui->m_MergingOldestClusterTime->setValue(config.getMergingOldestClusterTime());
	ui->m_MergingThresholdsTime->setValue(config.getMergingThresholds().getTime());
	ui->m_MergingThresholdsX->setValue(config.getMergingThresholds().getX());
	ui->m_MergingThresholdsY->setValue(config.getMergingThresholds().getY());

	ui->m_MinProcessTime->setValue(config.getMinProcessTime());
	ui->m_MaxClusterWidth->setValue(config.getMaxClusterWidth());
	ui->m_MaxClusterHeight->setValue(config.getMaxClusterHeight());

	ui->m_MinPathsInCluster->setValue(config.getMinPathsInCluster());

	// partition tab
	m_PartitionConfig->load();

	return true;
}

bool PathAnalysisConfig::save()
{
	return m_PartitionConfig->save() && config::PathAnalysis::getInstance().save();
}

void PathAnalysisConfig::setAnalyzer(const QString& val)
{
	config::PathAnalysis::getInstance().setAnalyzer(val.toStdString());
}

void PathAnalysisConfig::setUseOpenMP(bool val)
{
	config::PathAnalysis::getInstance().setUseOpenMP(val);
}

void PathAnalysisConfig::setAlpha(double val)
{
	config::PathAnalysis::getInstance().setAlpha(val);
}

void PathAnalysisConfig::setMinimumPathLength(int val)
{
	config::PathAnalysis::getInstance().setMinimumPathLength(val);
}

void PathAnalysisConfig::setEnableMerging(bool val)
{
	config::PathAnalysis::getInstance().setEnableMerging(val != 0 ? ui->m_EnableMergingCount->value() : 0);
	ui->m_EnableMergingCount->setEnabled(val != 0);
}

void PathAnalysisConfig::setEnableMergingCount(int val)
{
	config::PathAnalysis::getInstance().setEnableMerging(val);
}

void PathAnalysisConfig::setEnableClusterRemoving(bool val)
{
	config::PathAnalysis::getInstance().setEnableClusterRemoving(val != 0 ? ui->m_EnableClusterRemovingCount->value() : 0);
	ui->m_EnableClusterRemovingCount->setEnabled(val != 0);
}

void PathAnalysisConfig::setEnableClusterRemovingCount(int val)
{
	config::PathAnalysis::getInstance().setEnableClusterRemoving(val);
}

void PathAnalysisConfig::setEnableMovingPaths(bool val)
{
	config::PathAnalysis::getInstance().setEnableMovingPaths(val != 0 ? ui->m_EnableMovingPathsCount->value() : 0);
	ui->m_TabWidget->setTabEnabled(2, val != 0);
}

void PathAnalysisConfig::setEnableMergingGaps(bool val)
{
	config::PathAnalysis::getInstance().setEnableMergingGaps(val != 0 ? ui->m_EnableMergingGapsCount->value() : 0);
	ui->m_TabWidget->setTabEnabled(3, val != 0);
}

void PathAnalysisConfig::setWeightY(double val)
{
	config::PathAnalysis::getInstance().setWeightY(val);
}

void PathAnalysisConfig::setWeightX(double val)
{
	config::PathAnalysis::getInstance().setWeightX(val);
}

void PathAnalysisConfig::setWeightTau(double val)
{
	config::PathAnalysis::getInstance().setWeightTau(val);
}

void PathAnalysisConfig::setPathThresholdsTotal(double val)
{
	config::PathAnalysis::getInstance().getPathThresholds().setTotal(val);
}

void PathAnalysisConfig::setPathThresholdsPositional(double val)
{
	config::PathAnalysis::getInstance().getPathThresholds().setPositional(val);
}

void PathAnalysisConfig::setPathThresholdsTangent(double val)
{
	config::PathAnalysis::getInstance().getPathThresholds().setTangent(val);
}

void PathAnalysisConfig::setClusterThresholdsTotal(double val)
{
	config::PathAnalysis::getInstance().getClusterThresholds().setTotal(val);
}

void PathAnalysisConfig::setClusterThresholdsPositional(double val)
{
	config::PathAnalysis::getInstance().getClusterThresholds().setPositional(val);
}

void PathAnalysisConfig::setClusterThresholdsTangent(double val)
{
	config::PathAnalysis::getInstance().getClusterThresholds().setTangent(val);
}

void PathAnalysisConfig::setEnableMovingPathsCount(int val)
{
	config::PathAnalysis::getInstance().setEnableMovingPaths(val);
}

void PathAnalysisConfig::setPathsPercentageMovesPerFrame(double val)
{
	config::PathAnalysis::getInstance().setPathsPercentageMovesPerFrame(val);
}

void PathAnalysisConfig::setMinPathsMovesPerFrame(int val)
{
	config::PathAnalysis::getInstance().setMinPathsMovesPerFrame(val);
}

void PathAnalysisConfig::setCreateNewCluster(bool val)
{
	config::PathAnalysis::getInstance().setCreateNewCluster(val);
}

void PathAnalysisConfig::setTryMovingOnlyWhenCurrentIsTooFar(bool val)
{
	config::PathAnalysis::getInstance().setTryMovingOnlyWhenCurrentIsTooFar(val);
}

void PathAnalysisConfig::setEnableMergingGapsCount(int val)
{
	config::PathAnalysis::getInstance().setEnableMergingGaps(val);
}

void PathAnalysisConfig::setMergingThresholdsTime(int val)
{
	config::PathAnalysis::getInstance().getMergingThresholds().setTime(val);
}

void PathAnalysisConfig::setMergingThresholdsX(double val)
{
	config::PathAnalysis::getInstance().getMergingThresholds().setX(val);
}

void PathAnalysisConfig::setMergingThresholdsY(double val)
{
	config::PathAnalysis::getInstance().getMergingThresholds().setY(val);
}

void PathAnalysisConfig::setMergingOldestClusterTime(int val)
{
	config::PathAnalysis::getInstance().setMergingOldestClusterTime(val);
}

void PathAnalysisConfig::setMinProcessTime(int val)
{
	config::PathAnalysis::getInstance().setMinProcessTime(val);
}

void PathAnalysisConfig::setMaxClusterWidth(int val)
{
	config::PathAnalysis::getInstance().setMaxClusterWidth(val);
}

void PathAnalysisConfig::setMaxClusterHeight(int val)
{
	config::PathAnalysis::getInstance().setMaxClusterHeight(val);
}

void PathAnalysisConfig::setMinPathsInCluster(int val)
{
	config::PathAnalysis::getInstance().setMinPathsInCluster(val);
}

