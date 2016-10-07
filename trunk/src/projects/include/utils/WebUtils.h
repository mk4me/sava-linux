//
//
//  @ Project : Milestone
//  @ File Name : WebUtils.h
//  @ Date : 2016-02-18
//  @ Author : Kamil Lebek
//
//


#ifndef _WEBUTILS_H
#define _WEBUTILS_H

//
// CURL
//

// Lib (CURL is shipped in Relase, so no difference)
#ifdef _DEBUG
#pragma comment(lib, "libcurl_imp.lib")
#else
#pragma comment(lib, "libcurl_imp.lib")
#endif

#include <string>

//! Global utils namespace
namespace utils
{
	//! Simple web operations namespace
	namespace web
	{
		//! Performs blocking HTTP Request(GET) using basic authentication
		/*!
			\param inIPAddress IP address w/o protocol name e.g. "1.2.3.4"
			\param inOperationSuffix request resource e.g. "/filePath.txt" (slash is IMPORTANT)
			\param inUser username w/o white spaces e.g. "user"
			\param inPassword password w/o white space e.g. "password"
			\param outResponseString server response (get CONTENTS w/o HEADER)
			\return Returns 'true' if request was a success (IMPORTANT: function doesn't check credentials - you'll still get a proper response e.g. Error 401)
		*/
		bool BlockingHTTPGet(const std::string& inIPAddress, const std::string& inOperationSuffix, const std::string& inUser, const std::string& inPassword, std::string& outResponseString);

		//! Performs blocking HTTP Request(POST) for XML content w/o authentication
		/*!
			\param inIPAddress IP address w/o protocol name e.g. "1.2.3.4"
			\param inOperationSuffix request resource e.g. "/filePath.txt" (slash is IMPORTANT)
			\param inXmlPostData XML data to send
			\param outResponseString server response (get CONTENTS w/o HEADER)
			\return Returns 'true' if request was a success (IMPORTANT: function doesn't check credentials - you'll still get a proper response e.g. Error 401)
		*/
		bool BlockingHTTPPostXML_NoAuth(const std::string& inIPAddress, const std::string& inOperationSuffix, const std::string& inXmlPostData, std::string& outResponseString);

	} // web

} // utils

#endif // _WEBUTILS_H