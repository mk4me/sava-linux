#ifndef AQUISITIONCONFIG_H
#define AQUISITIONCONFIG_H

#include "IConfigItem.h"

namespace Ui {class AquisitionConfig;};

class AquisitionConfig : public IConfigItem
{
	Q_OBJECT

public:
	AquisitionConfig();
	~AquisitionConfig();

	virtual bool load() override;
	virtual bool save() override;

private slots:
	void setSeqLength(int seqLength);
	void setFps(int fps);
	void setCompression(int compression);

private:
	Ui::AquisitionConfig *ui;
};

#endif // AQUISITIONCONFIG_H
