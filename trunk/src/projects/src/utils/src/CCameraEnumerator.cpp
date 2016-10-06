//
//
//  @ Project : Milestone
//  @ File Name : CCameraEnumerator.cpp
//  @ Date : 2016-02-19
//  @ Author : Kamil Lebek
//
//


#include "CCameraEnumerator.h"

// Boost XML parser and property tree
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>

// HTTP Get
#include <utils/WebUtils.h>

// String stream
#include <sstream>

//! Global utils namespace
namespace utils
{
	//! Camera support namespace
	namespace camera
	{
		//
		// Class configuration
		//

		//! Milestone config URL
		const std::string CCameraEnumerator::MILESTONE_CONFIG_URL = "/systeminfo.xml";

		//! Milestone - path to camera config in XML
		const std::string CCameraEnumerator::MILESTONE_CONFIG_CAM_SECTION = "methodresponse.engines.engine.cameras";

		//! Milestone - camera config XML - name field
		const std::string CCameraEnumerator::MILESTONE_CONFIG_NAME_FIELD = "<xmlattr>.cameraid";

		//! Milestone - camera config XML - GUID field
		const std::string CCameraEnumerator::MILESTONE_CONFIG_GUID_FIELD = "guid";

		// 
		// Implementation
		//
		
		//! Retrieves cameras configuration
		bool CCameraEnumerator::GetCamerasConfig(std::vector<SMilestoneCameraParameters>& outCameras) const
		{
			// Clear out structure
			outCameras.clear();

			// Query server config
			std::string serverConfigXml;
			if (!web::BlockingHTTPGet(m_MilestoneLogin.GetIP(), MILESTONE_CONFIG_URL, m_MilestoneLogin.GetUserName(), m_MilestoneLogin.GetPassword(), serverConfigXml))
				return false;

			// Property tree
			boost::property_tree::ptree myPt;
			try
			{
				// Convert string to string stream to user XML parser in ptree
				std::stringstream serverConfigStream;
				serverConfigStream << serverConfigXml;
				
				// Parse property tree from XML
				boost::property_tree::read_xml(serverConfigStream, myPt);

				// Get camera section
				auto cameraSection = myPt.get_child(MILESTONE_CONFIG_CAM_SECTION);

				// Iterate
				for (const auto& camItem : cameraSection)
				{
					// Get fields
					std::string camNameField = camItem.second.get<std::string>(MILESTONE_CONFIG_NAME_FIELD);
					std::string camGUIDField = camItem.second.get<std::string>(MILESTONE_CONFIG_GUID_FIELD);

					// Add to our structure
					outCameras.push_back(camera::SMilestoneCameraParameters(camNameField, camGUIDField));
				}
			}
			catch (...)
			{
				// Suppress all errors
				return false;
			}

			// All ok
			return true;
		}

	} // camera

} // utils