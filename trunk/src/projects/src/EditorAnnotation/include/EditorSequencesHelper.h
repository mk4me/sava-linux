#ifndef EDITOR_SEQUENCES_MANAGER_H_
#define EDITOR_SEQUENCES_MANAGER_H_

#include <utils/Singleton.h>
#include <vector>
#include <string>
#include <sstream>

#include <QtCore/QDir>

/*--------------------------------------------------------------------*/
class FileNameComparator
{
public:
	bool operator()(const std::string & x, const std::string & y)
	{

		std::string s = x.substr(0, x.find_last_of("."));
		std::stringstream compared;
		compared << s.substr(s.find_last_of(".") + 1, s.length());
		int compIntXY;
		compared >> compIntXY;
		compared.clear();
		s = s.substr(0, s.find_last_of("."));
		compared << s.substr(s.find_last_of(".") + 1, s.length());
		int compIntXX;
		compared >> compIntXX;

		compared.clear();
		s = y.substr(0, y.find_last_of("."));
		compared << s.substr(s.find_last_of(".") + 1, s.length());
		int compIntYY;
		compared >> compIntYY;
		compared.clear();
		s = s.substr(0, s.find_last_of("."));
		compared << s.substr(s.find_last_of(".") + 1, s.length());
		int compIntYX;
		compared >> compIntYX;

		if (compIntXX < compIntYX) {
			return true;
		}

		if (compIntXX > compIntYX) {
			return false;
		}

		return compIntXY < compIntYY;
	}
};

/*--------------------------------------------------------------------*/
class EditorSequencesHelper
{
public:
	EditorSequencesHelper();
	~EditorSequencesHelper();

	/** 
		\brief try to load sequence files from given folder
		\param i_Dir path to dir, from which sequences should be loaded
		\return true if success, otherwise false
	*/
	bool load(const std::string i_Dir);

	/**
		\brief get sequences directory 
		\return sequences directory
	*/
	std::string getDir() const { return m_Dir;}

	/**
		\brief get list of sequences files
		\return names vector of sequences files
	*/
	std::vector<std::string> getFiles() { return m_Files; }

private:
	std::string m_Dir;
	std::vector<std::string> m_Files;
};


#endif //EDITOR_SEQUENCES_MANAGER_H_

