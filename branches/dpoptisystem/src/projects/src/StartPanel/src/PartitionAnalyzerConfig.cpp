#include "PartitionAnalyzerConfig.h"

#include "config/PartitionAnalyzer.h"

PartitionAnalyzerConfig::PartitionAnalyzerConfig(QWidget *parent)
{
	ui.setupUi(this);
	setParent(parent);
}

PartitionAnalyzerConfig::~PartitionAnalyzerConfig()
{

}

bool PartitionAnalyzerConfig::load()
{
	config::PartitionAnalyzer& config = config::PartitionAnalyzer::getInstance();
	config.load();

	ui.m_PointMaxDistance->setValue(config.getPointMaxDistance());

	ui.m_PathMinMovement->setValue(config.getPathMinMovement());
	ui.m_PathSpeedDiff->setValue(config.getPathSpeedDiff());
	ui.m_PathAngleDiff->setValue(config.getPathAngleDiff());

	ui.m_ClusterMinPoints->setValue(config.getClusterMinPoints());
	ui.m_ClusterMargin->setValue(config.getClusterMarign());

	ui.m_SequenceMaxDistance->setValue(config.getSequenceMaxDistance());
	ui.m_SequenceSpeedDiff->setValue(config.getSequenceSpeedDiff());
	ui.m_SequenceAngleDiff->setValue(config.getSequenceAngleDiff());
	ui.m_SequenceMinMovement->setValue(config.getSequenceMinMovement());
	ui.m_SequenceCoverLevel->setValue(config.getSequenceCoverLevel());
	ui.m_SequenceContainLevel->setValue(config.getSequenceContainLevel());
	ui.m_SequenceMinTime->setValue(config.getSequenceMinTime());
	ui.m_SequenceMinSize->setValue(config.getSequenceMinSize());
	ui.m_SequenceMaxSize->setValue(config.getSequenceMaxSize());

	return true;
}

bool PartitionAnalyzerConfig::save()
{
	config::PartitionAnalyzer& config = config::PartitionAnalyzer::getInstance();
	
	config.setPointMaxDistance(ui.m_PointMaxDistance->value());

	config.setPathMinMovement(ui.m_PathMinMovement->value());
	config.setPathSpeedDiff(ui.m_PathSpeedDiff->value());
	config.setPathAngleDiff((float)ui.m_PathAngleDiff->value());

	config.setClusterMinPoints(ui.m_ClusterMinPoints->value());
	config.setClusterMarign(ui.m_ClusterMargin->value());
	
	config.setSequenceMaxDistance(ui.m_SequenceMaxDistance->value());
	config.setSequenceSpeedDiff((float)ui.m_SequenceSpeedDiff->value());
	config.setSequenceAngleDiff(ui.m_SequenceAngleDiff->value());
	config.setSequenceMinMovement(ui.m_SequenceMinMovement->value());
	config.setSequenceCoverLevel(ui.m_SequenceCoverLevel->value());
	config.setSequenceContainLevel(ui.m_SequenceContainLevel->value());
	config.setSequenceMinTime(ui.m_SequenceMinTime->value());
	config.setSequenceMinSize(ui.m_SequenceMinSize->value());
	config.setSequenceMaxSize(ui.m_SequenceMaxSize->value());
	
	return config.save();
}
