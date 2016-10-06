//
//
//  @ Project : Milestone
//  @ File Name : SMilestoneStructures.h
//  @ Date : 2016-02-19
//  @ Author : Kamil Lebek
//
//


#ifndef _SMILESTONECREDENTIALS_H
#define _SMILESTONECREDENTIALS_H

// String
#include <string>

// Trim
#include <boost/algorithm/string.hpp>

//! Global utils namespace
namespace utils
{
	//! Camera support namespace
	namespace camera
	{
		//! Helper object for storing Milestone server logging credentials
		struct SMilestoneCredentials
		{
			//! Explicit constructor with simple verification (white-spaces)
			/*!
				\param inIP server IP e.g. "1.2.3.4"
				\param inUserName Milestone administrator's user name e.g. "user"
				\param inPassword Milestone administrator's password e.g. "1234"
			*/
			explicit SMilestoneCredentials(const std::string& inIP, const std::string& inUserName, const std::string& inPassword) :
				m_IP(inIP), m_UserName(inUserName), m_Password(inPassword)
			{
				// No white spaces allowed
				boost::algorithm::trim(m_IP);
				boost::algorithm::trim(m_UserName);
				boost::algorithm::trim(m_Password);

				// No throw... if user is a dumbfuck, what could I do?
				if (!m_IP.length() || !m_UserName.length() || !m_Password.length())
				{
					m_IP = m_UserName = m_Password = "ERROR";
				}
			}

			//! Accessors: IP
			std::string GetIP() const
			{
				return m_IP;
			}

			//! Accessors: UserName
			std::string GetUserName() const
			{
				return m_UserName;
			}
			
			//! Accessors: Password
			std::string GetPassword() const
			{
				return m_Password;
			}

		private:
			//! Management server IP
			std::string m_IP;

			//! Administrator login
			std::string m_UserName;

			//! Administrator password
			std::string m_Password;

		}; // SMilestoneCredentials

		//! Helper object for storing Camera parameters
		struct SMilestoneCameraParameters
		{
			//! Explicit constructor to fill object once and make it read-only
			/*!
				\param inCameraName camera name 
				\param inCameraGUID camera GUID
			*/
			explicit SMilestoneCameraParameters(const std::string& inCameraName, const std::string& inCameraGUID) :
				m_Name(inCameraName), m_GUID(inCameraGUID)	
			{ 
			}

			//! Accessors: Name
			std::string GetName() const
			{
				return m_Name;
			}

			//! Accessors: GUID
			std::string GetGUID() const
			{
				return m_GUID;
			}

		private:
			//! Camera name
			std::string m_Name;

			//! Camera GUID
			std::string m_GUID;
		};

	} // camera

} // utils

#endif // _SMILESTONECREDENTIALS_H