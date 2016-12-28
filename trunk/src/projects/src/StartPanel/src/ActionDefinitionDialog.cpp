#include "ActionDefinitionDialog.h"

ActionDefinitionDialog::ActionDefinitionDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	Qt::WindowFlags flags = windowFlags();
	Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
	flags = flags & (~helpFlag);
	setWindowFlags(flags);
}

ActionDefinitionDialog::~ActionDefinitionDialog()
{

}
