#ifndef READONLYCHECKBOX_H
#define READONLYCHECKBOX_H

#include <QtWidgets/QCheckBox>
#include <QtGui/QMouseEvent>


class ReadOnlyCheckBox : public QCheckBox
{
	Q_OBJECT

public:
	ReadOnlyCheckBox(QWidget *parent);
	~ReadOnlyCheckBox();

	virtual void mousePressEvent(QMouseEvent* e) override { e->accept(); }
	virtual void mouseReleaseEvent(QMouseEvent* e) override { e->accept(); }
	virtual void keyPressEvent(QKeyEvent* e) override { e->accept(); }

private:
	
};

#endif // READONLYCHECKBOX_H
