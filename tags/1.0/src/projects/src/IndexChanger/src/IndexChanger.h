/********************************************************************
	created:  2016/11/09 11:57
	filename: ArchiveConverter.h
	author:	  Wojciech Kniec
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_ARCHIVECONVERTER__ARCHIVECONVERTER_H__
#define HEADER_GUARD_ARCHIVECONVERTER__ARCHIVECONVERTER_H__

#include <boost/filesystem.hpp>

class IndexChanger
{
public:
	static void convertDir(const boost::filesystem::path& inputFolder, const boost::filesystem::path& outputFolder);
};


#endif