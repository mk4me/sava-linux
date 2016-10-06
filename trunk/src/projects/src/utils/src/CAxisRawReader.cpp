//
//
//  @ Project : Milestone
//  @ File Name : CAxisRawReader.cpp
//  @ Date : 2016-01-28
//  @ Author : Kamil Lebek
//
//

#include "CAxisRawReader.h"

//
// WARNING: It requires also LIB libcurl_imp.lib in Linker Input
//
#include <curl/curl.h>

//! Global utils namespace
namespace utils
{
	//! Camera support namespace
	namespace camera
	{

		//
		// Class configuration
		//

		// CURL specific
		const std::string CAxisRawReader::READ_MJPG_CGI = "/axis-cgi/mjpg/video.cgi";
		const std::string CAxisRawReader::ONVIF_FRAME_BOUNDARY = "--myboundary";
		const std::string CAxisRawReader::ONVIF_FRAME_BOUNDARY_END = "\r\n--myboundary";
		const std::string CAxisRawReader::HTTP_HEADER_END = "\r\n\r\n";

		//
		// Class implementation
		//

		//! Creates camera settings string (performs value check - if value is out of range, we apply defaults)
		std::string CAxisRawReader::CreateCameraSettingsString(size_t inFPS, size_t inCompression, size_t inWidth, size_t inHeight)
		{
			// Resolution - Width (1 .. MAX)
			if ((inWidth > DEFAULT_WIDTH) || (!inWidth))
				inWidth = DEFAULT_WIDTH;

			// Resolution - Height (1 .. MAX)
			if ((inHeight > DEFAULT_HEIGHT) || (!inHeight))
				inHeight = DEFAULT_HEIGHT;

			// Compression (1 .. MAX)
			if ((inCompression < 1) || (inCompression > MAX_COMPRESSION))
				inCompression = DEFAULT_COMPRESSION;

			// FPS (0 - Unlimited, 1 .. MAX)
			if (inFPS > MAX_FPS)
				inFPS = MAX_FPS;

			// Concatenate the string (e.g. "resolution=1920x1080&fps=0&compression=20")
			std::string ret = "resolution=" + std::to_string(inWidth) + "x" + std::to_string(inHeight)
				+ "&fps=" + std::to_string(inFPS) + "&compression=" + std::to_string(inCompression);
			return ret;
		}

		//! Worker thread that starts blocking CURL operation - it ends when connection is interrupted or we forcefully end it
		void CAxisRawReader::CURL_BlockOpWorker()
		{
			// Create request link from Camera Settings and default READ operation cgi link
			std::string axisCGIOp = READ_MJPG_CGI + std::string("?") + m_CameraSettingsString;

			// Invoke BLOCKING operation till we get proper termination flag
			while (m_CustomBuff.m_stopStreaming == false)
			{
				myCURL_BlockingOperation(m_CameraIP, m_UserAndPwd, axisCGIOp);
				
				// Inform about every reconnect
				std::cerr << "Reconnecting to camera (IP: " << m_CameraIP << ")..." << std::endl;
			}
		}

		//! Cuts frame from the string and returns it as a string (optimization makes code look ugly)
		bool CAxisRawReader::CutRawFrameFromString(std::string& stringToAlter, std::vector<unsigned char>& returnedFrame, const size_t lastStrToAlterSize)
		{
			// Find frame begging
			size_t jpgBeg = stringToAlter.find(ONVIF_FRAME_BOUNDARY);
			if (jpgBeg == std::string::npos)
				return false;

			// Find header begging (it's in the frame begging)
			size_t hdrBeg = stringToAlter.find(HTTP_HEADER_END, jpgBeg + ONVIF_FRAME_BOUNDARY.length());		// We skip begging tag

			// Find frame end
			// OPTIMIZATION: to avoid searching for end marker in every frame while we collect the data, we just check the last chunk of data
			// with look-ahead equal to the size of the string we are looking for.
			size_t bestOffsetToStart = lastStrToAlterSize - ONVIF_FRAME_BOUNDARY_END.length(); // We start where we finished last time minus look-ahead
			if (bestOffsetToStart < jpgBeg)
				bestOffsetToStart = jpgBeg + ONVIF_FRAME_BOUNDARY.length(); // We start after frame begging marker
			size_t jpgEnd = stringToAlter.find(ONVIF_FRAME_BOUNDARY_END, bestOffsetToStart);	// We skip all data we checked before

			// Check if we found a frame or should we accumulate more packets?
			if ((jpgEnd == std::string::npos) || (hdrBeg == std::string::npos))
				return false;

			// Let's cut it
			size_t cutStart = hdrBeg + HTTP_HEADER_END.length(); // Begging of HTTP header end sequence + it's length
			size_t cutLength = jpgEnd - cutStart; // End marker - cutStarting point

			// Just copy memory
			returnedFrame.resize(cutLength);
			std::copy(stringToAlter.c_str() + cutStart, stringToAlter.c_str() + cutStart + cutLength, returnedFrame.begin());

			// Remove frame from the working buffer
			stringToAlter.erase(0, jpgEnd);
			return true;
		}

		//! CURL Write function
		size_t CAxisRawReader::myCURL_CustomWrite(char *ptr, size_t size, size_t nmemb, SCustomBuffer* customData)
		{
			// Actual bytes to write
			size_t toWrite = size * nmemb;

			// Append data to my custom buffer
			size_t sizeBeforeAppend = customData->m_packetBuffer.size();
			customData->m_packetBuffer.append(ptr, toWrite);

			// Cut frames from the buffer
			std::vector<unsigned char> frameAsVector;

			// Valid frame was found
			if (CutRawFrameFromString(customData->m_packetBuffer, frameAsVector, sizeBeforeAppend))
				customData->m_RawFrames.push(SRawMJPGFrame(std::move(frameAsVector)));

			// Regular operation (different value will stop the transfer)
			if (customData->m_stopStreaming == false)
				return toWrite;
			// Otherwise - terminate the operation
			else
				return -1;
		}

		//! CURL blocking operation 
		bool CAxisRawReader::myCURL_BlockingOperation(const std::string& inIPAddress, const std::string& inUserAndPwd, const std::string& inOperationSuffix)
		{
			// Init CURL
			CURL* myCurl = curl_easy_init();

			// Clear my buffer - the rest of the structure contains valid data (handling reconnection)
			m_CustomBuff.m_packetBuffer.clear();

			// Create final request url
			std::string sUrl = inIPAddress + inOperationSuffix;

			curl_easy_setopt(myCurl, CURLOPT_URL, sUrl.c_str());
			curl_easy_setopt(myCurl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
			curl_easy_setopt(myCurl, CURLOPT_USERPWD, inUserAndPwd.c_str());

			struct curl_slist *slist = NULL;
			slist = curl_slist_append(slist, "Accept: application/x-ms-application, image/jpeg, application/xaml+xml, image/gif, image/pjpeg, application/x-ms-xbap, */*");
			slist = curl_slist_append(slist, "Accept-Language: pl-PL");
			slist = curl_slist_append(slist, "User-Agent: libcurl-agent/1.0");
			slist = curl_slist_append(slist, "Accept-Encoding: gzip, deflate");
			slist = curl_slist_append(slist, "Connection: Keep-Alive");

			curl_easy_setopt(myCurl, CURLOPT_HTTPHEADER, slist);
			curl_easy_setopt(myCurl, CURLOPT_HEADER, 0);

			curl_easy_setopt(myCurl, CURLOPT_WRITEFUNCTION, myCURL_CustomWrite);
			curl_easy_setopt(myCurl, CURLOPT_WRITEDATA, &m_CustomBuff);

			try
			{
				// Blocking operation that can be aborted in callback
				CURLcode opResult = curl_easy_perform(myCurl);
				if (opResult != CURLE_OK)
				{
					// We didn't establish connection - thread will end anyway
					return false;
				}

			}
			catch (...)
			{
				// Exception? That's wrong.
				return false;
			}

			// Just clean shutdown
			curl_easy_cleanup(myCurl);
			return true;
		}

	} // camera

} // utils