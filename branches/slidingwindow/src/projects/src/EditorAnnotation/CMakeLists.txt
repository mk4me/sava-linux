BEGIN_PROJECT("executable")


SET_PRIVATE_HEADERS(
EditorAction.h
EditorActionWidget.h
EditorActionScene.h
EditorAnnotationListDelegate.h
EditorAnnotationListModel.h
EditorAnnotationListView.h
EditorAnnotationWidget.h
EditorAutoSaveTimer.h
EditorCluster.h
EditorConfig.h
EditorConfigDialog.h
EditorGraphicsView.h
EditorLayer.h
EditorLayerManager.h
EditorMessageManager.h
EditorMultiSequence.h
EditorPopup.h
EditorScene.h
EditorSceneContextMenu.h
EditorSceneLayer.h
EditorSceneLayerManager.h
EditorSceneObject.h
EditorSceneObjectLayer.h
EditorSequencesListWidget.h
EditorSequencesManager.h
EditorSequencesWidget.h
EditorTimeline.h
EditorTimelineSlider.h
EditorTimelineWidget.h
EditorVideoSaver.h
EditorWindow.h
)


SET_SOURCE_FILES(
EditorAction.cpp
EditorActionWidget.cpp
EditorActionScene.cpp
EditorAnnotationListDelegate.cpp
EditorAnnotationListModel.cpp
EditorAnnotationListView.cpp
EditorAnnotationWidget.cpp
EditorAutoSaveTimer.cpp
EditorCluster.cpp
EditorConfig.cpp
EditorConfigDialog.cpp
EditorGraphicsView.cpp
EditorLayer.cpp
EditorLayerManager.cpp
EditorMessageManager.cpp
EditorMultiSequence.cpp
EditorPopup.cpp
EditorScene.cpp
EditorSceneContextMenu.cpp
EditorSceneLayer.cpp
EditorSceneLayerManager.cpp
EditorSceneObject.cpp
EditorSceneObjectLayer.cpp
EditorSceneObjectMeasure.cpp
EditorSequence.cpp
EditorSequencesExplorer.cpp
EditorSequencesListWidget.cpp
EditorSequencesManager.cpp
EditorSequencesWidget.cpp
EditorSingleSequence.cpp
EditorTimeline.cpp
EditorTimelineSlider.cpp
EditorTimelineWidget.cpp
EditorVideoSaver.cpp
EditorWindow.cpp
main.cpp
)

SET_UI_FILES(
EditorActionWidget.ui
EditorAnnotationWidget.ui
EditorConfigDialog.ui
EditorLayerTreeView.ui
EditorPopup.ui
EditorSequencesWidget.ui
EditorTimelineWidget.ui
EditorWindow.ui
)

SET_MOC_FILES(
EditorAnnotationListDelegate.h
EditorAnnotationListModel.h
EditorAnnotationListView.h
EditorAnnotationWidget.h
EditorAutoSaveTimer.h
EditorActionWidget.h
EditorActionScene.h
EditorConfigDialog.h
EditorGraphicsView.h
EditorPopup.h
EditorScene.h
EditorSceneContextMenu.h
EditorSceneLayer.h
EditorSceneLayerManager.h
EditorSceneObject.h
EditorSceneObjectLayer.h
EditorSceneObjectMeasure.h
EditorSequence.h
EditorSequencesExplorer.h
EditorSequencesListWidget.h
EditorSequencesManager.h
EditorSequencesWidget.h
EditorSingleSequence.h
EditorSingletons.h
EditorTimeline.h
EditorTimelineSlider.h
EditorTimelineWidget.h
EditorVideoSaver.h
EditorWindow.h
)

#SET_RC_FILES ()
END_PROJECT()
