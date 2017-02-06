#include "MaskRegions.h"
#include "boost/archive/binary_iarchive.hpp"
#include "boost/archive/binary_oarchive.hpp"
#include "boost/serialization/vector.hpp"
#include <boost/serialization/utility.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/back_inserter.hpp>

#include <QtCore/QDebug>
#include <QtGui/QCursor>
#include <QtCore/QTimer>

#include "opencv2/imgcodecs.hpp"
#include "config/Camera.h"


/************************************ MASK POINT ************************************/
MaskPoint::MaskPoint(const QPointF& point /*= QPointF()*/, QGraphicsItem* parent /*= 0*/)
	:QGraphicsEllipseItem(parent)
{
	int radius = 6;
	setRect(-radius, -radius, 2 * radius, 2 * radius);
	setPos(point);

	setFlag(QGraphicsItem::ItemIsMovable);
	setFlag(QGraphicsItem::ItemIsFocusable);
	setFlag(QGraphicsItem::ItemIsSelectable);
}

void MaskPoint::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= 0*/)
{
	setPen(QPen(Qt::gray, 2));
	setBrush(QBrush(isSelected() ? MaskRegion::SELECTION_COLOR : Qt::black));

	painter->setRenderHint(QPainter::Antialiasing);
	QGraphicsEllipseItem::paint(painter, option, widget);
}


void MaskPoint::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsEllipseItem::mouseMoveEvent(event);
	scene()->update();
}



/*********************************** MASK REGION *************************************/

QColor MaskRegion::SELECTION_COLOR = QColor(255, 127, 0);

MaskRegion::MaskRegion(QGraphicsItem* parent /*= 0*/)
	: MaskScene::Item(parent)
	, m_IsSelected(false)
	, m_State(CLOSE)
{
	//setFlag(QGraphicsItem::ItemIsMovable);
	setFlag(QGraphicsItem::ItemIsFocusable);
	setFlag(QGraphicsItem::ItemIsSelectable);
}


MaskRegion::~MaskRegion()
{
	for (MaskPoint* p : m_Points)
		delete p;
	m_Points.clear();
}


MaskPoint* MaskRegion::addPoint(float x, float y)
{
	MaskPoint* newPoint = new MaskPoint(QPointF(x, y));
	newPoint->setParentItem(this);
	m_Points.push_back(newPoint);

	return newPoint;
}

MaskPoint* MaskRegion::addPoint(const QPointF& point)
{
	MaskPoint* newPoint = new MaskPoint(point);
	newPoint->setParentItem(this);
	m_Points.push_back(newPoint);

	return newPoint;
}

void MaskRegion::removePoint(MaskPoint* point)
{
	auto it = std::find(m_Points.begin(), m_Points.end(), point);
	if (it != m_Points.end())
	{
		m_Points.erase(it);
		delete point;
	}
}

QPolygonF MaskRegion::getPolygon() const
{
	QPolygonF polygon;

	for (MaskPoint* point : m_Points)
		polygon << QPointF(point->x(), point->y());

	return polygon;
}


void MaskRegion::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= 0*/)
{
	painter->setRenderHint(QPainter::Antialiasing);
	painter->setPen(QPen(isSelected() ? SELECTION_COLOR : Qt::black, 2));

	//draw polygon lines
	if (m_Points.size() > 1)
	{
		MaskPoint* firstPoint = m_Points.at(0);
		for (int i = 1; i < m_Points.size(); i++)
		{
			MaskPoint* secondPoint = m_Points.at(i);

			painter->drawLine(firstPoint->pos(), secondPoint->pos());
			firstPoint = secondPoint;
		}

		painter->setBrush(Qt::black);
		painter->setOpacity(m_State == OPEN ? 0.3 : 1);
		painter->drawPolygon(getPolygon());
		painter->setOpacity(1);

		painter->drawLine(m_Points.back()->pos(), m_Points.front()->pos());
	}
}

QRectF MaskRegion::boundingRect() const {
	return getPolygon().boundingRect();
}

/************************************* MASK REGIONS ***********************************/
MaskRegions::MaskRegions(QGraphicsItem* parent)
	: MaskScene::Item(parent)
	, m_Action(NONE)
	, m_SelectedRegion(nullptr)
{
	//MaskRegion* item = new MaskRegion(this);
	//item->addPoint(0, 0);
	//item->addPoint(0, 100);
	//item->addPoint(100, 100);

	//addRegion(item);

	//MaskRegion* item2 = new MaskRegion(this);
	//item2->addPoint(200, 200);
	//item2->addPoint(500, 500);
	//item2->addPoint(350, 80);

	//addRegion(item2);
}

void MaskRegions::setEnabled(bool i_Enabled)
{
	for (MaskRegion* region : m_Regions)
		for (MaskPoint* point : region->getPoints())
			point->setVisible(i_Enabled);

	QGraphicsItem::setEnabled(i_Enabled);
}


void MaskRegions::setAction(EAction newAction)
{
	if (newAction != m_Action) {
		m_Action = newAction;
		emit actionChanged();
	}
}


void MaskRegions::addRegion(MaskRegion * region)
{
	auto it = std::find(m_Regions.begin(), m_Regions.end(), region);
	if (it == m_Regions.end())
		m_Regions.push_back(region);
}


void MaskRegions::removeRegion(MaskRegion* region)
{
	auto it = std::find(m_Regions.begin(), m_Regions.end(), region);
	if (it != m_Regions.end())
	{
		m_Regions.erase(it);
		delete region;
	}
}

void MaskRegions::removeSelected()
{
	if (m_Action == NONE)
	{
		//remove selected
		std::vector<MaskRegion*> selected;
		for (MaskRegion* reg : m_Regions)
			if (reg->isSelected())
				selected.push_back(reg);

		for (MaskRegion* reg : selected)
			removeRegion(reg);
	}
	else if (m_Action == CREATING)
	{
		stopCreatinRegion();
	}
}


void MaskRegions::removeAll()
{
	stopCreatinRegion();
	for (MaskRegion* reg : m_Regions)
		delete reg;
	\
		m_Regions.clear();
}



void MaskRegions::startCreatingRegion()
{
	if (m_Action == CREATING)
		return;

	//create new region with open state
	MaskRegion* newRegion = new MaskRegion(this);
	newRegion->setState(MaskRegion::OPEN);
	addRegion(newRegion);

	//create first point and set scene cursor pos to it
	QPoint viewPoint = scene()->views().back()->mapFromGlobal(QCursor::pos());
	QPointF scenePoint = scene()->views().back()->mapToScene(viewPoint);
	MaskPoint* newPoint = newRegion->addPoint(scenePoint);
	newPoint->setSelected(false);
	newRegion->update();

	m_SelectedRegion = newRegion;
	setAction(CREATING);
}


void MaskRegions::stopCreatinRegion()
{
	setAction(NONE);
	if (m_SelectedRegion)
	{
		for (MaskPoint* p : m_SelectedRegion->getPoints())
		{
			p->setFlag(QGraphicsItem::ItemIsSelectable);
			p->setSelected(false);
		}

		if (m_SelectedRegion->getState() != MaskRegion::CLOSE)
			removeRegion(m_SelectedRegion);

		m_SelectedRegion = nullptr;
	}
}


std::vector<MaskRegion*> MaskRegions::selectedRegions() const
{
	std::vector<MaskRegion*> selected;
	if (scene())
		for (QGraphicsItem* item : scene()->selectedItems())
		{
			MaskRegion* region = dynamic_cast<MaskRegion*>(item);
			if (region)
				selected.push_back(region);
		}

	return selected;
}


cv::Mat MaskRegions::getCvMask() const
{
	int w = scene()->width();
	int h = scene()->height();

	cv::Mat m = cv::Mat(h, w, CV_8UC1);
	m = cv::Scalar(255);

	//set mask polygon on scene
	QGraphicsScene scene;
	scene.setSceneRect(0, 0, w, h);
	for (MaskRegion* region : m_Regions)
	{
		QGraphicsPolygonItem* gPolygon = new QGraphicsPolygonItem(region->getPolygon());
		gPolygon->setPen(QPen(Qt::black));
		gPolygon->setBrush(QBrush(Qt::black));

		scene.addItem(gPolygon);
	}

	//create pixmap, to draw scene on it
	QImage image(w, h, QImage::Format_ARGB32);
	image.fill(Qt::transparent);

	QPainter painter(&image);
	painter.setRenderHint(QPainter::Antialiasing);
	scene.render(&painter, scene.sceneRect(), scene.sceneRect());
	painter.end();

	for (int x = 0; x < image.width(); x++)
		for (int y = 0; y < image.height(); y++)
		{
			QColor c = QColor::fromRgba(image.pixel(x, y));
			if (c.alpha() != 0)
				m.at<uchar>(y, x) = 0;
		}

	return m;
}


void MaskRegions::mouseMove(QGraphicsSceneMouseEvent* e)
{
	if (!m_SelectedRegion)
		return;

	if (m_Action == CREATING)
	{
		//move last point with cursor
		auto points = m_SelectedRegion->getPoints();
		if (!points.empty()) {
			points.back()->setPos(e->scenePos());
		}

		//change the region state
		if (points.size() > 2)
			m_SelectedRegion->setState(points.front()->collidesWithItem(points.back()) ? MaskRegion::ALMOST_CLOSE : MaskRegion::OPEN);

		scene()->update();
	}
}


void MaskRegions::mousePress(QGraphicsSceneMouseEvent *e)
{
	if (!m_SelectedRegion)
		return;

	if (m_Action == CREATING)
	{
		auto& points = m_SelectedRegion->getPoints();
		if (points.size() > 2 && points.front()->collidesWithItem(points.back()))
		{
			m_SelectedRegion->removePoint(points.back());
			m_SelectedRegion->setState(MaskRegion::CLOSE);

			stopCreatinRegion();
		}
		else
		{
			//unselect last point
			if (!points.empty())
			{
				MaskPoint* lastPoint = points.back();
				lastPoint->setSelected(false);
				lastPoint->setFlags(lastPoint->flags() & ~QGraphicsItem::ItemIsSelectable);
			}

			MaskPoint* newPoint = m_SelectedRegion->addPoint(e->scenePos());
			newPoint->setSelected(true);
		}

		if (m_SelectedRegion)
			m_SelectedRegion->update();
	}
}

QVariant MaskRegions::itemChange(GraphicsItemChange change, const QVariant &value)
{
	if (change == ItemSceneHasChanged)
		connect(scene(), SIGNAL(selectionChanged()), SIGNAL(selectionChanged()));

	return QGraphicsItem::itemChange(change, value);
}


bool MaskRegions::load(const std::string& maskData)
{
	try
	{
		boost::iostreams::basic_array_source<char> device(maskData.data(), maskData.size());
		boost::iostreams::stream<boost::iostreams::basic_array_source<char>> s(device);
		boost::archive::binary_iarchive ia(s);
		ia >> *this;
		return true;
	}
	catch (const std::exception& e)
	{
		std::cerr << "MaskManager::load() exception: " << e.what() << std::endl;
		return false;
	}
}


bool MaskRegions::save(std::string& maskData) const
{
	try
	{
		boost::iostreams::back_insert_device<std::string> inserter(maskData);
		boost::iostreams::stream<boost::iostreams::back_insert_device<std::string>> s(inserter);
		boost::archive::binary_oarchive oa(s);
		oa << *this;
		s.flush();
		return true;
	}
	catch (const std::exception& e)
	{
		std::cerr << "MaskManager::save() exception: " << e.what() << std::endl;
		return false;
	}
}

template<class Archive>
void MaskRegions::load(Archive & ar, const unsigned int version)
{
	int num;
	ar >> num;

	for (int i = 0; i < num; i++)
	{
		std::vector<std::pair<qreal, qreal>> points;
		ar >> points;

		MaskRegion* item = new MaskRegion(this);

		for (auto& point : points)
			item->addPoint(point.first, point.second);

		m_Regions.push_back(item);
	}
}


template<class Archive>
void MaskRegions::save(Archive & ar, const unsigned int version) const
{
	int num = m_Regions.size();
	ar << num;

	for (MaskRegion* item : m_Regions)
	{
		std::vector<MaskPoint*> maskPoints = item->getPoints();
		std::vector<std::pair<qreal, qreal>> points;

		for (MaskPoint* point : maskPoints)
			points.push_back(std::make_pair(point->x(), point->y()));

		ar << points;
	}
}
