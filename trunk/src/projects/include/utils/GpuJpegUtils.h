#pragma once
#ifndef GpuJpegUtils_h__
#define GpuJpegUtils_h__

#include <opencv2/core.hpp>

#include <vector>

struct gpujpeg_encoder;
struct gpujpeg_decoder;

namespace utils
{
	/// <summary>
	/// klasa zarzadzajaca bibliteka LibGpuJpeg.
	/// </summary>
	class GpuJpegLib
	{
	public:
		static int initDevice(int deviceId = 0);
	private:
		static bool ms_DeviceInited;
	};

	/// <summary>
	/// klasa do kodowania obrazów JPEG za pomoc¹ biblioteki LibGpuJpeg.
	/// </summary>
	class GpuJpegEncoder
	{
	public:
		GpuJpegEncoder(int quality = 75);
		~GpuJpegEncoder();

		void encode(const cv::Mat& inputImage, std::vector<unsigned char>& outputImage);

	private:
		void createEncoder();
		void destroyEncoder();

		gpujpeg_encoder* m_Encoder;
		int m_ImageWidth;
		int m_ImageHeight;
		int m_Quality;
	};

	/// <summary>
	/// klasa do dekodowania obrazów JPEG za pomoc¹ biblioteki LibGpuJpeg.
	/// </summary>
	class GpuJpegDecoder
	{
	public:
		GpuJpegDecoder();
		~GpuJpegDecoder();

		void decode(const std::vector<unsigned char>& inputImage, cv::Mat& outputImage);

	private:
		void createDecoder();
		void destroyDecoder();

		gpujpeg_decoder* m_Decoder;
	};
}

#endif // GpuJpegUtils_h__