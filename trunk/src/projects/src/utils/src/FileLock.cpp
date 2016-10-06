#include "FileLock.h"

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
#warning Not implemented yet
	throw std::runtime_error("Not implemented yet");
#endif // WIN32

	return m_File != nullptr;
}

void utils::FileLock::unlock()
{
	if (m_File != nullptr)
		fclose(m_File);
	m_File = nullptr;
}

