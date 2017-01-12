#pragma once
#ifndef Aquisition_h__
#define Aquisition_h__

#include <utils/IAppModule.h>

#include <boost/thread.hpp>

#include <atomic>
#include <QtCore/QObject>

namespace utils
{
	namespace camera
	{
		class ICameraRawReader;
	}
}

/// <summary>
/// klasa implementuj¹ca logikê modu³u. Na pocz¹tku ustawiane s¹ parametry, nastêpnie uruchamiany w¹tek który w miarê pojawiania siê danych z kamery tworzy pliki wyjciowe.
/// </summary>
/// <seealso cref="utils::IAppModule" />
/// <seealso cref="QObject" />
class Aquisition : public utils::IAppModule, public QObject
{
public:
	~Aquisition();

	/// <summary>
	/// Creates this instance.
	/// </summary>
	/// <returns></returns>
	static utils::IAppModule* create();
	/// <summary>
	/// Registers the parameters.
	/// </summary>
	/// <param name="programOptions">The program options.</param>
	static void registerParameters(ProgramOptions& programOptions);

	/// <summary>
	/// Starts this instance.
	/// </summary>
	/// <returns></returns>
	virtual bool start() override;
	/// <summary>
	/// Stops this instance.
	/// </summary>
	virtual void stop() override;
	/// <summary>
	/// Loads the parameters.
	/// </summary>
	/// <param name="options">The options.</param>
	/// <returns></returns>
	virtual bool loadParameters(const ProgramOptions& options) override;

private:
	Aquisition();

	static const std::string OUTPUT_FILE_EXTENSION;
	static const std::string MILESTONE_INPUT_TYPE;
	static const std::string AXIS_INPUT_TYPE;
	std::string m_OutputFolder;
	int m_SeqLength;
	std::string m_Ip;
	//std::string m_Credentials;
	std::string m_InputType;
	std::string m_User;
	std::string m_Password;
	std::string m_CameraGuid;
	size_t m_Fps;
	size_t m_Compression;
	size_t m_FrameWidth;
	size_t m_FrameHeight;

	std::shared_ptr<utils::camera::ICameraRawReader> m_FrameReader;

	std::atomic<bool> m_AquisitionRunning;
	boost::thread m_AquisitionThread;
	void aquisitionThreadFunc();
};

#endif // Aquisition_h__