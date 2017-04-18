#include "AquisitionWizard.h"

#include "utils/Filesystem.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <fstream>

AquisitionWizard::AquisitionWizard(QWidget *parent, bool showRecordOptions)
	: QWizard(parent)
	, m_ShowRecordOptions(showRecordOptions)
{
	ui.setupUi(this);

	Qt::WindowFlags flags = windowFlags();
	Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
	flags = flags & (~helpFlag);
	setWindowFlags(flags);

	setButtonText(QWizard::FinishButton, "Start");

	m_CameraTypePage = new CameraTypePage();
	m_AxisCameraPage = new AxisCameraPage();
	m_RecordOptionsPage = new RecordOptionsPage();
	m_MilestoneCameraPage = new MilestoneCameraPage();

	m_PageMap.insert(m_CameraTypePage, addPage(m_CameraTypePage));
	m_PageMap.insert(m_AxisCameraPage, addPage(m_AxisCameraPage));
	m_PageMap.insert(m_MilestoneCameraPage, addPage(m_MilestoneCameraPage));
	m_PageMap.insert(m_RecordOptionsPage, addPage(m_RecordOptionsPage));
}

AquisitionWizard::~AquisitionWizard()
{

}

int AquisitionWizard::nextId() const
{
	if (currentPage() == m_CameraTypePage)
	{
		if (m_CameraTypePage->getCameraType() == CameraTypePage::AXIS)
			return m_PageMap[m_AxisCameraPage];
		else
			return m_PageMap[m_MilestoneCameraPage];
	}
	
	if (currentPage() == m_AxisCameraPage || currentPage() == m_MilestoneCameraPage)
		return m_ShowRecordOptions ? m_PageMap[m_RecordOptionsPage] : -1;

	return -1;
}

bool AquisitionWizard::save(const std::string& file) const
{
	try
	{
		std::ofstream ofs(utils::Filesystem::getUserPath() + file, std::ios::binary | std::ios::out);
		boost::archive::binary_oarchive oa(ofs);
		oa << *this;
		return true;
	}
	catch (const std::exception& e)
	{
		std::cerr << "AquisitionWizard::save() exception: " << e.what() << std::endl;
		return false;
	}
}

bool AquisitionWizard::load(const std::string& file)
{
	try
	{
		std::ifstream ifs(utils::Filesystem::getUserPath() + file, std::ios::binary | std::ios::in);
		boost::archive::binary_iarchive ia(ifs);
		ia >> *this;
		return true;
	}
	catch (const std::exception& e)
	{
		std::cerr << "AquisitionWizard::load() exception: " << e.what() << std::endl;
		return false;
	}
}

template<class Archive>
void AquisitionWizard::serialize(Archive & ar, const unsigned int version)
{
	ar & *m_CameraTypePage;
	ar & *m_AxisCameraPage;
	ar & *m_MilestoneCameraPage;
	ar & *m_RecordOptionsPage;
}
