//
//
//  @ Project : Milestone
//  @ File Name : MilestoneRawReader.h
//  @ Date : 2016-03-04
//  @ Author : Kamil Lebek
//
//

#ifndef _MILESTONERAWREADER_H
#define _MILESTONERAWREADER_H

// MACROS
#define __MS_STR2__(x) #x
#define __MS_STR1__(x) __MS_STR2__(x)
#define __MS_LOC__ __FILE__ "("__MS_STR1__(__LINE__)") : warning: "

//
// Milestone Toolkit Factory
//

// Lib (Milestone Toolkit Factory is shipped in Relase, so no difference)
#ifdef _DEBUG
#pragma comment(lib, "ToolkitFactoryProvider.lib")
#else
#pragma comment(lib, "ToolkitFactoryProvider.lib")
#endif

// Includes
#include <string>
#include <atomic>
#include <thread>

// Common interface definition
#include "ICameraRawReader.h"

// Milestone credentials
#include "MilestoneStructures.h"

// Blocking Queue and Raw Frame
#include "BlockingQueue.h"
#include "RawMJFPFrame.h"

//! Global utils namespace
namespace utils
{
	//! Camera support namespace
	namespace camera
	{
		//! Class providing RAW MJPG access to the Milestone Camera (frames as MJPG raw bytes)
		class MilestoneRawReader : public ICameraRawReader
		{
		public:

			//! Instantiates Milestone MJPG reader
			/*!
				\param inServerCredentials Milestone server credentials (IP, basic auth user name and password)
				\param inCameraGUID camera-specific GUID - must be obtained from Milestone using e.g. CCameraEnumerator
				\param inFPS desired frame rate (range: 1 to 25) - 0 means maximum rate (default setting)
				\param inCompression compression rate from 1 (lowest) to 99 (highest) - 30 is default setting
				\param inWidth frame width - default is 1920
				\param inHeight frame height - default is 1080
			*/
			explicit MilestoneRawReader(const MilestoneCredentials& inServerCredentials, const std::string& inCameraGUID,
				const size_t inFPS = DEFAULT_FPS, const size_t inCompression = DEFAULT_COMPRESSION,
				const size_t inWidth = DEFAULT_WIDTH, const size_t inHeight = DEFAULT_HEIGHT) :
				m_ServerCredentials(inServerCredentials), m_CameraGUID(inCameraGUID), m_stopStreaming(false)
			{
				// Validate camera's GUID (pass random, invalid GUID)
				if (m_CameraGUID.length() <= 0)
					m_CameraGUID = "d3fba69e-fa92-4347-acd2-e216dcf35682";

				// Set camera settings
				ParseCameraSettings(inFPS, inCompression, inWidth, inHeight);

				// Start streaming (it will move this temp thread, destroying old, empty one)
				m_MilestoneWorker = std::thread(&MilestoneRawReader::milestone_StreamWorker, this);
			}

			//! Simple destructor
			virtual ~MilestoneRawReader()
			{
				// Stop worker thread using cond_var
				m_stopStreaming = true;

				// Join
				if (m_MilestoneWorker.joinable())
					m_MilestoneWorker.join();

				// Release thread-specific resources
				// ...
			}

			// no copy and assign
			MilestoneRawReader(const MilestoneRawReader&) = delete;
			MilestoneRawReader& operator=(const MilestoneRawReader&) = delete;

			// no move
			MilestoneRawReader(MilestoneRawReader&&) = delete;
			MilestoneRawReader& operator=(MilestoneRawReader&&) = delete;

			//! Retrieves RAW frame (bytes)
			bool popRawFrame(RawMJPGFrame& retObj) override
			{
				// Move semantics
				return m_RawFrames.pop(retObj);
			}

			//! Returns queue size
			size_t bufferSize() const override
			{ 
				return m_RawFrames.size(); 
			}

		private:
			//! Default milestone port (video media server)
			static const short MILESTONE_VMS_PORT = 80;

			//! Milestone server credentials
			MilestoneCredentials m_ServerCredentials;

			//! GUID for the chosen camera
			std::string m_CameraGUID;

			//! Desired stream FPS rate
			size_t m_FPS;

			//! Desired stream quality (100 - compression)
			size_t m_Quality;

			//! Desired image width
			size_t m_Width;

			//! Desired image height
			size_t m_Height;

			//! Worker thread performing continous BLOCKING operation using Milestone Toolkit library
			std::thread m_MilestoneWorker;

			//! Global stop flag for my streaming task
			std::atomic<bool> m_stopStreaming;

			//! Vector for all the collected frames
			utils::BlockingQueue<RawMJPGFrame> m_RawFrames;

			//! Milestone streaming worker function (can be interrupted - will wait for blocking operation to finish)
			void milestone_StreamWorker();

			//! Main thread function (performs BLOCKING stream operation - can be terminated with m_stopStreaming flag)
			bool milestone_StreamFunc();

			//! Parses camera settings (performs value check - if value is out of range, we apply defaults)
			void ParseCameraSettings(size_t inFPS, size_t inCompression, size_t inWidth, size_t inHeight);

			//! Private - max FPS and compression (25 and 99% compression)
			static const size_t MAX_FPS = 25;
			static const size_t MAX_COMPRESSION = 99;

		}; // MilestoneRawReader

	} // camera

} // utils

#endif // _MILESTONERAWREADER_H