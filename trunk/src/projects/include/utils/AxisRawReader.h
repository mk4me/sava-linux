//
//
//  @ Project : Milestone
//  @ File Name : AxisRawReader.h
//  @ Date : 2016-01-28
//  @ Author : Kamil Lebek
//
//


#ifndef _AXISRAWREADER_H
#define _AXISRAWREADER_H

//
// CURL
//

// Lib (CURL is shipped in Relase, so no difference)
#ifdef _DEBUG
#pragma comment(lib, "libcurl_imp.lib")
#else
#pragma comment(lib, "libcurl_imp.lib")
#endif

#include <string>
#include <thread>
#include <atomic>

// Common interface definition
#include "ICameraRawReader.h"

// Blocking Queue and Raw Frame
#include "BlockingQueue.h"
#include "RawMJFPFrame.h"

//! Global utils namespace
namespace utils
{
	//! Camera support namespace
	namespace camera
	{

		//! Class providing RAW MJPG access to the Axis Camera (frames as MJPG raw bytes)
		class AxisRawReader : public ICameraRawReader
		{
		public:

			//! Explicit constructor to avoid conversion - creates Axis camera stream reader and starts streaming immediately
			/*!
				\param inCameraIP camera IP string e.g. "1.2.3.4" ("http://" is added automatically)
				\param inUserAndPwd camera login e.g. "user:pwd" (must be supplied)
				\param inFPS desired frame rate (range: 1 to 25) - 0 means maximum rate (default setting)
				\param inCompression compression rate from 1 (lowest) to 99 (highest) - 30 is default setting
				\param inWidth frame width - default is 1920
				\param inHeight frame height - default is 1080
				*/
			explicit AxisRawReader(const std::string& inCameraIP, const std::string& inUserAndPwd,
				const size_t inFPS = DEFAULT_FPS, const size_t inCompression = DEFAULT_COMPRESSION,
				const size_t inWidth = DEFAULT_WIDTH, const size_t inHeight = DEFAULT_HEIGHT)
				: m_CameraIP(inCameraIP), m_UserAndPwd(inUserAndPwd), 
				m_InitialWidth(inWidth), m_InitialHeight(inHeight), m_InitialCompression(inCompression)
			{
				// Validate parameters
				if (!m_CameraIP.length() || !m_UserAndPwd.length())
					throw std::runtime_error("Enter Camera IP / UserName / Password");

				// Set camera settings string
				m_CameraSettingsString = CreateCameraSettingsString(inFPS, inCompression, inWidth, inHeight);

				// Start streaming (it will move this temp thread, destroying old, empty one)
				m_CurlWorker = std::thread(&AxisRawReader::CURL_BlockOpWorker, this);
			}

			// destructor - we stop, sync with running threads and clean up
			virtual ~AxisRawReader()
			{
				// Stop thread execution
				m_CustomBuff.m_stopStreaming = true;

				// Join
				if (m_CurlWorker.joinable())
					m_CurlWorker.join();

				// Release possible lock in popRawFrame() - can be called by other thread using our object
				m_CustomBuff.m_RawFrames.release();
			}

			// no copy and assign
			AxisRawReader(const AxisRawReader&) = delete;
			AxisRawReader& operator=(const AxisRawReader&) = delete;

			// no move
			AxisRawReader(AxisRawReader&&) = delete;
			AxisRawReader& operator=(AxisRawReader&&) = delete;

			//! Retrieves RAW frame (bytes)
			bool popRawFrame(RawMJPGFrame& retObj) override
			{
				// Move semantics
				return m_CustomBuff.m_RawFrames.pop(retObj);
			}

			size_t bufferSize() const override 
			{ 
				return m_CustomBuff.m_RawFrames.size(); 
			}

		private:

			//! My custom parameter class
			struct CustomBuffer
			{
				//! Default constructor for my custom buffer
				CustomBuffer() : m_stopStreaming(false)
				{
				}

				// Temporary buffer - we cut frames from here in myCURL_CustomWrite()
				std::string m_packetBuffer;

				//! Vector for all the collected frames
				utils::BlockingQueue<RawMJPGFrame> m_RawFrames;

				//! Global stop flag for my streaming task
				std::atomic<bool> m_stopStreaming;
			};

			//! Instance of my buffer
			CustomBuffer m_CustomBuff;

			//! IP address of the camera
			std::string m_CameraIP;

			//! Login for the camera in "user:pwd" format
			std::string m_UserAndPwd;

			//! String with camera settings
			std::string m_CameraSettingsString;

			//! Worker thread performing continous BLOCKING operation using CURL library
			std::thread m_CurlWorker;

			//! Initial width
			size_t m_InitialWidth;

			//! Initial height and compression
			size_t m_InitialHeight; 

			//! Initial compression
			size_t m_InitialCompression;

			//! Operation - Read MPJG
			static const std::string READ_MJPG_CGI;

			//! Frame boundary string - ONVIF Standard specific
			static const std::string ONVIF_FRAME_BOUNDARY;
			static const std::string ONVIF_FRAME_BOUNDARY_END;

			//! HTTP header ending - HTTP Protocol specific
			static const std::string HTTP_HEADER_END;

			//! Private - max FPS and compression (25 and 99% compression)
			static const size_t MAX_FPS = 25;
			static const size_t MAX_COMPRESSION = 99;

			//! Default FPS rate when connection was lost (crude approximation)
			static const size_t DEFAULT_FPS_ON_CONNECTION_LOSS = 25;

			//! Creates camera settings string (performs value check - if value is out of range, we apply defaults)
			static std::string CreateCameraSettingsString(size_t inFPS, size_t inCompression, size_t inWidth, size_t inHeight);

			//! Worker thread that starts blocking CURL operation - it ends when connection is interrupted or we forcefully end it
			void CURL_BlockOpWorker();

			//! Cuts frame from the string and returns it as a string (optimization makes code look ugly)
			static bool CutRawFrameFromString(std::string& stringToAlter, std::vector<unsigned char>& returnedFrame, const size_t lastStrToAlterSize);

			//! CURL Write function
			static size_t myCURL_CustomWrite(char *ptr, size_t size, size_t nmemb, CustomBuffer* customData);

			//! CURL blocking operation 
			bool myCURL_BlockingOperation(const std::string& inIPAddress, const std::string& inUserAndPwd, const std::string& inOperationSuffix);

		};

	} // camera

} // utils

#endif // _AXISRAWREADER_H