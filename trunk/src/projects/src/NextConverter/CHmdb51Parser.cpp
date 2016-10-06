#include "CHmdb51Parser.h"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <map>
#include <iostream>

const std::string CHmdb51Parser::trainFolder = "train";
const std::string CHmdb51Parser::testFolder = "test";
const std::string CHmdb51Parser::splitMarker = "split1";

void CHmdb51Parser::Parse(const std::string _srcVideo, const std::string _srcClassification, const std::string _dst)
{
	namespace fs = boost::filesystem;
	
	const fs::path srcVideoPath(_srcVideo);
	if (!fs::exists(srcVideoPath) || !fs::is_directory(srcVideoPath))
		return;

	const fs::path srcCalssificationPath(_srcClassification);
	if (!fs::exists(srcCalssificationPath) || !fs::is_directory(srcCalssificationPath))
		return;

	const fs::path dstPath(_dst);
	if (!fs::exists(dstPath) || !fs::is_directory(dstPath))
		return;

	fs::recursive_directory_iterator videoIterEnd;
	std::map<std::string, int> splitMap;
	int dirIndex = -1;

	//lecimy po wszystkich folderach z video
	for (fs::recursive_directory_iterator videoDirIter(srcVideoPath); videoDirIter != videoIterEnd; ++videoDirIter)
	{
		//poszukaj pliku txt i zrob klasyfikacje
		if (fs::is_directory(videoDirIter->path()))
		{
			const std::string folderName = videoDirIter->path().stem().string();
			const fs::path classificationFilePath(_srcClassification + "/" + folderName + "_test_" + splitMarker + ".txt");			 
			splitMap.clear();

			//jezeli jest parsujemy
			if (fs::exists(classificationFilePath))
			{
				std::ifstream classificationFile(classificationFilePath.string());
				std::string line;
				while (std::getline(classificationFile, line))
				{
					std::vector<std::string> splitedLine;
					boost::split(splitedLine, line, boost::is_any_of(" "));

					if (splitedLine.size() >= 2)
						splitMap.insert(std::pair<std::string, int>(splitedLine[0], std::stoi(splitedLine[1])));				
				}	

				//
				dirIndex++;
			}

			continue;
		}

		//jezeli nie znaleziono pliku do klasyfikacji to nie rozdzielamy plikow
		if (splitMap.size() == 0)
			continue;

		const std::string fileName = videoDirIter->path().filename().string();
		auto it = splitMap.find(fileName);
		
		//znaleziono, a te co maja index 0 nie kopiujemy
		if (it != splitMap.end() && it->second != 0)
		{
			const std::string subCopyFolder = it->second == 1 ? trainFolder : testFolder;
			const fs::path dstFileName = _dst + "/" + std::to_string(dirIndex) + "/" + subCopyFolder + "/" + fileName;

			//stworz foldery posrednie			
			fs::create_directories(dstFileName.parent_path());
			
			try
			{
				std::cout << "Copying: " << videoDirIter->path().string() << " => " << dstFileName.string() << std::endl;
				fs::copy_file(videoDirIter->path(), dstFileName, fs::copy_option::overwrite_if_exists);				
			}
			catch (const fs::filesystem_error& e)
			{
				std::cerr << "Error: " << e.what() << std::endl;
			}			
		}
	}
}