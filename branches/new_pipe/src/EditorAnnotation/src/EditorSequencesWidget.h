#pragma once
#ifndef ANNOTATIONSEQUENCESWIDGET_H
#define ANNOTATIONSEQUENCESWIDGET_H

#include "ui_EditorSequencesWidget.h"

#include <QtWidgets/QWidget>
#include <QtWidgets/QStyledItemDelegate>

#include "EditorSequencesExplorer.h"
#include "EditorMessageManager.h"
#include "EditorTimeline.h"
#include "EditorSequencesManager.h"
 

/// <summary>
/// Kontrolka zawieraj¹ca listê wszystkich sekwencji (EditorSequencesListWidget) wraz z mo¿liwoci¹ ich przesuwania.
/// </summary>
/// <seealso cref="EditorSequencesListWidget" />
class EditorSequencesWidget : public QWidget, public EditorMessageListener
{
	Q_OBJECT

public:
	EditorSequencesWidget(QWidget *parent = 0);
	~EditorSequencesWidget();

public slots:
	void onAutoReloadChecked();
	void scrollToSelectedItem();
	void onPrevButtonClicked();
	void onNextButtonClicked();
	void onListMouseReleased();

private slots:
	void tryToReload();
	void reload();
	void onLoadingStarted();
	void onLoadingFinished();

protected:
	virtual void onMessage(EditorMessageType i_Message, int i_ExtraParam = -1) override;

private:
	bool canAutoReload(int nextFrame);
	void loadSelectedSequences();
	void selectItems();
	std::string timeToString(const sequence::IVideo::Timestamp& time);


private:
	Ui::AnnotationSequencesWidget ui;

	EditorSequencesExplorerPtr m_CachedVideoExplorer;
	EditorTimeline* m_Timeline;
	EditorSequencesManager* m_SeqManager;

	std::string m_CurrenttWorkingDir;
	std::map<std::string, QListWidgetItem*> m_ItemsMap;

	//bool m_WasPlaying;
	int m_NextFrame;
};



#endif // ANNOTATIONSEQUENCESWIDGET_H
