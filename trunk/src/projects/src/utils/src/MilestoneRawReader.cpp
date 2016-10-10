//
//
//  @ Project : Milestone
//  @ File Name : MilestoneRawReader.cpp
//  @ Date : 2016-03-08
//  @ Author : Kamil Lebek
//
//

#include "MilestoneRawReader.h"
#include <utils/GenerateStaticMJPG.h>

// Implementation specific includes
// Cerr 
#include <iostream>
// Library functions, GetLastError()
#include <Windows.h>
// Milestone API
#include <Toolkits/ToolkitInterface.h>
#include <Toolkits/ToolkitFactoryProvider.h>
#include <Tools/ServerCommandServiceClient.h>
// For local_time_adjustor
#include <boost/date_time/c_local_time_adjustor.hpp>


//! Global utils namespace
namespace utils
{
	//! Camera support namespace
	namespace camera
	{
		// Warning about leak in Milestone's Toolkit factory
		#ifdef _DEBUG
			#pragma message(__MS_LOC__"Milestone: in DEBUG mode console will be spammed with FAKE leaks inside Milestone libraries.")
		#endif

		//
		// Class configuration
		//

		// ...

		//
		// Toolhelp functions
		//

		//! Not exposed functions
		namespace internal
		{
			//! Milestone login fuction (toolhelp function)
			static bool milestone_ServerLogin(NmServerCommandService::ImServerCommandServiceClient& client, NmServerCommandService::utf8_char_t** tokenPtr, size_t* tokenLength, NmServerCommandService::utc_time_t *registrationTime, NmServerCommandService::time_span_t *timeToLive)
			{
				// Unused status flag
				bool limited = false;

				// Call client specific function
				bool result = client.Login(tokenPtr, tokenLength, registrationTime, timeToLive, &limited);
				return result;
			}

			//! Creates authorization token
			static std::string milestone_CreateAuthToken(const std::string& inTokenKey)
			{
				// Format token text
				std::string authToken =
					"<authorization method='token'>"
					"   <token update_key='token_key'>" + inTokenKey + "</token>"
					"</authorization>";

				return authToken;
			}

			//! Create milestone config string
			static std::string milestone_CreateConfigString(const std::string& recordingServerIP, const std::string& cameraGUID, const std::string& authToken,
				const size_t inFPS, const size_t inQuality,	const size_t inWidth, const size_t inHeight)
			{
				std::string configString = "<?xml version='1.0' encoding='utf-8'?>"
					"<toolkit type='source'>"
					"  <provider>mmp</provider>"
					"  <config>"
					"    <jpeg_encoder quality='" + std::to_string(inQuality) + "' quality_update_key='qual'>"
					"     <image_boxed_resize width='" + std::to_string(inWidth) + "' height='" + std::to_string(inHeight) + "'>"
					"      <video_decoder number_of_threads='4'>"
					"        <toolkit type='source'>"
					"          <provider>is</provider>"
					"          <config>"
					"            <server_uri>http://" + recordingServerIP + "</server_uri>"
					"            <device_id>" + cameraGUID + "</device_id>"
					"            <media_type>VIDEO</media_type>" + authToken +
					"            <maximum_queue_size>5</maximum_queue_size>"
					"          </config>"
					"        </toolkit>"
					"      </video_decoder>"
					"     </image_boxed_resize>"
					"    </jpeg_encoder>"
					"  </config>"
					"</toolkit>";

				return configString;
			}

			//! Retrieves UTC offset as time difference
			static boost::posix_time::time_duration GetUtcOffset()
			{
				// boost::date_time::c_local_adjustor uses the C-API to adjust a
				// moment given in utc to the same moment in the local time zone.
				const boost::posix_time::ptime utc_now = boost::posix_time::second_clock::universal_time();
				const boost::posix_time::ptime local_now = boost::date_time::c_local_adjustor<boost::posix_time::ptime>::utc_to_local(utc_now);

				return local_now - utc_now;
			}

			//! Converts Milestone time to posix time
			static boost::posix_time::ptime milestone_ServerTimeToPtime(NmToolkit::utc_time_t inMilestoneTime)
			{
				boost::posix_time::ptime retTime = boost::posix_time::from_time_t(inMilestoneTime / 1000); // time_t is in seconds, milestone time is in ms
				return retTime + GetUtcOffset();
			}

		} // internal

		//
		// Class implementation
		//

		//! Parses camera settings (performs value check - if value is out of range, we apply defaults)
		void MilestoneRawReader::ParseCameraSettings(size_t inFPS, size_t inCompression, size_t inWidth, size_t inHeight)
		{
			// Resolution - Width (1 .. MAX)
			if ((inWidth > DEFAULT_WIDTH) || (!inWidth))
				m_Width = DEFAULT_WIDTH;
			else
				m_Width = inWidth;

			// Resolution - Height (1 .. MAX)
			if ((inHeight > DEFAULT_HEIGHT) || (!inHeight))
				m_Height = DEFAULT_HEIGHT;
			else
				m_Height = inHeight;

			// Compression (1 .. MAX) is (100 - Quality)
			if ((inCompression < 1) || (inCompression > MAX_COMPRESSION))
				m_Quality = 100 - DEFAULT_COMPRESSION;
			else
				m_Quality = 100 - inCompression;

			// FPS (0 - Unlimited, 1 .. MAX)
			if (inFPS > MAX_FPS)
				m_FPS = MAX_FPS;
			else
				m_FPS = inFPS;
		}

		//! Milestone streaming worker function (can be interrupted - will wait for blocking operation to finish)
		void MilestoneRawReader::milestone_StreamWorker()
		{
			// Error string
			std::string errString = "Reconnecting to camera (GUID: " + m_CameraGUID + ")...";
			GenerateStaticMJPG staticMJPG(errString, m_Width, m_Height, 100 - m_Quality);

			// Invoke BLOCKING operation till we get proper termination flag
			while (m_stopStreaming == false)
			{
				milestone_StreamFunc();

				// Inform about every reconnect
				if (m_stopStreaming == false)
				{
					// Emit error info
					std::cerr << errString << std::endl;

					// Put fake frame (no lock needed - we just put frame there)
					m_RawFrames.push(RawMJPGFrame(std::move(staticMJPG.getRawFrame())));

					// Add some lag (not needed - querying Milestone Toolkit in milestone_StreamFunc() takes a lot of time)
					// ...
				}
			}
		}

		//! Main thread function (performs BLOCKING stream operation - can be terminated with m_stopStreaming flag)
		bool MilestoneRawReader::milestone_StreamFunc()
		{
			// Factory provider - MUST BE on the outer scope, since we capture it to lambda
			NmToolkit::CmToolkitFactoryProvider provider;

			// Deleters
			auto toolkitFactoryDeleter = [&provider](NmToolkit::ImToolkitFactory* factoryToDestroy) { provider.DeleteInstance(factoryToDestroy); };

			// We catch all toolkit exceptions
			try
			{
				// Authentication method and product family
				NmServerCommandService::AuthenticationMethod_t authMethod = NmServerCommandService::BasicAuthentication;
				NmServerCommandService::XProtectProductFamily_t product = NmServerCommandService::XProtectEnterpriseFamily;

				// Load Command Service client
				std::unique_ptr<std::remove_pointer_t<HMODULE>, decltype(&FreeLibrary)> commandServiceModuleHandle(LoadLibrary("ServerCommandServiceClient"), &FreeLibrary);
				if (!commandServiceModuleHandle)
					return false; // LoadLibrary() failed

				// Get function pointers
				NmServerCommandService::CreateInstanceFuncPtr_t cscCreateInstanceFuncPtr = reinterpret_cast<NmServerCommandService::CreateInstanceFuncPtr_t>(GetProcAddress(commandServiceModuleHandle.get(), "CreateInstance"));
				NmServerCommandService::DeleteInstanceFuncPtr_t cscDeleteInstanceFuncPtr = reinterpret_cast<NmServerCommandService::DeleteInstanceFuncPtr_t>(GetProcAddress(commandServiceModuleHandle.get(), "DeleteInstance"));
				if (!cscCreateInstanceFuncPtr && !cscDeleteInstanceFuncPtr)
					return false; // Can't get function pointers

				// Create client instance
				std::unique_ptr<NmServerCommandService::ImServerCommandServiceClient, NmServerCommandService::DeleteInstanceFuncPtr_t> scsClient(cscCreateInstanceFuncPtr(), cscDeleteInstanceFuncPtr);

				// Setup connect details of the server command service
				scsClient->SetServerHostName(m_ServerCredentials.GetIP().c_str(), m_ServerCredentials.GetIP().length());
				scsClient->SetServerPort(MILESTONE_VMS_PORT);
				scsClient->SetUserName(m_ServerCredentials.GetUsername().c_str(), m_ServerCredentials.GetUsername().length());
				scsClient->SetPassword(m_ServerCredentials.GetPassword().c_str(), m_ServerCredentials.GetPassword().length());
				scsClient->SetAuthenticationMethod(authMethod);
				scsClient->SetServerProduct(product);

				// Login to server
				NmServerCommandService::utf8_char_t* tokenPtr = NULL;
				size_t tokenLength = 0;
				NmServerCommandService::utc_time_t registrationTime = 0;
				NmServerCommandService::time_span_t timeToLive = 0;
				bool result = internal::milestone_ServerLogin(*scsClient, &tokenPtr, &tokenLength, &registrationTime, &timeToLive);
				if (!result)
					return false; // Problem with login

				// Create configuration string
				const std::string tokenKey(tokenPtr, tokenLength);
				NmToolkit::utf8_string_t authorizationToken = internal::milestone_CreateAuthToken(tokenKey);
				NmToolkit::utf8_string_t config = internal::milestone_CreateConfigString(m_ServerCredentials.GetIP(), m_CameraGUID, authorizationToken, m_FPS, m_Quality, m_Width, m_Height);

				// Create the toolkit factory
				std::unique_ptr<NmToolkit::ImToolkitFactory, decltype(toolkitFactoryDeleter)> toolkitFactory(provider.CreateInstance(), toolkitFactoryDeleter);

				// Only continue if the toolkit factory was created successfully
				if (!toolkitFactory)
					return false; // Cannot create toolkit factory

				// We create another SCOPE, for this deleter to preserve deletion order
				auto toolkitDeleter = [&toolkitFactory](NmToolkit::ImToolkit* toolkitToDestroy) { toolkitFactory->DeleteInstance(toolkitToDestroy); };
				{

					// Create source toolkit instance
					std::unique_ptr<NmToolkit::ImToolkit, decltype(toolkitDeleter)> toolkit(toolkitFactory->CreateInstance(config), toolkitDeleter);

					// Downcast to source toolkit interface
					NmToolkit::ImSourceToolkit *sourceToolkit = dynamic_cast<NmToolkit::ImSourceToolkit *>(toolkit.get());

					// Verify that the toolkit created is a source toolkit as expected
					if (!sourceToolkit)
						return false; // Created toolkit is not a Source Toolkit
					
					// Connect to the source
					sourceToolkit->Connect();

					// Downcast to live source toolkit interface
					NmToolkit::ImLiveSourceToolkit *liveSourceToolkit = dynamic_cast<NmToolkit::ImLiveSourceToolkit *>(sourceToolkit);

					// Verify that the source toolkit created implements the live source toolkit interface
					if (!liveSourceToolkit)
						return false; // Created source toolkit does not implement the live source toolkit interface

					// Start live stream and initialize queue
					liveSourceToolkit->StartLiveStream();

					// Stream till we set semaphore to stop
					while (m_stopStreaming == false)
					{
						// Poll live queue waiting at most 10 seconds for data to arrive
						NmToolkit::ImData* rawData;
						NmToolkit::ImLiveSourceToolkit::get_live_status_t liveDataResult = liveSourceToolkit->GetLiveData(rawData, 10000);
						if (liveDataResult == NmToolkit::ImLiveSourceToolkit::LIVE_DATA_RETRIEVED)
						{
							// Downcast to media data
							NmToolkit::ImMediaData *mediaData = dynamic_cast<NmToolkit::ImMediaData *>(rawData);

							// Handle media data if that was what we retrieved
							if (mediaData)
							{
								// Copy frame to std::string
								unsigned char* rawPtr = (unsigned char*)mediaData->GetMediaBodyPointer();
								size_t rawSize = mediaData->GetMediaBodySize();
								std::vector<unsigned char> frameAsVector(rawPtr, rawPtr + rawSize);

								// Get timestamp
								NmToolkit::utc_time_t frameEndTime = mediaData->GetEndTimeStamp();
								boost::posix_time::ptime milestoneTime = internal::milestone_ServerTimeToPtime(frameEndTime);
									
								// insert frame to our queue
								m_RawFrames.push(RawMJPGFrame(std::move(frameAsVector), milestoneTime));
							}

							// Delete data instance
							delete rawData;
						}

						// Compute the time to the when the token expires in seconds
						NmServerCommandService::utc_time_t expire_time_in_milliseconds = registrationTime + timeToLive;
						time_t expire_time_in_seconds = expire_time_in_milliseconds / 1000;

						// Find how many seconds are left before the token expires.
						// NOTE: This calculation is simplified and does not take into account if time(NULL) does not return the current time in UTC.
						// If it does not, you need to convert it to UTC.
						time_t current_time = time(NULL);
						time_t time_to_token_expires = expire_time_in_seconds - current_time;

						// Renew the token if it expires in less than 30 seconds.
						if (time_to_token_expires < 30)
						{
							// Simply login again
							result = internal::milestone_ServerLogin(*scsClient, &tokenPtr, &tokenLength, &registrationTime, &timeToLive);
							if (!result)
								return false;

							// Create the new token and update the toolkit configuration with the new token.
							// We use the update key supplied in the original configuration.
							const std::string token(tokenPtr, tokenLength);
							sourceToolkit->UpdateConfiguration("token_key", token);
						}
					}

					// Stop live stream and clear the internal queue
					liveSourceToolkit->StopLiveStream();

					// Disconnect from the source
					sourceToolkit->Disconnect();
				}

				// Verify the logout method
				scsClient->Logout();

			}
			catch (const NmToolkit::ImToolkitError &error)
			{
				// Write toolkit error to standard error output
				std::cerr << "Milestone SDK: " << error.GetSpecificError() << std::endl;
			}
			catch (...)
			{
				// Unknown error
				std::cerr << "Unknown error from Milestone SDK" << std::endl;
			}

			// All ok
			return 0;

		} // Main thread function

	} // camera

} // utils