#pragma once

#include <iostream>
#include <string>
#include <fstream>

#include "boost/serialization/access.hpp"
#include <boost/serialization/split_member.hpp>
#include <opencv/cv.h>

#include <QtGui/QPolygon>
#include <QtWidgets/QGraphicsItem>
#include <QtWidgets/QGraphicsScene>
#include <QtGui/QPainter>
#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QtCore/QDebug>

#include "MaskView.h"
#include "utils/CvSerialization.h"


/************************************************************************/
class MaskPoint : public QGraphicsEllipseItem
{
	public:
		MaskPoint(const QPointF& point = QPointF(), QGraphicsItem* parent = 0);

	protected:
		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;
		virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
};




/************************************************************************/
class MaskRegion : public MaskScene::Item
{

public:
	static QColor SELECTION_COLOR;

	enum EState
	{
		OPEN,
		ALMOST_CLOSE,
		CLOSE,
	};

	MaskRegion(QGraphicsItem* parent = 0);
	~MaskRegion();

	MaskPoint* addPoint(const QPointF& point);
	MaskPoint* addPoint(float x, float y);

	void removePoint(MaskPoint* point);
	//void removePoint(size_t index);

	std::vector<MaskPoint*>& getPoints() { return m_Points; }
	QPolygonF getPolygon() const;

	void setState(EState newState) { m_State = newState; }
	EState getState() const { return m_State; }

protected:
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;
	virtual QRectF boundingRect() const override;

private:
	std::vector<MaskPoint*> m_Points;

	EState m_State;
	bool m_IsSelected;
};




/************************************************************************/
class MaskRegions : public QObject, public MaskScene::Item
{
	Q_OBJECT

	friend class boost::serialization::access;

public:
	enum EAction
	{
		NONE,
		CREATING,
	};

	MaskRegions(QGraphicsItem* parent = 0);

	void setEnabled(bool i_Enabled);

	bool load(const std::string& maskData);
	bool save(std::string& maskData) const;
	cv::Mat getCvMask() const;

	void addRegion(MaskRegion * region);
	void removeRegion(MaskRegion* region);
	void removeSelected();
	void removeAll();

	const std::vector<MaskRegion*>& getRegions() { return m_Regions; }

	void startCreatingRegion();
	void stopCreatinRegion();

	EAction getAction() const { return m_Action; }
	std::vector<MaskRegion*> selectedRegions() const;

signals:
	void selectionChanged();
	void actionChanged();

protected:
	void setAction(EAction newAction);

	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override {}
	virtual void mouseMove(QGraphicsSceneMouseEvent* e) override;
	virtual void mousePress(QGraphicsSceneMouseEvent *e) override;
	virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
	template<class Archive>
	void load(Archive & ar, const unsigned int version);

	template<class Archive>
	void save(Archive & ar, const unsigned int version) const;

	BOOST_SERIALIZATION_SPLIT_MEMBER()


private:
	//vector of mask regions
	std::vector<MaskRegion*> m_Regions;
	MaskRegion* m_SelectedRegion;
	
	EAction m_Action;

};



