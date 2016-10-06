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
// WARNING: It requires also LIB libcurl_imp.lib in Linker Input
//
#include <curl/curl.h>

//! Global utils namespace
namespace utils
{
	//! Simple web operations namespace
	namespace web
	{
		//! CURL Write function
		static size_t internal_CURL_CustomWrite(char *ptr, size_t size, size_t nmemb, std::string* stringData)
		{
			// Actual bytes to write
			size_t toWrite = size * nmemb;

			// Append data to my custom buffer
			stringData->append(ptr, toWrite);

			// Regular operation (different value will stop the transfer)
			return toWrite;
		}

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
			curl_easy_setopt(myCurl, CURLOPT_WRITEFUNCTION, internal_CURL_CustomWrite);
			curl_easy_setopt(myCurl, CURLOPT_WRITEDATA, &outResponseString);

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

	} // web

} // utils