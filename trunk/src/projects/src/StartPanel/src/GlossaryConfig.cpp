#include "GlossaryConfig.h"
#include <QMessageBox>

GlossaryConfig::GlossaryConfig(QWidget *parent) 
{
	ui.setupUi(this);
}

GlossaryConfig::~GlossaryConfig()
{
	
}

bool GlossaryConfig::load()
{
	if (!m_Glossary.load())
		return false;

	ui.listWidget->clear();

	auto annotations = m_Glossary.getAnnotations();
	for (int i = 0; i < annotations.size(); ++i)	
		ui.listWidget->insertItem(ui.listWidget->count(), annotations[i].c_str());

	return true;
}

bool GlossaryConfig::save()
{
	return m_Glossary.save();
}

void GlossaryConfig::addAnnotation()
{
	GlossaryConfig_AddEdit gcAdd("Add annotation", "Please write your new anntotaion below:", -1, GlossaryConfig_AddEdit::Add, ui.listWidget);
	gcAdd.show();
	gcAdd.exec();
}

void GlossaryConfig::deleteAnnotation()
{
	auto cRow = ui.listWidget->currentRow();
	if (cRow == -1)
	{
		QMessageBox::information(this, tr("Glossary configuration"), tr("Please select item you want to delete first."));			
		return;
	}

	delete ui.listWidget->takeItem(cRow);
}

void GlossaryConfig::editAnnotation()
{
	auto cRow = ui.listWidget->currentRow();
	if (cRow == -1)
	{
		QMessageBox::information(this, tr("Glossary configuration"), tr("Please select item you want to edit first."));
		return;
	}
	
	GlossaryConfig_AddEdit gcEdit("Edit annotation", "Please edit your existing anntotaion below:", cRow, GlossaryConfig_AddEdit::Edit, ui.listWidget);
	gcEdit.show();
	gcEdit.exec();
}

GlossaryConfig_AddEdit::GlossaryConfig_AddEdit(const std::string _windowTitle, const std::string _label, const int _rowId, GlossaryOperation _glossaryOperation, QListWidget* _listWidget) 
	: rowId(_rowId), glossaryOperation(_glossaryOperation), listWidget(_listWidget)
{
	ui.setupUi(this);

	setWindowTitle(_windowTitle.c_str());
	ui.label->setText(_label.c_str());

	switch (glossaryOperation)
	{
		case GlossaryConfig_AddEdit::Add:
			ui.lineEdit->setText("Some new annotation...");
			ui.lineEdit->selectAll();
			break;
		case GlossaryConfig_AddEdit::Edit:
			ui.lineEdit->setText(listWidget->item(rowId)->text());
			break;
		default:
			break;
	}
}

GlossaryConfig_AddEdit::~GlossaryConfig_AddEdit()
{

}

void GlossaryConfig_AddEdit::ok()
{
	bool found = false;
	QString trimmed = ui.lineEdit->text().trimmed();

	//nie mozna dawac pustych
	if (trimmed.isEmpty())
	{
		QMessageBox::information(this, tr("Glossary configuration"), tr("Empty annotation is not allowed."));
		return;
	}

	switch (glossaryOperation)
	{
		case GlossaryConfig_AddEdit::Add:
			for (int i = 0; i < listWidget->count(); ++i)
			{
				if (!found && QString::compare(trimmed, listWidget->item(i)->text(), Qt::CaseInsensitive) == 0)
				{
					found = true;
				}
			}

			if (found)
				QMessageBox::information(this, tr("Glossary configuration"), tr("Annotation you want to add already exist."));
			else
				listWidget->addItem(trimmed);
			break;
		case GlossaryConfig_AddEdit::Edit:
			for (int i = 0; i < listWidget->count(); ++i)
				if (!found && QString::compare(trimmed, listWidget->item(i)->text(), Qt::CaseInsensitive) == 0 && i != rowId)
					found = true;

			if (found)
				QMessageBox::information(this, tr("Glossary configuration"), tr("New name for annotation you want to edit already exist."));
			else
				listWidget->item(rowId)->setText(trimmed);
			break;
		default:
			break;
	}

	//znaczy ze dodalismy/edytowalismy z sukcesem
	if (!found)
		close();
}

void GlossaryConfig_AddEdit::cancel()
{	
	close();
}