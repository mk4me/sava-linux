#ifndef MILESTONECAMERAPAGE_H
#define MILESTONECAMERAPAGE_H

#include "ui_MilestoneCameraPage.h"

#include <QtWidgets/QWizardPage>
#include <QtCore/QFutureWatcher>

#include <boost/serialization/split_member.hpp>

class MilestoneCameraPage : public QWizardPage
{
	Q_OBJECT

	friend class boost::serialization::access;

public:
	MilestoneCameraPage(QWidget *parent = 0);
	~MilestoneCameraPage();

	std::string getCameraGuid() const;
	std::string getCameraName() const;

	virtual bool isComplete() const override { return ui.m_Cameras->currentRow() >= 0; }

private slots:
	void onCurrentCameraChanged() { emit completeChanged(); }
	void onLoadingFinished();

private:
	Ui::MilestoneCameraPage ui;
	QFutureWatcher<void> m_LoadingWatcher;

	std::string m_CameraToSelect;

	void loadCameras();

	template<class Archive>
	void save(Archive & ar, const unsigned int version) const;

	template<class Archive>
	void load(Archive & ar, const unsigned int version);

	BOOST_SERIALIZATION_SPLIT_MEMBER()
};

template<class Archive>
void MilestoneCameraPage::save(Archive & ar, const unsigned int version) const
{
	auto name = getCameraName();
	ar << name;
}

template<class Archive>
void MilestoneCameraPage::load(Archive & ar, const unsigned int version)
{
	ar & m_CameraToSelect;
}

#endif // MILESTONECAMERAPAGE_H
