#pragma once

#include <QtWidgets/QListView>
#include <QtGui/QKeyEvent>
#include "EditorLayerElementsListModel.h"
#include "EditorMessageManager.h"
#include "EditorLayerElementsListFilter.h"
#include "EditorLayerElementsListDelegate.h"
#include "EditorSceneLayerManager.h"
#include "EditorLayer.h"

/**
 * \class EditorLayerElementsList
 *
 * \brief Graphics list, that represent elements stored in EditorLayer. 
 *
 */
class EditorLayerElementsList : public QListView, public EditorMessageListener
{
	Q_OBJECT

public:
	EditorLayerElementsList(QWidget *parent = 0);
	virtual ~EditorLayerElementsList();

	QModelIndex mapToSource(const QModelIndex& index) const;
	QModelIndex mapFromSource(const QModelIndex& index) const;

public slots:
	QModelIndex insertRow();
	void removeRow();
	void selectRow(const QModelIndex& index);
	void unselectRows();

	virtual void keyPressEvent(QKeyEvent *) override;
	virtual void paintEvent(QPaintEvent *) override;
	virtual void onMessage(EditorMessageType i_Message, int i_ExtraParam = -1) override;

protected:
	void init();

private:
	virtual EditorLayerElementsListModel* getListModel() = 0;
	virtual EditorLayerElementsListFilter* getListFilter() = 0;
	virtual EditorLayerElementsListDelegate* getListDelegate() = 0;
	virtual ELayer getLayerType() = 0;
	virtual int getElementId(const QModelIndex& index) = 0;

private:
	EditorLayerElementsListModel* m_ModelView;
	EditorLayerElementsListFilter* m_FilterView;
	EditorLayerElementsListDelegate* m_DelegateView;

	ELayer m_Layer;
};
