#pragma once
#ifndef EDITORGLOSSARYWIDGET_H
#define EDITORGLOSSARYWIDGET_H

#include "ui_EditorGlossaryWidget.h"
#include "EditorGlossaryModel.h"
#include "EditorMessageManager.h"
#include <QtWidgets/QWidget>
#include <QtCore/QSortFilterProxyModel>
#include <QtGui/QKeyEvent>

typedef std::list <std::pair<std::string, std::string>> QualTagList;

/**
* \class EditorGlossaryWidget
*
* \brief Widget to show editor glossary content 
*
*/
class EditorGlossaryWidget : public QWidget, public EditorMessageListener
{
	Q_OBJECT

public:
	EditorGlossaryWidget(QWidget *parent = 0);
	~EditorGlossaryWidget();

	/** \brief get glossary widget model view*/
	EditorGlossaryModel* getModelView() { return m_ModelView; }

signals:
	/** \brief emit when double clicked on glossary widget item */
	void doubleClicked(const QModelIndex& index);

private slots:
	void onClicked(const QModelIndex& index);
	void onDoubleClicked(const QModelIndex& index);
	void filterChanged(const QString& text);

protected:
	virtual void onMessage(EditorMessageType i_Message, int i_ExtraParam = -1) override;
	virtual void keyPressEvent(QKeyEvent *) override;

private:
	Ui::EditorGlossaryWidget ui;
	EditorGlossaryModel* m_ModelView;

	/*--------------------------------------------------------------------*/
	class GlossaryFilter : public QSortFilterProxyModel{
		virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override{
			QModelIndex currentIndex = sourceModel()->index(source_row, 0, source_parent);
			int childNum = sourceModel()->rowCount(currentIndex);
			if (childNum != 0){
				for (int i = 0; i < childNum; i++){
					QModelIndex child = currentIndex.child(i, 0);
					if (sourceModel()->data(child).toString().contains(filterRegExp()))
						return true;
				}
			}
			return sourceModel()->data(currentIndex).toString().contains(filterRegExp());
		};
	};
	/*--------------------------------------------------------------------*/

	GlossaryFilter m_Filter;
};

#endif // EDITORGLOSSARYWIDGET_H
