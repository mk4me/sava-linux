//
//
//  @ Project : Milestone
//  @ File Name : AlertSender.h
//  @ Date : 2016-03-21
//  @ Author : Kamil Lebek
//
//


#ifndef _ALERTSENDER_H
#define _ALERTSENDER_H

#include "MilestoneStructures.h"

// For posix_time
#include <boost/date_time/posix_time/posix_time.hpp>
// For CvRect
#include <opencv2/opencv.hpp>

//! Global utils namespace
namespace utils
{
	//! Camera support namespace
	namespace camera
	{
		
		//! Camera configuration retriever
		class AlertSender
		{
		public:
			//! Instantiates Milestone Alert sender
			/*!
				\param inServerCredentials Milestone server credentials (IP, basic auth user name and password)
				\param inCameraGUID camera-specific GUID - must be obtained from Milestone using e.g. CCameraEnumerator
			*/
			explicit AlertSender(const MilestoneCredentials& inServerCredentials, const std::string& inCameraGUID) : 
				m_ServerCredentials(inServerCredentials), m_CameraGUID(inCameraGUID) 
			{
				// Validate camera's GUID (pass random, invalid GUID)
				if (m_CameraGUID.length() <= 0)
					m_CameraGUID = "d3fba69e-fa92-4347-acd2-e216dcf35682";
			}

			//! Simple destructor
			virtual ~AlertSender() { }

			//! Sends alert to the camera
			/*!
				\param inName name of defined Analytics Event (e.g. "ManEnter", "ManLeave")
				\param inTime time of the alert (must be the same as the server time - read it from SRawMJPGFrame)
				\param inDescription detailed description of the alert e.g. "ACTION detected by PJATK IMS"
				\param inAreaOfInterest alert area (in normalized coordinates 0.0 - 1.0) - x, y, width, height
				\param inLocation place, where event took place
				\param inBoxColor area of interest color (BGR - alpha is ignored - forced 1.0) - default: blue
				\return False if query failed (wrong credentials, wrong camrea, server offline, etc.).
			*/
			bool SendAlert(const std::string& inName, const boost::posix_time::ptime& inTime, const std::string inDescription, const cv::Rect2d& inAreaOfInterest, 
				const std::string& inLocation, const cv::Scalar& inBoxColor = cv::Scalar(1.0f, 0.0f, 0.0f));

			//! Returns last error send from server
			/*!
				\return Detailed error description.
			*/
			std::string GetLastErrorDesc()
			{
				return m_LastErrorDescription;
			}

		private:
			//! Milestone server credentials
			MilestoneCredentials m_ServerCredentials;

			//! GUID for the chosen camera
			std::string m_CameraGUID;

			//! Last error message
			std::string m_LastErrorDescription;

			//! Default analytics event port
			static const std::string MS_DEFAULT_EVENT_PORT_STRING;

			//! Tags stripped from strings passed to Milestone
			static const std::string MS_TAGS_TO_STRIP;

			//! Polish diacritical symbols (source)
			static const std::wstring MS_POLISH_SYMBOLS;

			//! Polish symbols (replacers)
			static const std::wstring MS_POLISH_REPLACERS;

			//! Line end character string
			static const std::string MS_LINE_END;
			
			//! Line break html string
			static const std::string MS_LINE_HTML_BREAK;

			//! Precision conversion
			template <typename T>
			static std::string to_string_with_precision(const T a_value, const int n = 3)
			{
				std::ostringstream out;
				out << std::setprecision(n) << a_value;
				return out.str();
			}

			//! Clamping to uniform value (0.0 to 1.0)
			template <typename T>
			static T clamp_to_uniform(const T inValue)
			{
				T clampedVal = std::max((T)0.0, std::min((T)1.0, inValue));
				return clampedVal;
			}

			//! Color conversion
			template <typename T>
			static unsigned char floating_to_color(const T inValue)
			{
				T clampedVal = clamp_to_uniform(inValue);
				unsigned char retV = (unsigned char)floor((clampedVal == (T)1.0) ? 255 : clampedVal * (T)256.0);
				return retV;
			}

			//! Strips string from HTML tags (most of them)
			static std::string StripHTMLTags(const std::string& inStr);

			//! Strips tags and uses default value for empty strings
			static std::string StripAndDefault(const std::string& inStr, const std::string& inDefaultValue);

			//! Subsitutes diacritical symbols (e.g. polish characters)
			static std::string SubstituteSpecialSymbols(const std::string& inStr, const std::wstring& charsToFind, const std::wstring& charsToReplaceWith);

			//! Validates Box dimensions (it should be always on screen - clamps if necessary)
			static cv::Rect2d ValidateAndDefaultAOI(const cv::Rect2d& inAreaOfInterest);

			//! Validates color (overwrites alpha)
			static cv::Scalar ValidateAndDefaultColor(const cv::Scalar& inBoxColor);

			//! Converts line breaks ('\n') to "</br>
			static std::string ConvertLineBreaks(const std::string& inStr);

			//! Generates XML string for the query - assumming all parameters are ok
			static std::string GenerateXMLQuery(const std::string& inName, const boost::posix_time::ptime& inTime, const std::string inDescription, 
				const cv::Rect2d& inAreaOfInterest, const std::string& inCameraGUID,
				const std::string& inLocation, const cv::Scalar& inBoxColor);

		}; // CAlertSender

	} // camera

} // utils

#endif // _ALERTSENDER_H