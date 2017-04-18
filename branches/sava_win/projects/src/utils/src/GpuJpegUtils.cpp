#include "GpuJpegUtils.h"

#include <libgpujpeg/gpujpeg.h>


//#pragma comment(lib, "libgpujpeg.lib")

/**
* JPEG decoder structure
*/
struct gpujpeg_decoder
{
	// JPEG coder structure
	struct gpujpeg_coder coder;

	// ...
};

namespace utils
{
	int GpuJpegLib::initDevice(int deviceId /*= 0*/)
{
		if (!ms_DeviceInited)
		{
			int ret = gpujpeg_init_device(deviceId, 0);
			if (ret)
				return ret;
			ms_DeviceInited = true;
		}
		return 0;
	}

	bool GpuJpegLib::ms_DeviceInited = false;

	GpuJpegEncoder::GpuJpegEncoder(int quality /*= 75*/) 
		: m_Quality(75)
		, m_Encoder(nullptr)
		, m_ImageWidth(-1)
		, m_ImageHeight(-1)
	{
	}

	GpuJpegEncoder::~GpuJpegEncoder()
	{
		destroyEncoder();
	}

	void GpuJpegEncoder::encode(const cv::Mat& inputImage, std::vector<unsigned char>& outputImage)
	{
		if (m_Encoder == nullptr || inputImage.cols != m_ImageWidth || inputImage.rows != m_ImageHeight)
		{
			m_ImageWidth = inputImage.cols;
			m_ImageHeight = inputImage.rows;
			createEncoder();
		}

		struct gpujpeg_encoder_input encoder_input;
		gpujpeg_encoder_input_set_image(&encoder_input, inputImage.data);

		uint8_t* image_compressed = NULL;
		int image_compressed_size = 0;
		if (gpujpeg_encoder_encode(m_Encoder, &encoder_input, &image_compressed, &image_compressed_size) != 0 && gpujpeg_encoder_encode(m_Encoder, &encoder_input, &image_compressed, &image_compressed_size) != 0)
			throw std::runtime_error("GpuJpegEncoder::encode(): Can't encode image.");

		outputImage.assign(image_compressed, image_compressed + image_compressed_size);

		//gpujpeg_image_destroy(image_compressed);
	}

	void GpuJpegEncoder::createEncoder()
	{
		destroyEncoder();

		struct gpujpeg_parameters param;
		gpujpeg_set_default_parameters(&param);
		gpujpeg_parameters_chroma_subsampling(&param);
		param.quality = m_Quality;

		struct gpujpeg_image_parameters param_image;
		gpujpeg_image_set_default_parameters(&param_image);
		param_image.width = m_ImageWidth;
		param_image.height = m_ImageHeight;

		GpuJpegLib::initDevice();

		m_Encoder = gpujpeg_encoder_create(&param, &param_image);
		if (m_Encoder == nullptr)
			throw std::runtime_error("GpuJpegEncoder::createEncoder(): Can't create encoder.");
	}

	void GpuJpegEncoder::destroyEncoder()
	{
		if (m_Encoder)
		{
			gpujpeg_encoder_destroy(m_Encoder);
			m_Encoder = nullptr;
		}
	}

	GpuJpegDecoder::GpuJpegDecoder()
		: m_Decoder(nullptr)
	{
		GpuJpegLib::initDevice();
	}

	GpuJpegDecoder::~GpuJpegDecoder()
	{
		destroyDecoder();
	}

	void GpuJpegDecoder::decode(const std::vector<unsigned char>& inputImage, cv::Mat& outputImage)
	{
		if (m_Decoder == nullptr)
			createDecoder();		

		struct gpujpeg_decoder_output decoder_output;
		gpujpeg_decoder_output_set_default(&decoder_output);
		if (gpujpeg_decoder_decode(m_Decoder, const_cast<uint8_t*>(inputImage.data()), static_cast<int>(inputImage.size()), &decoder_output) != 0)
			throw std::runtime_error("GpuJpegDecoder::decode(): Can't decode image.");

		int width = m_Decoder->coder.param_image.width;
		int height = m_Decoder->coder.param_image.height;

		outputImage = cv::Mat(height, width, CV_8UC3, decoder_output.data).clone();
	}

	void GpuJpegDecoder::createDecoder()
	{
		GpuJpegLib::initDevice();
		m_Decoder = gpujpeg_decoder_create();
		if (m_Decoder == nullptr)
			throw std::runtime_error("GpuJpegDecoder::createDecoder(): Can't create decoder.");
	}

	void GpuJpegDecoder::destroyDecoder()
	{
		if (m_Decoder)
		{
			gpujpeg_decoder_destroy(m_Decoder);
			m_Decoder = nullptr;
		}
	}

}