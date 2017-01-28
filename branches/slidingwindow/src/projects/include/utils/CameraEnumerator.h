//
//
//  @ Project : Milestone
//  @ File Name : CameraEnumerator.h
//  @ Date : 2016-02-19
//  @ Author : Kamil Lebek
//
//


#ifndef _CAMERAENUMERATOR_H
#define _CAMERAENUMERATOR_H

#include <vector>

#include "MilestoneStructures.h"

//! Global utils namespace
namespace utils
{
	//! Camera support namespace
	namespace camera
	{
		//! Camera configuration retriever
		class CameraEnumerator
		{
		public:
			//! Argument constructor (using credentials class)
			explicit CameraEnumerator(const MilestoneCredentials& inMilestoneLogin) : m_MilestoneLogin(inMilestoneLogin) { }

			//! Simple destructor
			virtual ~CameraEnumerator() { }

			//! Retrieves cameras configuration
			/*!
				\param outCameras vector containing parameters for cameras attached to Milestone server (name and GUID)
				\return False if query failed (wrong credentials, server offline).
			*/
			bool GetCamerasConfig(std::vector<MilestoneCameraParameters>& outCameras) const;

		private:
			//! Milestone server credentials
			MilestoneCredentials m_MilestoneLogin;

			//! Milestone config URL
			static const std::string MILESTONE_CONFIG_URL;

			//! Milestone - path to camera config in XML
			static const std::string MILESTONE_CONFIG_CAM_SECTION;

			//! Milestone - camera config XML - name field
			static const std::string MILESTONE_CONFIG_NAME_FIELD;

			//! Milestone - camera config XML - GUID field
			static const std::string MILESTONE_CONFIG_GUID_FIELD;

		}; // CameraEnumerator

	} // camera

} // utils

#endif // _CAMERAENUMERATOR_H