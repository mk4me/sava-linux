#ifndef AXISCAMERAPAGE_H
#define AXISCAMERAPAGE_H

#include <QtWidgets/QWizardPage>
#include "ui_AxisCameraPage.h"

#include <boost/serialization/split_member.hpp>

class AxisCameraPage : public QWizardPage
{
	Q_OBJECT

	friend class boost::serialization::access;

public:
	AxisCameraPage(QWidget *parent = 0);
	~AxisCameraPage();

	int getCameraIndex() const { return ui.m_Cameras->currentRow(); }

	virtual bool isComplete() const override { return ui.m_Cameras->currentRow() >= 0; }

private slots:
	void onCurrentCameraChanged() { emit completeChanged(); }

private:
	Ui::AxisCameraPage ui;

	template<class Archive>
	void save(Archive & ar, const unsigned int version) const;

	template<class Archive>
	void load(Archive & ar, const unsigned int version);

	BOOST_SERIALIZATION_SPLIT_MEMBER()
};

template<class Archive>
void AxisCameraPage::save(Archive & ar, const unsigned int version) const
{
    auto idx = getCameraIndex();
	ar << idx;
}

template<class Archive>
void AxisCameraPage::load(Archive & ar, const unsigned int version)
{
	int index;
	ar >> index;

	if (ui.m_Cameras->count() > index && index > 0)
		ui.m_Cameras->setCurrentRow(index);
}

#endif // AXISCAMERAPAGE_H
