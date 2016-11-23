#include "EditorGlossaryModel.h"
#include "EditorWindow.h"

/*--------------------------------------------------------------------*/
EditorGlossaryModel::EditorGlossaryModel(QObject *parent)
: QStandardItemModel(parent)
{
}

/*--------------------------------------------------------------------*/
EditorGlossaryModel::~EditorGlossaryModel()
{
}

/*--------------------------------------------------------------------*/
void EditorGlossaryModel::reload()
{
	clear();

	beginResetModel();

	config::Glossary::getInstance().load();
	for (auto& action : config::Glossary::getInstance().getEditorActions())
	{
		appendRow(new QStandardItem(action.name.c_str()));
	}

	endResetModel();
}
