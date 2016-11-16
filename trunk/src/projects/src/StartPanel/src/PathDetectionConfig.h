#ifndef PATHDETECTIONCONFIG_H
#define PATHDETECTIONCONFIG_H

#include "IConfigItem.h"

namespace Ui {class PathDetectionConfig;};

class PathDetectionConfig : public IConfigItem
{
	Q_OBJECT

public:
	PathDetectionConfig();
	~PathDetectionConfig();

	virtual bool load() override;
	virtual bool save() override;

private slots:
	void setMaxCostThresh(int val);
	void setMaxPathLength(int val);
	void setMaxMissedFramesInPath(int val);
	void setCurrMaxDistFromPredictedNextWorldPoint(double val);

	void setDescCostModifier(int val);
	void setDistanceModifier(int val);
	void setAngleModifier(int val);

	void setMaxProcessTime(int val);

	void setSIFTDetector(bool toggle);
	void setSURFDetector(bool toggle);

	void setSIFTnfeatures(int val);
	void setSIFTnOctaveLayers(int val);
	void setSIFTContrastThreshold(double val);
	void setSIFTEdgeThreshold(double val);
	void setSIFTSigma(double val);
	
	void setSURFHessianThreshold(double val);
	void setSURFnOctaves(int val);
	void setSURFnOctaveLayers(int val);

	void setOpticalFlow(bool val);
	void setHungarian(bool val);

	void setMaxPathsCount(int val);

	void setPredictedRoiDetector(bool val);
	void setPredictedRoiRadius(int val);
	void setRoiFilter(int val);
	void setRoiFilterWindow(int val);
	void setRoiFilterThreshold(int val);
	void setRoiFilterSobel(bool val);
	void setDistance0MinPathLenght(int val);
	void setDistance0MinPathDistance(int val);
	void setDistance1MinPathLenght(int val);
	void setDistance1MinPathDistance(int val);
	void setPathRemoveFilterT1(int val);
	void setPathRemoveFilterT2(int val);
	void setPathRemoveFilterN(int val);

private:
	Ui::PathDetectionConfig *ui;
};

#endif // PATHDETECTIONCONFIG_H
