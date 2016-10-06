#ifndef FileLock_h__
#define FileLock_h__

#include <fstream>
#include <string>

namespace utils
{
	class FileLock
	{
	public:
		// parameters equivalent to fopen function from stdio
		FileLock(const std::string& fileName);
		~FileLock();

		bool lock();
		void unlock();

	private:
		FILE* m_File;
		std::string m_Filename;
	};
}

#endif // FileLock_h__
