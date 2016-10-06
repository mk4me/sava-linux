#ifndef COMPRESSIONCONFIG_H
#define COMPRESSIONCONFIG_H

#include "IConfigItem.h"

#include <config/Compression.h>

namespace Ui {class CompressionConfig;};

class CompressionConfig : public IConfigItem
{
	Q_OBJECT

public:
	CompressionConfig();
	~CompressionConfig();

	virtual bool load() override;
	virtual bool save() override;

private slots:
	void setFilesInPackage(int val);
	void setImageCompression(int val);
	void setBackgroundHistory(int val);
	void setDifferenceThreshold(int val);
	void setNewBackgroundMinPixels(double val);
	void setMinCrumbleArea(int val);
	void setMergeCrumblesIterations(int val);

private:
	Ui::CompressionConfig *ui;
	config::Compression m_Config;
};

#endif // COMPRESSIONCONFIG_H
