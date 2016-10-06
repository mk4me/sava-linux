// Cout 
#include <iostream>

// Camera Enumerator
#include <utils/CCameraEnumerator.h>

//! Main entry point
int main(int argc, const char* argv[])
{
	// Prepare credentials
	utils::camera::CCameraEnumerator camEnum(utils::camera::SMilestoneCredentials("172.16.100.83", "config_user", "1234"));
	std::vector<utils::camera::SMilestoneCameraParameters> cameraList;
	
	// Retrieve cameras' config
	if (!camEnum.GetCamerasConfig(cameraList))
	{
		std::cout << "Failed to get server config" << std::endl;
		std::getchar();
		return 1;
	}

	// Show all cameras
	std::cout << "CAMERA LIST:" << std::endl;
	for (const auto camDesc : cameraList)
	{
		std::cout << "Camera: " << camDesc.GetName() << " GUID: " << camDesc.GetGUID() << std::endl;
	}

	std::getchar();
	return 0;
}