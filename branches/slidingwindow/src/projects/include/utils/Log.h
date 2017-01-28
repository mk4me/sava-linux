#pragma once
#ifndef Log_h__
#define Log_h__

#include <string>
#include <iostream>
#include <fstream>

namespace utils
{
	/// <summary>
	/// klasa implementuj¹ca mechanizm logowania do pliku.
	/// </summary>
	class Log
	{
	public:
		Log(bool clear = false);

		template<typename T>
		Log& operator<<(const T& obj)
		{
			m_File << obj;
			return *this;
		}

		static void clear();

	private:
		std::ofstream m_File;
	};

}

#endif // Log_h__