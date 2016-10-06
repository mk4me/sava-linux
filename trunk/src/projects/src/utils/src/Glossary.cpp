#include "Glossary.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include "utils/Filesystem.h"

namespace utils
{
	Glossary::Glossary()
	{
		filePath = utils::Filesystem::getDataPath() + "Glossary.txt";
	}

	bool Glossary::load()
	{
		try
		{
			std::fstream fs;
			fs.open(filePath, std::fstream::in);

			std::string line;
			annotations.clear();
			while (getline(fs, line))
				annotations.push_back(line);

			fs.close();
		}
		catch (const std::exception& e)
		{
			std::cerr << "utils::Glossary::load() exception: " << e.what() << std::endl;
			return false;
		}

		return true;
	}

	bool Glossary::save()
	{
		try
		{
			std::fstream fs;
			fs.open(filePath, std::fstream::out);

			for (int i = 0; i < annotations.size(); ++i)
				fs << annotations[i] << std::endl;

			fs.close();
		}
		catch (const std::exception& e)
		{
			std::cerr << "utils::Glossary::save() exception: " << e.what() << std::endl;
			return false;
		}

		return true;
	}

	void  Glossary::add(const std::string& _newAnn)
	{
		annotations.push_back(_newAnn);
	}

	bool Glossary::edit(const int& _id, const std::string& _newAnn)
	{
		if (_id < annotations.size())
		{
			annotations[_id] = _newAnn;
			return true;
		}

		return false;
	}

	bool Glossary::edit(const std::string& _oldAnn, const std::string& _newAnn)
	{
		auto it = std::find(annotations.begin(), annotations.end(), _oldAnn);
		if (it != annotations.end())
		{
			*it = _newAnn;
			return true;
		}

		return false;
	}

	bool Glossary::remove(const int& _id)
	{
		if (_id < annotations.size())
		{
			annotations.erase(annotations.begin() + _id);
			return true;
		}

		return false;
	}

	bool Glossary::remove(const std::string& _ann)
	{
		auto it = std::find(annotations.begin(), annotations.end(), _ann);
		if (it != annotations.end())
		{
			annotations.erase(it);
			return true;
		}

		return false;
	}
}