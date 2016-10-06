#include "PathAnalysisConfig.h"
#include "ui_PathAnalysisConfig.h"

PathAnalysisConfig::PathAnalysisConfig()
	: IConfigItem()
{
	ui = new Ui::PathAnalysisConfig();
	ui->setupUi(this);
}

PathAnalysisConfig::~PathAnalysisConfig()
{
	delete ui;
}

bool PathAnalysisConfig::load()
{
	m_Config.load();

	// general tab
	ui->m_TabWidget->setTabEnabled(2, false);
	ui->m_TabWidget->setTabEnabled(3, false);

	ui->m_UseOpenMP->setChecked(m_Config.isUseOpenMP());
	ui->m_Alpha->setValue(m_Config.getAlpha());
	ui->m_MinimumPathLength->setValue(m_Config.getMinimumPathLength());

	ui->m_EnableMergingCount->setValue(m_Config.getEnableMerging());
	ui->m_EnableMerging->setChecked(m_Config.getEnableMerging() != 0);

	ui->m_EnableClusterRemovingCount->setValue(m_Config.getEnableClusterRemoving());
	ui->m_EnableClusterRemoving->setChecked(m_Config.getEnableClusterRemoving() != 0);

	ui->m_EnableMovingPathsCount->setValue(m_Config.getEnableMovingPaths());
	ui->m_EnableMovingPaths->setChecked(m_Config.getEnableMovingPaths() != 0);

	ui->m_EnableMergingGapsCount->setValue(m_Config.getEnableMergingGaps());
	ui->m_EnableMergingGaps->setChecked(m_Config.getEnableMergingGaps() != 0);

	// distance tab
	ui->m_WeightX->setValue(m_Config.getWeightX());
	ui->m_WeightY->setValue(m_Config.getWeightY());
	ui->m_WeightTau->setValue(m_Config.getWeightTau());

	ui->m_PathThresholdsTotal->setValue(m_Config.getPathThresholds().getTotal());
	ui->m_PathThresholdsPositional->setValue(m_Config.getPathThresholds().getPositional());
	ui->m_PathThresholdsTangent->setValue(m_Config.getPathThresholds().getTangent());

	ui->m_ClusterThresholdsTotal->setValue(m_Config.getClusterThresholds().getTotal());
	ui->m_ClusterThresholdsPositional->setValue(m_Config.getClusterThresholds().getPositional());
	ui->m_ClusterThresholdsTangent->setValue(m_Config.getClusterThresholds().getTangent());

	// paths moving tab
	ui->m_PathsPercentageMovesPerFrame->setValue(m_Config.getPathsPercentageMovesPerFrame());
	ui->m_MinPathsMovesPerFrame->setValue(m_Config.getMinPathsMovesPerFrame());
	ui->m_CreateNewCluster->setChecked(m_Config.isCreateNewCluster());
	ui->m_TryMovingOnlyWhenCurrentIsTooFar->setChecked(m_Config.isTryMovingOnlyWhenCurrentIsTooFar());

	// merging gaps tab
	ui->m_MergingOldestClusterTime->setValue(m_Config.getMergingOldestClusterTime());
	ui->m_MergingThresholdsTime->setValue(m_Config.getMergingThresholds().getTime());
	ui->m_MergingThresholdsX->setValue(m_Config.getMergingThresholds().getX());
	ui->m_MergingThresholdsY->setValue(m_Config.getMergingThresholds().getY());

	return true;
}

bool PathAnalysisConfig::save()
{
	return m_Config.save();
}

void PathAnalysisConfig::setUseOpenMP(bool val)
{
	m_Config.setUseOpenMP(val);
}

void PathAnalysisConfig::setAlpha(double val)
{
	m_Config.setAlpha(val);
}

void PathAnalysisConfig::setMinimumPathLength(int val)
{
	m_Config.setMinimumPathLength(val);
}

void PathAnalysisConfig::setEnableMerging(bool val)
{
	m_Config.setEnableMerging(val != 0 ? ui->m_EnableMergingCount->value() : 0);
	ui->m_EnableMergingCount->setEnabled(val != 0);
}

void PathAnalysisConfig::setEnableMergingCount(int val)
{
	m_Config.setEnableMerging(val);
}

void PathAnalysisConfig::setEnableClusterRemoving(bool val)
{
	m_Config.setEnableClusterRemoving(val != 0 ? ui->m_EnableClusterRemovingCount->value() : 0);
	ui->m_EnableClusterRemovingCount->setEnabled(val != 0);
}

void PathAnalysisConfig::setEnableClusterRemovingCount(int val)
{
	m_Config.setEnableClusterRemoving(val);
}

void PathAnalysisConfig::setEnableMovingPaths(bool val)
{
	m_Config.setEnableMovingPaths(val != 0 ? ui->m_EnableMovingPathsCount->value() : 0);
	ui->m_TabWidget->setTabEnabled(2, val != 0);
}

void PathAnalysisConfig::setEnableMergingGaps(bool val)
{
	m_Config.setEnableMergingGaps(val != 0 ? ui->m_EnableMergingGapsCount->value() : 0);
	ui->m_TabWidget->setTabEnabled(3, val != 0);
}

void PathAnalysisConfig::setWeightY(double val)
{
	m_Config.setWeightY(val);
}

void PathAnalysisConfig::setWeightX(double val)
{
	m_Config.setWeightX(val);
}

void PathAnalysisConfig::setWeightTau(double val)
{
	m_Config.setWeightTau(val);
}

void PathAnalysisConfig::setPathThresholdsTotal(double val)
{
	m_Config.getPathThresholds().setTotal(val);
}

void PathAnalysisConfig::setPathThresholdsPositional(double val)
{
	m_Config.getPathThresholds().setPositional(val);
}

void PathAnalysisConfig::setPathThresholdsTangent(double val)
{
	m_Config.getPathThresholds().setTangent(val);
}

void PathAnalysisConfig::setClusterThresholdsTotal(double val)
{
	m_Config.getClusterThresholds().setTotal(val);
}

void PathAnalysisConfig::setClusterThresholdsPositional(double val)
{
	m_Config.getClusterThresholds().setPositional(val);
}

void PathAnalysisConfig::setClusterThresholdsTangent(double val)
{

	m_Config.getClusterThresholds().setTangent(val);
}

void PathAnalysisConfig::setEnableMovingPathsCount(int val)
{
	m_Config.setEnableMovingPaths(val);
}

void PathAnalysisConfig::setPathsPercentageMovesPerFrame(double val)
{
	m_Config.setPathsPercentageMovesPerFrame(val);
}

void PathAnalysisConfig::setMinPathsMovesPerFrame(int val)
{
	m_Config.setMinPathsMovesPerFrame(val);
}

void PathAnalysisConfig::setCreateNewCluster(bool val)
{
	m_Config.setCreateNewCluster(val);
}

void PathAnalysisConfig::setTryMovingOnlyWhenCurrentIsTooFar(bool val)
{
	m_Config.setTryMovingOnlyWhenCurrentIsTooFar(val);
}

void PathAnalysisConfig::setEnableMergingGapsCount(int val)
{
	m_Config.setEnableMergingGaps(val);
}

void PathAnalysisConfig::setMergingThresholdsTime(int val)
{
	m_Config.getMergingThresholds().setTime(val);
}

void PathAnalysisConfig::setMergingThresholdsX(double val)
{
	m_Config.getMergingThresholds().setX(val);
}

void PathAnalysisConfig::setMergingThresholdsY(double val)
{
	m_Config.getMergingThresholds().setY(val);
}

void PathAnalysisConfig::setMergingOldestClusterTime(int val)
{
	m_Config.setMergingOldestClusterTime(val);
}

