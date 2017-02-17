/********************************************************************
	created:  2016/11/09 11:57
	filename: ArchiveConverter.h
	author:	  Wojciech Knie�
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_ARCHIVECONVERTER__ARCHIVECONVERTER_H__
#define HEADER_GUARD_ARCHIVECONVERTER__ARCHIVECONVERTER_H__

#include <boost/filesystem.hpp>

class ArchiveConverter
{
public:
	static void convertDir(const boost::filesystem::path& inputFolder, const boost::filesystem::path& outputFolder);
	static void convertToText(const boost::filesystem::path& inputFolder, const boost::filesystem::path& outputFolder);
	static void convertToBinary(const boost::filesystem::path& inputFolder, const boost::filesystem::path& outputFolder);
};


#endif