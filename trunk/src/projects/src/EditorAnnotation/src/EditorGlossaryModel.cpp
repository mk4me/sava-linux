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

	utils::Glossary::getPointer()->load();
	auto annotations = utils::Glossary::getPointer()->getAnnotations();
	for (int i = 1; i < annotations.size(); ++i)
	{
		appendRow(new QStandardItem(QString::fromStdString(annotations[i])));
	}

	endResetModel();
}
