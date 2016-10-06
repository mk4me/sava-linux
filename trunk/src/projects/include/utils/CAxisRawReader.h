//
//
//  @ Project : Milestone
//  @ File Name : CAxisRawReader.h
//  @ Date : 2016-01-28
//  @ Author : Kamil Lebek
//
//


#ifndef _CAXISRAWREADER_H
#define _CAXISRAWREADER_H

#include <string>
#include <thread>
#include <atomic>

#include "BlockingQueue.h"
#include "SRawMJFPFrame.h"

//! Global utils namespace
namespace utils
{
	//! Camera support namespace
	namespace camera
	{

		//! Class providing RAW MJPG access to the Axis Camera (frames as MJPG raw bytes)
		class CAxisRawReader
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
			explicit CAxisRawReader(const std::string& inCameraIP, const std::string& inUserAndPwd,
				const size_t inFPS = DEFAULT_FPS, const size_t inCompression = DEFAULT_COMPRESSION,
				const size_t inWidth = DEFAULT_WIDTH, const size_t inHeight = DEFAULT_HEIGHT)
				: m_CameraIP(inCameraIP), m_UserAndPwd(inUserAndPwd)
			{
				// Validate parameters
				if (!m_CameraIP.length() || !m_UserAndPwd.length())
					throw std::runtime_error("Enter Camera IP / UserName / Password");

				// Set camera settings string
				m_CameraSettingsString = CreateCameraSettingsString(inFPS, inCompression, inWidth, inHeight);

				// Start streaming (it will move this temp thread, destroying old, empty one)
				m_CurlWorker = std::thread(&CAxisRawReader::CURL_BlockOpWorker, this);
			}

			// destructor - we stop, sync with running threads and clean up
			virtual ~CAxisRawReader()
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
			CAxisRawReader(const CAxisRawReader&) = delete;
			CAxisRawReader& operator=(const CAxisRawReader&) = delete;

			// no move
			CAxisRawReader(CAxisRawReader&&) = delete;
			CAxisRawReader& operator=(CAxisRawReader&&) = delete;

			//! Retrieves RAW frame (bytes)
			bool popRawFrame(SRawMJPGFrame& retObj)
			{
				// Move semantics
				return m_CustomBuff.m_RawFrames.pop(retObj);
			}

			size_t bufferSize() const { return m_CustomBuff.m_RawFrames.size(); }

			//! Default camera settings - FPS (Unlimited)
			static const size_t DEFAULT_FPS = 0;

			//! Default camera settings - Compression (30%)
			static const size_t DEFAULT_COMPRESSION = 30;

			//! Default camera settings - Width (1920)
			static const size_t DEFAULT_WIDTH = 1920;

			//! Default camera settings - Height (1080)
			static const size_t DEFAULT_HEIGHT = 1080;

		private:

			//! My custom parameter class
			struct SCustomBuffer
			{
				//! Default constructor for my custom buffer
				SCustomBuffer() : m_stopStreaming(false)
				{
				}

				// Temporary buffer - we cut frames from here in myCURL_CustomWrite()
				std::string m_packetBuffer;

				//! Vector for all the collected frames
				utils::BlockingQueue<SRawMJPGFrame> m_RawFrames;

				//! Global stop flag for my streaming task
				std::atomic<bool> m_stopStreaming;
			};

			//! Instance of my buffer
			SCustomBuffer m_CustomBuff;

			//! IP address of the camera
			std::string m_CameraIP;

			//! Login for the camera in "user:pwd" format
			std::string m_UserAndPwd;

			//! String with camera settings
			std::string m_CameraSettingsString;

			//! Worker thread performing continous BLOCKING operation using CURL library
			std::thread m_CurlWorker;

			//! Operation - Read MPJG
			static const std::string READ_MJPG_CGI;

			//! Frame boundary string - ONVIF Standard specific
			static const std::string ONVIF_FRAME_BOUNDARY;
			static const std::string ONVIF_FRAME_BOUNDARY_END;

			//! HTTP header ending - HTTP Protocol specific
			static const std::string CAxisRawReader::HTTP_HEADER_END;

			//! Private - max FPS and compression (25 and 99% compression)
			static const size_t MAX_FPS = 25;
			static const size_t MAX_COMPRESSION = 99;

			//! Creates camera settings string (performs value check - if value is out of range, we apply defaults)
			static std::string CreateCameraSettingsString(size_t inFPS, size_t inCompression, size_t inWidth, size_t inHeight);

			//! Worker thread that starts blocking CURL operation - it ends when connection is interrupted or we forcefully end it
			void CURL_BlockOpWorker();

			//! Cuts frame from the string and returns it as a string (optimization makes code look ugly)
			static bool CutRawFrameFromString(std::string& stringToAlter, std::vector<unsigned char>& returnedFrame, const size_t lastStrToAlterSize);

			//! CURL Write function
			static size_t myCURL_CustomWrite(char *ptr, size_t size, size_t nmemb, SCustomBuffer* customData);

			//! CURL blocking operation 
			bool myCURL_BlockingOperation(const std::string& inIPAddress, const std::string& inUserAndPwd, const std::string& inOperationSuffix);

		};

	} // camera

} // utils

#endif // _CAXISRAWREADER_H