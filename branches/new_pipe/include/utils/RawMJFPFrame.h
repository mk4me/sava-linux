//
//
//  @ Project : Milestone
//  @ File Name : RawMJGPFrame.h
//  @ Date : 2016-01-28
//  @ Author : Kamil Lebek
//
//

#ifndef _RAWMJPGFRAME_H
#define _RAWMJPGFRAME_H

#include <vector>
#include <boost/date_time/posix_time/posix_time.hpp>

//! Global utils namespace
namespace utils
{
	//! Camera support namespace
	namespace camera
	{

		//! Structure for storing Raw MJPG frames (MOVABLE only) - designed for BlockingQueue.h usage
		struct MJPGFrame
		{
			//! Default constructor
			MJPGFrame()
			{
				// Create empty structure w/o raw bytes
				// ...
			}

			//! Argument constructor
			MJPGFrame(std::vector<unsigned char>&& frameAsVector) : m_JPEGFrame(std::move(frameAsVector))
			{
				// Initialize time stamp
				m_TimeStamp = boost::posix_time::microsec_clock::local_time();
			}

			//! Argument constructor (Milestone version)
			MJPGFrame(std::vector<unsigned char>&& frameAsVector, boost::posix_time::ptime inTimeStamp) :
				m_JPEGFrame(std::move(frameAsVector)), m_TimeStamp(inTimeStamp)
			{
				// Timestamp is provided
			}

			//! Move constructor
			MJPGFrame(MJPGFrame&& other) : m_JPEGFrame(std::move(other.m_JPEGFrame))
			{
				// Copy time stamp
				m_TimeStamp = other.m_TimeStamp;
			}

			//! Move assignment
			MJPGFrame& operator=(MJPGFrame&& other)
			{
				// Move
				m_JPEGFrame = std::move(other.m_JPEGFrame);

				// Copy
				m_TimeStamp = other.m_TimeStamp;

				// Return self
				return *this;
			}

			//! Raw frame data containing MJPG image
			std::vector<unsigned char> m_JPEGFrame;

			//! Time stamp (when frame was collected - using local time)
			boost::posix_time::ptime m_TimeStamp;

			// no copy and assign
			MJPGFrame(const MJPGFrame&) = delete;
			MJPGFrame& operator=(const MJPGFrame&) = delete;
		};

	} // camera

} // utils

#endif // _RAWMJPGFRAME_H
