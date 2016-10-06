#ifndef ANNOTATIONSEQUENCESWIDGET_H
#define ANNOTATIONSEQUENCESWIDGET_H

#include "ui_EditorSequencesWidget.h"

#include <QtWidgets/QWidget>
#include <QtWidgets/QStyledItemDelegate>

#include "EditorSequencesExplorer.h"
#include "EditorMessageManager.h"
#include "EditorTimeline.h"
#include "EditorSequencesManager.h"

/**
* \class EditorSequencesWidget
*
* \brief Widget to show all loaded video sequences from working directory.
*
*/
class EditorSequencesWidget : public QWidget, public EditorMessageListener
{
	Q_OBJECT

public:
	EditorSequencesWidget(QWidget *parent = 0);
	~EditorSequencesWidget();

public slots:
	void onItemClicked(QListWidgetItem* item);
	void onAutoReloadChecked();

private slots:
	void tryToReload();
	void reload();

protected:
	virtual void onMessage(EditorMessageType i_Message, int i_ExtraParam = -1) override;

private:
	bool canAutoReload(int nextFrame);

private:
	Ui::AnnotationSequencesWidget ui;

	EditorSequencesExplorerPtr m_CachedVideoExplorer;
	EditorTimeline* m_Timeline;
	EditorSequencesManager* m_SeqManager;

	bool m_WasPlaying;
	int m_NextFrame;
};



#endif // ANNOTATIONSEQUENCESWIDGET_H
