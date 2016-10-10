//
//
//  @ Project : Milestone
//  @ File Name : AlertSender.cpp
//  @ Date : 2016-03-21
//  @ Author : Kamil Lebek
//
//

#include "AlertSender.h"

// For blocking HTTP/POST
#include <utils/WebUtils.h>

// For boost::remove_if
#include <boost/range/algorithm/remove_if.hpp>

// For local_time_adjustor
#include <boost/date_time/c_local_time_adjustor.hpp>

// Conversion and removal of special chars (QString)
#include <QtCore/QString>
#include <codecvt>

//! Global utils namespace
namespace utils
{
	//! Camera support namespace
	namespace camera
	{
		//
		// Class configuration
		//

		//! Default analytics event port
		const std::string AlertSender::MS_DEFAULT_EVENT_PORT_STRING		= ":9090";
		//! Tags stripped from strings passed to Milestone
		const std::string AlertSender::MS_TAGS_TO_STRIP					= "</>\"\'";
		//! Polish diacritical symbols (source)
		const std::wstring AlertSender::MS_POLISH_SYMBOLS				= L"πÍúÊ≥Ûøü• å∆£”Øè";
		//! Polish symbols (replacers)
		const std::wstring AlertSender::MS_POLISH_REPLACERS				= L"aesclozzAESCLOZZ";
		//! Line end character string
		const std::string AlertSender::MS_LINE_END						= "\n";
		//! Line break html string
		const std::string AlertSender::MS_LINE_HTML_BREAK				= "&#xA;";

		//! Internal stuff
		namespace internal
		{

			//! Retrieves UTC offset as time difference
			static boost::posix_time::time_duration GetUtcOffset()
			{
				// boost::date_time::c_local_adjustor uses the C-API to adjust a
				// moment given in utc to the same moment in the local time zone.
				const boost::posix_time::ptime utc_now = boost::posix_time::second_clock::universal_time();
				const boost::posix_time::ptime local_now = boost::date_time::c_local_adjustor<boost::posix_time::ptime>::utc_to_local(utc_now);

				return local_now - utc_now;
			}

			//! Retrieves UTC offset as string e.g. "+1:00"
			static std::string GetUtcOffsetString()
			{
				std::stringstream outStr;

				boost::posix_time::time_facet* tf = new boost::posix_time::time_facet(); // WARNING: stringstream gets ownership
				tf->time_duration_format("%+%H:%M");
				outStr.imbue(std::locale(outStr.getloc(), tf));

				outStr << GetUtcOffset();

				return outStr.str();
			}

			//! Generates proper string timestamp for Milestone XML query
			static std::string PosixTimeToStr(const boost::posix_time::ptime& inTime)
			{
				// Formatting codes taken from Milestone sample
				// "$timestamp$" == ("yyyy-MM-ddTHH:mm:sszzz")
				//<Timestamp>2016-03-21T13:28:13+01:00</Timestamp>

				// Create string in iso_extended format with timezone offset
				std::string timeZoneStr = GetUtcOffsetString();
				std::string retStr = boost::posix_time::to_iso_extended_string(inTime);
				return retStr + timeZoneStr;
			}
		}

		//
		// Class implementation
		//

		//! Generates XML string for the query - assuming all parameters are ok
		std::string AlertSender::GenerateXMLQuery(const std::string& inName, const boost::posix_time::ptime& inTime, 
			const std::string inDescription, const cv::Rect2d& inAreaOfInterest, const std::string& inCameraGUID, 
			const std::string& inLocation, const cv::Scalar& inBoxColor)
		{
			// Get proper timestamp as a string
			std::string sTimestamp = internal::PosixTimeToStr(inTime);

			// Precision delta
			const double bb_delta = 0.001;

			// Get box corners (Real)
			std::string X_minR = to_string_with_precision(inAreaOfInterest.x);
			std::string X_maxR = to_string_with_precision(inAreaOfInterest.x + inAreaOfInterest.width);
			std::string Y_minR = to_string_with_precision(inAreaOfInterest.y);
			std::string Y_maxR = to_string_with_precision(inAreaOfInterest.y + inAreaOfInterest.height);

			// Get box corners (Inside)
			std::string X_minI = to_string_with_precision(inAreaOfInterest.x + bb_delta);
			std::string X_maxI = to_string_with_precision(inAreaOfInterest.x + inAreaOfInterest.width - bb_delta);
			std::string Y_minI = to_string_with_precision(inAreaOfInterest.y + bb_delta);
			std::string Y_maxI = to_string_with_precision(inAreaOfInterest.y + inAreaOfInterest.height - bb_delta);

			// Get box corners (Outside)
			std::string X_minO = to_string_with_precision(inAreaOfInterest.x - bb_delta);
			std::string X_maxO = to_string_with_precision(inAreaOfInterest.x + inAreaOfInterest.width + bb_delta);
			std::string Y_minO = to_string_with_precision(inAreaOfInterest.y - bb_delta);
			std::string Y_maxO = to_string_with_precision(inAreaOfInterest.y + inAreaOfInterest.height + bb_delta);

			// Get colors
			unsigned char blueC = floating_to_color(inBoxColor[0]);
			unsigned char greenC = floating_to_color(inBoxColor[1]);
			unsigned char redC = floating_to_color(inBoxColor[2]);

			// Just format XML string
			std::string xmlString =
				"<AnalyticsEvent xmlns:i=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns=\"urn:milestone-systems\">"
				"	<EventHeader>"
				"		<ID>00000000-0000-0000-0000-000000000000</ID>"
				"		<Timestamp>" + sTimestamp + "</Timestamp>"
				"		<Type>IMS_Event</Type>"
				"		<!-- Insert Event Message here -->"
				"		<Message>" + inName + "</Message>"
				"		<CustomTag>Analyzed By PJATK IMS</CustomTag>"
				"		<Source>"
				"			<FQID>"
				"				<!-- Insert camera GUID here, if you have it -->"
				"				<ObjectId>" + inCameraGUID + "</ObjectId>"
				"			</FQID>"
				"		</Source>"
				"	</EventHeader>"
				"	<Description>"
				"		" + inDescription + " "
				"	</Description>"
				"	<Location>"
				"		" + inLocation + " "
				"	</Location>"
				"	<!-- Overlay data. -->"
				"	<ObjectList>"
				"		<Object>"
				"			<Name>Rectangle</Name>"
				"			<Value>Detected action</Value>"
				"			<AlarmTrigger>true</AlarmTrigger>"
				"			<Polygon>"
				"				<Color>"
				"					<A>255</A>"
				"					<R>" + std::to_string(redC) + "</R>"
				"					<G>" + std::to_string(greenC) + "</G>"
				"					<B>" + std::to_string(blueC) + "</B>"
				"				</Color>"
				"				<PointList>"
				"					<Point><X>" + X_minO + "</X><Y>" + Y_minO + "</Y></Point>" // O0
				"					<Point><X>" + X_maxO + "</X><Y>" + Y_minO + "</Y></Point>" // O1
				"					<Point><X>" + X_maxO + "</X><Y>" + Y_maxO + "</Y></Point>" // O2
				"					<Point><X>" + X_minO + "</X><Y>" + Y_maxO + "</Y></Point>" // O3
				"					<Point><X>" + X_minO + "</X><Y>" + Y_minO + "</Y></Point>" // O0
				"					<Point><X>" + X_minR + "</X><Y>" + Y_minR + "</Y></Point>" // R0
				"					<Point><X>" + X_maxR + "</X><Y>" + Y_minR + "</Y></Point>" // R1
				"					<Point><X>" + X_maxR + "</X><Y>" + Y_maxR + "</Y></Point>" // R2
				"					<Point><X>" + X_minR + "</X><Y>" + Y_maxR + "</Y></Point>" // R3
				"					<Point><X>" + X_minR + "</X><Y>" + Y_minR + "</Y></Point>" // R0
				"					<Point><X>" + X_minI + "</X><Y>" + Y_minI + "</Y></Point>" // I0
				"					<Point><X>" + X_maxI + "</X><Y>" + Y_minI + "</Y></Point>" // I1
				"					<Point><X>" + X_maxI + "</X><Y>" + Y_maxI + "</Y></Point>" // I2
				"					<Point><X>" + X_minI + "</X><Y>" + Y_maxI + "</Y></Point>" // I3
				"					<Point><X>" + X_minI + "</X><Y>" + Y_minI + "</Y></Point>" // I0
				"				</PointList>"
				"			</Polygon>"
				"		</Object>"
				"	</ObjectList>"
				"	<Vendor>"
				"		<Name>PJATK Video Processor</Name>"
				"	</Vendor>"
				"</AnalyticsEvent>";

			// Return XML string
			return xmlString;
		}

		//! Sends alert to the camera
		bool AlertSender::SendAlert(const std::string& inName, const boost::posix_time::ptime& inTime, const std::string inDescription, const cv::Rect2d& inAreaOfInterest, 
			const std::string& inLocation, const cv::Scalar& inBoxColor)
		{
			// Validate function parameters
			std::string nonPolish = SubstituteSpecialSymbols(inName, MS_POLISH_SYMBOLS, MS_POLISH_REPLACERS);
			std::string strippedName = StripAndDefault(nonPolish, "UndefinedAction");
			std::string strippedDesc = StripAndDefault(inDescription, "NoDescription");
			std::string htmlDesc = ConvertLineBreaks(strippedDesc);
			std::string strippedLoc = StripAndDefault(inLocation, "NoLocation");
			cv::Rect2d validAOI = ValidateAndDefaultAOI(inAreaOfInterest);
			cv::Scalar validColor = ValidateAndDefaultColor(inBoxColor);

			// Generate XML string to send
			std::string xmlQuery = GenerateXMLQuery(strippedName, inTime, htmlDesc, validAOI, m_CameraGUID, strippedLoc, validColor);

			// Generate HTTP/Post request
			m_LastErrorDescription.clear();
			bool retCode = web::BlockingHTTPPostXML_NoAuth(m_ServerCredentials.GetIP(), MS_DEFAULT_EVENT_PORT_STRING, xmlQuery, m_LastErrorDescription);
			return (m_LastErrorDescription.length() > 0) ? false : retCode;
		}

		//! Strips string from HTML tags (most of them)
		std::string AlertSender::StripHTMLTags(const std::string& inStr)
		{
			std::string retVal = inStr;
			retVal.erase(boost::remove_if(retVal, boost::is_any_of(MS_TAGS_TO_STRIP)), retVal.end());
			return retVal;
		}

		//! Strips tags and uses default value for empty strings
		std::string AlertSender::StripAndDefault(const std::string& inStr, const std::string& inDefaultValue)
		{
			std::string retVal = StripHTMLTags(inStr);
			return (retVal.length() == 0) ? inDefaultValue : retVal;
		}

		//! Validates Box dimensions (it should be always on screen - clamps if necessary)
		cv::Rect2d AlertSender::ValidateAndDefaultAOI(const cv::Rect2d& inAreaOfInterest)
		{
			// Copy rect
			cv::Rect2d retVal = inAreaOfInterest;

			// X, Y on screen
			retVal.x = clamp_to_uniform(retVal.x);
			retVal.y = clamp_to_uniform(retVal.y);

			// Width (clamp to maximal value)
			if ((retVal.x + retVal.width) > 1.0)
				retVal.width = 1.0 - retVal.x;

			// Height (clamp to maximal value)
			if ((retVal.y + retVal.height) > 1.0)
				retVal.height = 1.0 - retVal.y;

			return retVal;
		}

		//! Validates color (overwrites alpha)
		cv::Scalar AlertSender::ValidateAndDefaultColor(const cv::Scalar& inBoxColor)
		{
			// Clamp all values and set alpha to 1.0
			cv::Scalar retVal(
				clamp_to_uniform(inBoxColor[0]), 
				clamp_to_uniform(inBoxColor[1]),
				clamp_to_uniform(inBoxColor[2]), 
				1.0);

			return retVal;
		}

		//! Substitutes diacritical symbols (e.g. polish characters)
		std::string AlertSender::SubstituteSpecialSymbols(const std::string& inStr, const std::wstring& charsToFind, const std::wstring& charsToReplaceWith)
		{
			// Check length (we gonna operate on raw indices)
			size_t len = charsToFind.length();
			if (len != charsToReplaceWith.length())
				return "<REPLACERS DON'T MATCH>";

			// Make it QtString
			QString qStr(inStr.c_str());

			// I don't know an easy way
			std::wstring outStr(qStr.toStdWString());
			for (auto& currC : outStr)
			{
				// Find character in polish map
				auto pos = charsToFind.find(currC);
				if (pos == std::string::npos)
					continue; // Valid Character

				// Replace
				currC = charsToReplaceWith[pos];
			}

			// Convert to std string
			std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
			return converter.to_bytes(outStr);
		}

		//! Converts line breaks ('\n') to "</br>
		std::string AlertSender::ConvertLineBreaks(const std::string& inStr)
		{
			// Perform copy
			std::string result = boost::replace_all_copy(inStr, MS_LINE_END, MS_LINE_HTML_BREAK);
			return result;
		}
		

	} // camera

} // utils