#ifndef NETWORKCONFIG_H
#define NETWORKCONFIG_H

#include "IConfigItem.h"
#include "ui_NetworkConfig.h"

class NetworkConfig : public IConfigItem
{
	Q_OBJECT

public:
	NetworkConfig(QWidget *parent = 0);
	~NetworkConfig();

	virtual bool load();
	virtual bool save();

private slots:
	void addNode();
	void editNode();
	void editNode(int row, int column);
	void removeNode();
	void onSelectionChanged(int currentRow, int currentCol, int prevRow, int prewCol);
	void setPort(int port);

private:
	Ui::NetworkConfig ui;

	void setNode(int row, const QString& name, const QString& ip);
	void setItem(int row, int column, const QString& text);
};

#endif // NETWORKCONFIG_H
