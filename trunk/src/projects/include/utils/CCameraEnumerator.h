//
//
//  @ Project : Milestone
//  @ File Name : CCameraEnumerator.h
//  @ Date : 2016-02-19
//  @ Author : Kamil Lebek
//
//


#ifndef _CCAMERAENUMERATOR_H
#define _CCAMERAENUMERATOR_H

#include <vector>

#include "SMilestoneStructures.h"

//! Global utils namespace
namespace utils
{
	//! Camera support namespace
	namespace camera
	{
		//! Camera configuration retriever
		class CCameraEnumerator
		{
		public:
			//! Argument constructor (using credentials class)
			explicit CCameraEnumerator(const SMilestoneCredentials& inMilestoneLogin) : m_MilestoneLogin(inMilestoneLogin) { }

			//! Simple destructor
			virtual ~CCameraEnumerator() { }

			//! Retrieves cameras configuration
			/*!
				\param outCameras vector containing parameters for cameras attached to Milestone server (name and GUID)
				\return False if query failed (wrong credentials, server offline).
			*/
			bool GetCamerasConfig(std::vector<SMilestoneCameraParameters>& outCameras) const;

		private:
			//! Milestone server credentials
			SMilestoneCredentials m_MilestoneLogin;

			//! Milestone config URL
			static const std::string MILESTONE_CONFIG_URL;

			//! Milestone - path to camera config in XML
			static const std::string MILESTONE_CONFIG_CAM_SECTION;

			//! Milestone - camera config XML - name field
			static const std::string MILESTONE_CONFIG_NAME_FIELD;

			//! Milestone - camera config XML - GUID field
			static const std::string MILESTONE_CONFIG_GUID_FIELD;

		}; // CCameraEnumerator

	} // camera

} // utils

#endif // _CCAMERAENUMERATOR_H