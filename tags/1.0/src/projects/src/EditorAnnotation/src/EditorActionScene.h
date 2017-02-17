#ifndef EDITORACTIONSCENE_H
#define EDITORACTIONSCENE_H

#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGraphicsRectItem>

#include "EditorMessageManager.h"
#include "EditorCluster.h"
 
/// <summary>
/// Kontrolka wywietlaj¹ca podzia³ akcji wraz z ich nazwami w wybranym klastrze.
/// </summary>
/// <seealso cref="EditorMessageListener" />
class EditorActionScene : public QGraphicsScene, public EditorMessageListener
{
	Q_OBJECT

	class GraphicsAction : public QGraphicsRectItem{
		public:
			GraphicsAction(QGraphicsItem* parent = 0);

		protected:	
			virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;
	};

public:
	EditorActionScene(QGraphicsView* view, QObject *parent = 0);
	~EditorActionScene();

	void setCluster(const EditorClusterPtr& cluster) { m_Cluster = cluster; }
	void refresh(const EditorActionPtr& selectedAction = nullptr);

	EditorActionPtr selectedAction() { return m_Action; }

signals:
	void actionChanged();

protected:
	virtual void onMessage(EditorMessageType i_Message, int i_ExtraParam = -1) override;
	virtual void drawForeground(QPainter *painter, const QRectF &rect) override;
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
	virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

private slots:
	void onSelectionChanged();



private:
	QGraphicsView* m_View;
	EditorClusterPtr m_Cluster;
	EditorActionPtr m_Action;
};


Q_DECLARE_METATYPE(EditorActionPtr)

#endif // EDITORACTIONSCENE_H
