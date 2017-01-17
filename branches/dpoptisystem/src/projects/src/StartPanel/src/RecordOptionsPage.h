#ifndef RECORDOPTIONSPAGE_H
#define RECORDOPTIONSPAGE_H

#include <QtWidgets/QWizardPage>
#include "ui_RecordOptionsPage.h"

#include <boost/serialization/split_member.hpp>

class RecordOptionsPage : public QWizardPage
{
	Q_OBJECT

	friend class boost::serialization::access;

public:
	RecordOptionsPage(QWidget *parent = 0);
	~RecordOptionsPage();

	bool getCompressVideo() const { return ui.m_CompressVideo->isChecked(); }
	bool getDetectObjects() const { return ui.m_DetectObjects->isChecked(); }
	bool getPredictActions() const { return ui.m_PredictActions->isChecked(); }

private slots:
	void onCompressVideo(int state);
	void onDetectObjects(int state);
	void onPredictActions(int state);

private:
	Ui::RecordOptionsPage ui;

	template<class Archive>
	void save(Archive & ar, const unsigned int version) const;

	template<class Archive>
	void load(Archive & ar, const unsigned int version);

	BOOST_SERIALIZATION_SPLIT_MEMBER()
};

template<class Archive>
void RecordOptionsPage::save(Archive & ar, const unsigned int version) const
{
	auto a = getCompressVideo();
	auto b = getDetectObjects();
	auto c = getPredictActions();
	ar << a << b << c;
}

template<class Archive>
void RecordOptionsPage::load(Archive & ar, const unsigned int version)
{
	bool compressVideo;
	bool detectObjects;
	bool predictActions;

	ar & compressVideo;
	ar & detectObjects;
	ar & predictActions;

	ui.m_CompressVideo->setChecked(compressVideo);
	ui.m_DetectObjects->setChecked(detectObjects);
	ui.m_PredictActions->setChecked(predictActions);
}

#endif // RECORDOPTIONSPAGE_H
