#ifndef PARTITIONANALYZERCONFIG_H
#define PARTITIONANALYZERCONFIG_H

#include "IConfigItem.h"
#include "ui_PartitionAnalyzerConfig.h"

class PartitionAnalyzerConfig : public IConfigItem
{
	Q_OBJECT

public:
	PartitionAnalyzerConfig(QWidget *parent = 0);
	~PartitionAnalyzerConfig();

	virtual bool load() override;
	virtual bool save() override;

private:
	Ui::PartitionAnalyzerConfig ui;
};

#endif // PARTITIONANALYZERCONFIG_H
