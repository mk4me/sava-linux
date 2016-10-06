#pragma once
#ifndef __GLOSSARY_H__
#define __GLOSSARY_H__

#include "Singleton.h"
#include <string>
#include <vector>

namespace utils
{
	class Glossary : public Singleton<Glossary>
	{
	public:
		Glossary();

		const std::string& getFilePath() const { return filePath; }
		void setFilePath(const std::string& _filePath) { filePath = _filePath; }

		bool load();
		bool save();

		const std::vector<std::string>& getAnnotations() const { return annotations; }
		void setAnnotations(const std::vector<std::string>& _annotations) { annotations = _annotations; }

		void add(const std::string& _newAnn);
		bool edit(const int& _id, const std::string& _newAnn);
		bool edit(const std::string& _oldAnn, const std::string& _newAnn);
		bool remove(const int& _id);
		bool remove(const std::string& _ann);

	private:
		std::string filePath;
		std::vector<std::string> annotations;
	};
}

#endif // __GLOSSARY_H__