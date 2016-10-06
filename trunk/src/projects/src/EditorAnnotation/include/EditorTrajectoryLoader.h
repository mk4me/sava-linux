#pragma once

#include "boost/archive/text_oarchive.hpp"
#include "boost/archive/text_iarchive.hpp"
#include <fstream>


/**
 * \class EditorTrajectoryLoader
 *
 * \brief Archiving trajectory data (load/save data from EditorTrajectoryManager object).
 */
class EditorTrajectoryLoader {

public:
	
	/**
	 * Load trajectory data from file.
	 * \param manager object to load data from file.
	 * \param i_FilePath path to file, which includes trajectory data.
	 * \return true if succeeded, otherwise false.
	 */
	static bool load(EditorTrajectoryManager& manager, const std::string& i_FilePath )
	{
		std::ifstream file(i_FilePath);

		if (file.good())
		{
			boost::archive::text_iarchive iarchive(file);
			iarchive >> manager;
			file.close();
			return true;
		}

		return false;
	}



	/**
	* Save trajectory data to file.
	* \param manager serializable object
	* \param i_FilePath path to file.
	* \return true if succeeded, otherwise false.
	*/
	static void save(const EditorTrajectoryManager& manager, const std::string& i_FilePath)
	{
		std::ofstream file(i_FilePath);

		if (file.good()) {
			try {
				boost::archive::text_oarchive oarchive(file);
				oarchive << manager;
				file.close();

				std::cout << "Trajectories saved. " << std::endl;
			}
			catch (...)
			{
				std::cout << "Can't save file: " << i_FilePath << std::endl;
			};
		}
	}
};



