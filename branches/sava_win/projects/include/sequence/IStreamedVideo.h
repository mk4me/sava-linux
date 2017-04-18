#pragma once
#ifndef IStreamedVideo_h__
#define IStreamedVideo_h__

#include <opencv/cv.h>

#include <iostream>
#include <string>
#include <memory>

namespace sequence
{
	/// <summary>
	/// interfejs odczytu dowolnego formatu wideo w projekcie oraz najpopularniejszych formatów globalnych.
	/// </summary>
	class IStreamedVideo
	{
	public:
		static std::shared_ptr<IStreamedVideo> create(const std::string& filename);

		virtual ~IStreamedVideo() { }

		/// <summary>
		/// Gets the number of frames.
		/// </summary>
		/// <returns>Number of frames in video</returns>
		virtual size_t getNumFrames() const = 0;

		/// <summary>
		/// Gets the next frame.
		/// </summary>
		/// <param name="frame">Frame to be written to.</param>
		/// <returns>False if end of video or error, otherwise true</returns>
		virtual bool getNextFrame(cv::Mat& frame) = 0;

		/// <summary>
		/// Sets video position to given frame number.
		/// </summary>
		/// <param name="frame">The frame position.</param>
		virtual void setPosition(size_t frame) = 0;

		/// <summary>
		/// Gets the next frame or empty matrix if end of video.
		/// </summary>
		IStreamedVideo& operator >> (cv::Mat& frame);

	private:

	};
}

std::shared_ptr<sequence::IStreamedVideo> operator >> (std::shared_ptr<sequence::IStreamedVideo> video, cv::Mat& frame);


#endif // IStreamedVideo_h__
