#ifndef CAMERATYPEPAGE_H
#define CAMERATYPEPAGE_H

#include "ui_CameraTypePage.h"

#include <QtWidgets/QWizardPage>

#include <boost/serialization/split_member.hpp>

class CameraTypePage : public QWizardPage
{
	Q_OBJECT

	friend class boost::serialization::access;

public:
	enum CameraType
	{
		AXIS,
		MILESTONE,
	};

	CameraTypePage(QWidget* parent = 0);
	~CameraTypePage();

	CameraType getCameraType() const { return ui.m_Axis->isChecked() ? AXIS : MILESTONE; }

private:
	Ui::CameraTypePage ui;

	template<class Archive>
	void save(Archive & ar, const unsigned int version) const;

	template<class Archive>
	void load(Archive & ar, const unsigned int version);

	BOOST_SERIALIZATION_SPLIT_MEMBER()
};

template<class Archive>
void CameraTypePage::save(Archive & ar, const unsigned int version) const
{
    int ct = (int)getCameraType();
	ar << ct;
}

template<class Archive>
void CameraTypePage::load(Archive & ar, const unsigned int version)
{
	int camType;
	ar >> camType;

	switch (camType)
	{
	case CameraTypePage::AXIS:
		ui.m_Axis->setChecked(true);
		break;
	case CameraTypePage::MILESTONE:
		ui.m_Milestone->setChecked(true);
		break;
	}
}

#endif // CAMERATYPEPAGE_H
