#ifndef PATHANALYSISCONFIG_H
#define PATHANALYSISCONFIG_H

#include "IConfigItem.h"

namespace Ui {class PathAnalysisConfig;};

class PathAnalysisConfig : public IConfigItem
{
	Q_OBJECT

public:
	PathAnalysisConfig();
	~PathAnalysisConfig();

	virtual bool load() override;
	virtual bool save() override;

private slots:
	// general tab
	void setAnalyzer(const QString& val);
	void setUseOpenMP(bool val);
	void setAlpha(double val);
	void setMinimumPathLength(int val);

	void setEnableMerging(bool val);
	void setEnableMergingCount(int val);

	void setEnableClusterRemoving(bool val);
	void setEnableClusterRemovingCount(int val);

	void setEnableMovingPaths(bool val);
	void setEnableMergingGaps(bool val);

	// distance tab
	void setWeightY(double val);
	void setWeightX(double val);
	void setWeightTau(double val);

	void setPathThresholdsTotal(double val);
	void setPathThresholdsPositional(double val);
	void setPathThresholdsTangent(double val);

	void setClusterThresholdsTotal(double val);
	void setClusterThresholdsPositional(double val);
	void setClusterThresholdsTangent(double val);

	// paths moving tab
	void setEnableMovingPathsCount(int val);
	void setPathsPercentageMovesPerFrame(double val);
	void setMinPathsMovesPerFrame(int val);
	void setCreateNewCluster(bool val);
	void setTryMovingOnlyWhenCurrentIsTooFar(bool val);

	// merging gaps tab
	void setEnableMergingGapsCount(int val);
	void setMergingThresholdsTime(int val);
	void setMergingThresholdsX(double val);
	void setMergingThresholdsY(double val);
	void setMergingOldestClusterTime(int val);

	void setMinProcessTime(int val);
	void setMaxClusterWidth(int val);
	void setMaxClusterHeight(int val);
	
	void setMinPathsInCluster(int val);

private:
	Ui::PathAnalysisConfig *ui;

	IConfigItem* m_PartitionConfig;
};

#endif // PATHANALYSISCONFIG_H
