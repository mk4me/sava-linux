//
//
//  @ Project : Milestone
//  @ File Name : WebUtils.cpp
//  @ Date : 2016-02-18
//  @ Author : Kamil Lebek
//
//

#include "WebUtils.h"

//
// CURL
//

// Include
#include <curl/curl.h>

//! Global utils namespace
namespace utils
{
	//! Simple web operations namespace
	namespace web
	{
		//! Internal stuff
		namespace internal
		{
			//! HTTP OK Response Code
			static long HTTP_OK_RC = 200;

			//! CURL Write function
			static size_t CURL_CustomWrite(char *ptr, size_t size, size_t nmemb, std::string* stringData)
			{
				// Actual bytes to write
				size_t toWrite = size * nmemb;

				// Append data to my custom buffer
				stringData->append(ptr, toWrite);

				// Regular operation (different value will stop the transfer)
				return toWrite;
			}

		} // internal

		//! Performs blocking HTTP Request (GET) using basic authentication 
		bool BlockingHTTPGet(const std::string& inIPAddress, const std::string& inOperationSuffix, const std::string& inUser, const std::string& inPassword, std::string& outResponseString)
		{
			// Init CURL
			CURL* myCurl = curl_easy_init();

			// Clear response buffer
			outResponseString.clear();

			// Create final request url
			std::string sUrl = inIPAddress + inOperationSuffix;

			// Set credentials
			std::string inUserAndPwd = inUser + ":" + inPassword;
			curl_easy_setopt(myCurl, CURLOPT_URL, sUrl.c_str());
			curl_easy_setopt(myCurl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
			curl_easy_setopt(myCurl, CURLOPT_USERPWD, inUserAndPwd.c_str());

			// Setup header
			struct curl_slist *slist = NULL;
			slist = curl_slist_append(slist, "User-Agent: libcurl-agent/1.0");

			// Setup write functions (for header and content)
			curl_easy_setopt(myCurl, CURLOPT_HTTPHEADER, slist);
			curl_easy_setopt(myCurl, CURLOPT_HEADER, 0);
			curl_easy_setopt(myCurl, CURLOPT_WRITEFUNCTION, internal::CURL_CustomWrite);
			curl_easy_setopt(myCurl, CURLOPT_WRITEDATA, &outResponseString);

			// Set temp error
			CURLcode opResult = CURLE_NO_CONNECTION_AVAILABLE;
			try
			{
				// Blocking operation that can be aborted in callback
				opResult = curl_easy_perform(myCurl);
			}
			catch (...)
			{
				// Exception? That's wrong.
				return false;
			}

			// Just clean shutdown
			curl_slist_free_all(slist);
			curl_easy_cleanup(myCurl);
			
			// Return value
			return ((opResult == CURLE_OK) ? true : false);
		}


		//! Performs blocking HTTP Request(POST) for XML content w/o authentication
		bool BlockingHTTPPostXML_NoAuth(const std::string& inIPAddress, const std::string& inOperationSuffix, const std::string& inXmlPostData, std::string& outResponseString)
		{
			// Init CURL
			CURL* myCurl = curl_easy_init();

			// Clear response buffer
			outResponseString.clear();

			// Create final request url
			std::string sUrl = inIPAddress + inOperationSuffix;

			// Set link
			curl_easy_setopt(myCurl, CURLOPT_URL, sUrl.c_str());

			// Setup header
			struct curl_slist *slist = NULL;
			slist = curl_slist_append(slist, "User-Agent: libcurl-agent/1.0");
			slist = curl_slist_append(slist, "Content-Type: text/xml; charset=utf-8"); // or whatever charset your XML is really using...

			// Setup write functions (for header and content)
			curl_easy_setopt(myCurl, CURLOPT_HTTPHEADER, slist);
			curl_easy_setopt(myCurl, CURLOPT_HEADER, 0);
			curl_easy_setopt(myCurl, CURLOPT_WRITEFUNCTION, internal::CURL_CustomWrite);
			curl_easy_setopt(myCurl, CURLOPT_WRITEDATA, &outResponseString);

			// Post parameters
			curl_easy_setopt(myCurl, CURLOPT_POST, 1);
			curl_easy_setopt(myCurl, CURLOPT_POSTFIELDS, inXmlPostData.c_str());
			curl_easy_setopt(myCurl, CURLOPT_POSTFIELDSIZE, inXmlPostData.length());


			// Set temp error
			long response_code;
			CURLcode opResult = CURLE_NO_CONNECTION_AVAILABLE;
			try
			{
				// Blocking operation that can be aborted in callback
				opResult = curl_easy_perform(myCurl);

				// Check response code (e.g. HTTP 200 OK)
				curl_easy_getinfo(myCurl, CURLINFO_RESPONSE_CODE, &response_code);
			}
			catch (...)
			{
				// Exception? That's wrong.
				return false;
			}

			// Just clean shutdown
			curl_slist_free_all(slist);
			curl_easy_cleanup(myCurl);

			// Return value
			if ((opResult == CURLE_OK) && (response_code == internal::HTTP_OK_RC))
				return true;
			else
				return false;
		}

	} // web

} // utils