#pragma once
#ifndef IConfigItem_h__
#define IConfigItem_h__

#include <QWidget>

class IConfigItem : public QWidget
{
public:
	IConfigItem() : QWidget(nullptr) { }
	virtual ~IConfigItem() { }

	virtual bool load() = 0;
	virtual bool save() = 0;
};

#endif // IConfigItem_h__
