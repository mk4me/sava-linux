#pragma once
#ifndef EDITORGLOSSARYMODEL_H
#define EDITORGLOSSARYMODEL_H

#include <QtGui/QStandardItemModel>


/**
* \class EditorGlossaryModel
*
* \brief Model for EditorGlossaryWidget
*
*/
class EditorGlossaryModel : public QStandardItemModel
{
	Q_OBJECT

public:
	EditorGlossaryModel(QObject *parent);
	~EditorGlossaryModel();

	/** \brief reload model*/
	void reload();

	/** \brief add new item to glossary model*/
	//QModelIndex add(const QString& i_Qualifier, const QString& i_Tag = QString());

	/** \brief remove item from glossary model*/
	//void remove(const QString& i_Qualifier, const QString& i_Tag = QString());

	/** \brief get qulifier-tag pair from model glossary*/
	//std::pair<std::string, std::string> getQualTagPair(const QModelIndex& index);
};

#endif // EDITORGLOSSARYMODEL_H
