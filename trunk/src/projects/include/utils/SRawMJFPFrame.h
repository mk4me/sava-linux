//
//
//  @ Project : Milestone
//  @ File Name : SRawMJGPFrame.h
//  @ Date : 2016-01-28
//  @ Author : Kamil Lebek
//
//

#ifndef _SRAWMJPGFRAME_H
#define _SRAWMJPGFRAME_H

#include <vector>
#include <boost/date_time/posix_time/posix_time.hpp>

//! Global utils namespace
namespace utils
{
	//! Camera support namespace
	namespace camera
	{

		//! Structure for storing Raw MJPG frames (MOVABLE only) - designed for BlockingQueue.h usage
		struct SRawMJPGFrame
		{
			//! Default constructor
			SRawMJPGFrame()
			{
				// Create empty structure w/o raw bytes
				// ...
			}

			//! Argument constructor
			SRawMJPGFrame(std::vector<unsigned char>&& frameAsVector) : m_RawFrame(std::move(frameAsVector))
			{
				// Initialize time stamp
				m_TimeStamp = boost::posix_time::microsec_clock::local_time();
			}

			//! Move constructor
			SRawMJPGFrame(SRawMJPGFrame&& other) : m_RawFrame(std::move(other.m_RawFrame))
			{
				// Copy time stamp
				m_TimeStamp = other.m_TimeStamp;
			}

			//! Move assignment
			SRawMJPGFrame& operator=(SRawMJPGFrame&& other)
			{
				// Move
				m_RawFrame = std::move(other.m_RawFrame);

				// Copy
				m_TimeStamp = other.m_TimeStamp;

				// Return self
				return *this;
			}

			//! Raw frame data containing MJPG image
			std::vector<unsigned char> m_RawFrame;

			//! Time stamp (when frame was collected - using local time)
			boost::posix_time::ptime m_TimeStamp;

			// no copy and assign
			SRawMJPGFrame(const SRawMJPGFrame&) = delete;
			SRawMJPGFrame& operator=(const SRawMJPGFrame&) = delete;
		};

	} // camera

} // utils

#endif // _SRAWMJPGFRAME_H
