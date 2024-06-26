#pragma once
#ifndef EDITORANNOTATIONLISTVIEW_H
#define EDITORANNOTATIONLISTVIEW_H

#include "EditorAnnotationListModel.h"
#include "EditorAnnotationListDelegate.h"
#include "EditorMessageManager.h"

#include <QtWidgets/QTableView>


/// <summary>
/// Implementuje widok listy w panelu adnotacji.
/// </summary>
/// <seealso cref="EditorMessageListener" />
class EditorAnnotationListView : public QTableView, public EditorMessageListener
{
	Q_OBJECT

public:
	EditorAnnotationListView(QWidget *parent);
	~EditorAnnotationListView();

protected:

	/// <summary>
	/// Ovveride widget paint method. 
	/// Paint widget timeline line in this method.
	/// </summary>
	/// <param name="">The .</param>
	virtual void paintEvent(QPaintEvent *) override;
	virtual void keyReleaseEvent(QKeyEvent *) override;
	virtual void onMessage(EditorMessageType i_Message, int i_ExtraParam = -1) override;

private:
	void scrollToSelected();



	EditorAnnotationListModel* m_Model;
};

#endif // EDITORANNOTATIONLISTVIEW_H
