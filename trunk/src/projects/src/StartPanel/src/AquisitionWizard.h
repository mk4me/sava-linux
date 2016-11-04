#ifndef AQUISITIONWIZARD_H
#define AQUISITIONWIZARD_H

#include "ui_AquisitionWizard.h"
#include "CameraTypePage.h"
#include "AxisCameraPage.h"
#include "RecordOptionsPage.h"
#include "MilestoneCameraPage.h"

#include <QtWidgets/QWizard>
#include <QtCore/QMap>


class AquisitionWizard : public QWizard
{
	Q_OBJECT

	friend class boost::serialization::access;

public:
	AquisitionWizard(QWidget *parent = 0, bool showRecordOptions = true);
	~AquisitionWizard();

	virtual int nextId() const override;

	CameraTypePage* getCameraTypePage() const { return m_CameraTypePage; }
	AxisCameraPage* getAxisCameraPage() const { return m_AxisCameraPage; }
	RecordOptionsPage* getRecordOptionsPage() const { return m_RecordOptionsPage; }
	MilestoneCameraPage* getMilestoneCameraPage() const { return m_MilestoneCameraPage; }

	bool save(const std::string& file) const;
	bool load(const std::string& file);

private:
	Ui::AquisitionWizard ui;

	bool m_ShowRecordOptions;

	CameraTypePage* m_CameraTypePage;
	AxisCameraPage* m_AxisCameraPage;
	RecordOptionsPage* m_RecordOptionsPage;
	MilestoneCameraPage* m_MilestoneCameraPage;

	QMap<QWizardPage*, int> m_PageMap;

	template<class Archive>
	void serialize(Archive & ar, const unsigned int version);
};

#endif // AQUISITIONWIZARD_H
