#include "FileLock.h"
#include <stdexcept>
#include <fcntl.h>
#include <unistd.h>

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
	struct flock fl;
	int fd;

	fl.l_type   = F_WRLCK;  /* F_RDLCK, F_WRLCK, F_UNLCK    */
	fl.l_whence = SEEK_SET; /* SEEK_SET, SEEK_CUR, SEEK_END */
	fl.l_start  = 0;        /* Offset from l_whence         */
	fl.l_len    = 0;        /* length, 0 = to EOF           */
	fl.l_pid    = getpid(); /* our PID                      */

	fd = open(m_Filename.c_str(), O_WRONLY);

	fcntl(fd, F_SETLKW, &fl);  /* F_GETLK, F_SETLK, F_SETLKW */
	m_File = fd;
#endif // WIN32

	return m_File != nullptr;
}

void utils::FileLock::unlock()
{
	if (m_File != nullptr) {
#ifdef WIN32
		fclose(m_File);
#else

	struct flock fl;
	int fd;
	fd = m_File;
	fl.l_type   = F_WRLCK;  /* F_RDLCK, F_WRLCK, F_UNLCK    */
	fl.l_whence = SEEK_SET; /* SEEK_SET, SEEK_CUR, SEEK_END */
	fl.l_start  = 0;        /* Offset from l_whence         */
	fl.l_len    = 0;        /* length, 0 = to EOF           */
	fl.l_pid    = getpid(); /* our PID                      */

	//fd = open("filename", O_WRONLY);  /* get the file descriptor */
	//fcntl(fd, F_SETLKW, &fl);  /* set the lock, waiting if necessary */

	fl.l_type   = F_UNLCK;  /* tell it to unlock the region */
	fcntl(fd, F_SETLK, &fl); /* set the region to unlocked */

#endif
	}
	m_File = nullptr;
}

