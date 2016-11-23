#pragma once

#include "EditorSceneLayer.h"
#include "EditorSceneObject.h"
#include "EditorTimeline.h"
#include "EditorSceneObjectMeasure.h"

#include <QtWidgets/QRubberBand>

#include <map>
#include <set>

/**
* \class EditorSceneObjectLayer
*
* \brief Layer in scene that handles and visualizes moving objects.
*
*/
class EditorSceneObjectLayer: public EditorSceneLayer, public EditorMessageListener
{

	Q_OBJECT

	friend EditorSceneObject;

	static const Qt::KeyboardModifier OBJECT_EDIT_MODIFY_KEY = Qt::ControlModifier;
	static const Qt::KeyboardModifier OBJECT_CREATE_KEY = Qt::ShiftModifier;
	static const Qt::KeyboardModifier OBJECT_MULTI_SELECTION = Qt::AltModifier;
	static const int NEW_CLUSTER_TIME_RANGE = 20;

	enum EEditMode
	{
		EDIT_DISABLE,
		EDIT_MOVE,
		EDIT_TOP_LEFT_CORNER,
		EDIT_TOP_RIGHT_CONRNER,
		EDIT_BOTTOM_LEFT_CORNER,
		EDIT_BOTTOM_RIGHT_CORNER,
	}; 

	struct SCachedData
	{
		QPoint savedMousePos;
		QRect  savedObjRect;
		EditorSceneObject* savedObj;
	};

public:

	enum EAction
	{
		ACTION_NONE,
		ACTION_EDIT,
		ACTION_CREATE,
	};

	EditorSceneObjectLayer(const EditorLayerPtr& i_SourceLayer);
	~EditorSceneObjectLayer();

	/// <summary>
	/// Initializes this instance.
	/// </summary>
	void init() override;

	/// <summary>
	/// Redraws scene.
	/// </summary>
	void redraw() override;

	/// <summary>
	/// Reloads scene.
	/// </summary>
	void reload() override;

	/// <summary>
	/// Unselects scene objects.
	/// </summary>
	void unselectAll();

	/// <summary>
	/// Determines whether [is edit locked].
	/// </summary>
	/// <returns>bool.</returns>
	bool isEditLocked() const { return m_IsEditLocked; }


	/// <summary>
	/// Adds the scene object.
	/// </summary>
	/// <param name="i_ObjRect">The object rectangle.</param>
	void addSceneObject(const QRectF& i_ObjRect = QRectF());

	/// <summary>
	/// Gets the selected objects ids. 
	/// </summary>
	/// <returns>set of selected objects ids</returns>
	std::set<size_t> getSelectedObjectsIds() const;

	/// <summary>
	/// Select object by its identifier.
	/// </summary>
	/// <param name="i_Id">The object identifier.</param>
	void setSelectedObjectById(int i_Id, bool forceMultiselection = false);

	/// <summary>
	/// Get scene objects count.
	/// </summary>
	/// <returns>Get count of scene objects</returns>
	size_t getObjectsCount() const;

	/// <summary>
	/// Deletes the selected object.
	/// </summary>
	void deleteSelectedObject();

	
	/// <summary>
	/// Gets the current action.
	/// </summary>
	/// <returns>EditorSceneObjectLayer.EAction.</returns>
	EAction getAction() const { return m_Action; }

	/*
	*  \brief get scene object ids by cursor pos
	*  \param x - x coordinate in scene
	*  \param y - t coordinate in scene
	*  \return ids of scene regions
	*/
	std::vector<size_t> getObjectIdsByCursorPos();


protected:
	virtual void onMessage(EditorMessageType i_Message, int i_ExtraParam = -1) override;
	virtual void keyPressEvent(QKeyEvent *event) override;
	virtual void keyReleaseEvent(QKeyEvent *event) override;
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
	virtual void mouseMove(QGraphicsSceneMouseEvent* e) override;
	virtual void mouseRelease(QGraphicsSceneMouseEvent *event) override;
	virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
	virtual void wheelEvent(QGraphicsSceneWheelEvent *event) override;

private:
	QPoint getMouseScenePos();

	EditorSceneObject* createObject(size_t i_Id);
	EditorSceneObject* getObjectById(size_t i_Id);
	void deleteObject(size_t i_Id);
	void deleteObjects();
	void setSelectedObject(EditorSceneObject* i_Obj, bool forceMultiselection = false);

	bool isCursonInCorner(QRectF originalRec, QPointF cursorPos, QPointF corner);
	void updateEditMode();
	void updateObjectSize();

	void addClusterRect(size_t i_ClusterId, const QRect& i_Rect, int i_Time = -1, bool checkIfNotDuplicated = true);
	void updateCluster(int i_Time);

	bool canEdit(Qt::KeyboardModifiers modifiers) const;
	bool canCreate(Qt::KeyboardModifiers modifiers) const;
	void setAction(EAction i_Action);

	//lock scene object to edit
	void lockEditSceneObject(bool i_LockEnabled);

	EditorSceneObject* getOnlyOneSelectedObject() const;
	
private:
	//TODO dave change to map
	std::map<size_t, EditorSceneObject*> m_SceneObjects;

	//selected scene object
	std::set<EditorSceneObject*> m_SelectedObjects;

	//cache editor scene
	EditorScene* m_Scene;

	//current action
	EAction m_Action;

	//object edit mode 
	EEditMode m_EditMode;

	//save mouse position
	SCachedData m_CachedData;

	//graphics object represents object rectangle size
	EditorSceneObjectMeasure* m_ObjectMeasure;

	//is scene object locked
	bool m_IsEditLocked;
};
