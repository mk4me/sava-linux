#pragma once
#ifndef Aquisition_h__
#define Aquisition_h__

#include <utils/IAppModule.h>

#include <boost/thread.hpp>

#include <atomic>

namespace utils
{
	namespace camera
	{
		class CAxisRawReader;
	}
}

class Aquisition : public utils::IAppModule
{

public:
	~Aquisition();

	static utils::IAppModule* create();
	static void registerParameters(ProgramOptions& programOptions);

	virtual bool start() override;
	virtual void stop() override;
	virtual bool loadParameters(const ProgramOptions& options) override;

private:
	Aquisition();

	static const std::string OUTPUT_FILE_EXTENSION;
	std::string m_OutputFolder;
	int m_SeqLength;
	std::string m_Ip;
	std::string m_Credentials;
	size_t m_Fps;
	size_t m_Compression;
	size_t m_FrameWidth;
	size_t m_FrameHeight;

	std::shared_ptr<utils::camera::CAxisRawReader> m_FrameReader;

	std::atomic<bool> m_AquisitionRunning;
	boost::thread m_AquisitionThread;
	void aquisitionThreadFunc();
};

#endif // Aquisition_h__