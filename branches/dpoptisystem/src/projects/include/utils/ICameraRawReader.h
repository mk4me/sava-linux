//
//
//  @ Project : Milestone
//  @ File Name : ICameraRawReader.h
//  @ Date : 2016-03-22
//  @ Author : Kamil Lebek
//
//

#ifndef _ICAMERARAWREADER_H
#define _ICAMERARAWREADER_H

// Raw Frame
#include "RawMJFPFrame.h"

//! Global utils namespace
namespace utils
{
	//! Camera support namespace
	namespace camera
	{

		//! Common inteface for classes providing RAW MJPG access to the cameras (frames as MJPG raw bytes)
		class ICameraRawReader
		{
		public:
			//! Pure virtual: polymorphic interface destructor
			virtual ~ICameraRawReader()	{}

			//! Pure virtual: Retrieves RAW frame (bytes)
			virtual bool popRawFrame(RawMJPGFrame& retObj) = 0;

			//! Pure virtual: Returns queue size
			virtual size_t bufferSize() const = 0;

			//! Default camera settings - FPS (Unlimited)
			static const size_t DEFAULT_FPS = 0;

			//! Default camera settings - Compression (30%)
			static const size_t DEFAULT_COMPRESSION = 30;

			//! Default camera settings - Width (1920)
			static const size_t DEFAULT_WIDTH = 1920;

			//! Default camera settings - Height (1080)
			static const size_t DEFAULT_HEIGHT = 1080;

		}; // ICameraRawReader

	} // camera

} // utils

#endif // _ICAMERARAWREADER_H