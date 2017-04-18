#include "FileLock.h"
#include <stdexcept>
#include <fcntl.h>

#ifndef WIN32
	#include <unistd.h>
#endif

#include <utils/Filesystem.h>
#include <iostream>
#include <fstream>

utils::FileLock::FileLock(const std::string& fileName) 
	: m_Filename(fileName)
	, m_File(nullptr)

{

}

utils::FileLock::~FileLock()
{
	unlock();
}

bool utils::FileLock::lock()
{
	if (m_File != nullptr)
		return true;

#ifdef WIN32
	m_File = _fsopen(m_Filename.c_str(), "wD", _SH_DENYRW);
	
#else
    try {
        // recreating windows behaviour, if file does not exist - we create one
        bool exist = utils::Filesystem::exists(m_Filename.c_str());
        if (!exist) {
            std::ofstream emptyFile(m_Filename.c_str());
            emptyFile.close();
        }
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        return false;
    }
    fl = std::make_unique<boost::interprocess::file_lock>(m_Filename.c_str());
    // non blocking try
    bool res = fl->try_lock();
    if (res) {
        // in this scenario m_File works as flag...
        m_File = (FILE*)1;
        return true;
    }
#endif // WIN32

	return m_File != nullptr;
}

void utils::FileLock::unlock()
{
	if (m_File != nullptr) {
#ifdef WIN32
		fclose(m_File);
#else
        fl->unlock();
#endif
	}
	m_File = nullptr;
}

