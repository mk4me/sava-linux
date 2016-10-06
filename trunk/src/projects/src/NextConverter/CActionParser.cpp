#include "CActionParser.h"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <sequence/Action.h>
#include <sequence/Cluster.h>
#include <sequence/CompressedVideo.h>

const std::string CActionParser::acnExt = ".acn";
const std::string CActionParser::cluExt = ".clu";
const std::string CActionParser::vsqExt = ".vsq";

void CActionParser::Parse(const std::string _src, const std::string _dst)
{
	namespace fs = boost::filesystem;
	
	const fs::path srcPath(_src);
	if (!fs::exists(srcPath) || !fs::is_directory(srcPath))
		return;

	const fs::path dstPath(_dst);
	if (!fs::exists(dstPath) || !fs::is_directory(dstPath))
		return;

	fs::directory_iterator acnIterEnd;
	
	//lecimy po wszystkich acn
	for (fs::directory_iterator acnDirIter(srcPath); acnDirIter != acnIterEnd; ++acnDirIter)
	{
		if (acnDirIter->path().extension().string() != acnExt)
			continue;

		//wczytaj
		sequence::Action acnFile(acnDirIter->path().string());

		//zbuduj nowa sciezke docelowa
		std::string newDst = _dst;
		
		if (acnFile.getSplit() == sequence::Action::Split::GENERATED)
			continue;
		else if (acnFile.getSplit() == sequence::Action::Split::TRAIN)
			newDst += "/TRAIN";
		else if (acnFile.getSplit() == sequence::Action::Split::TEST)
			newDst += "/TEST";
				
		newDst += "/" + std::to_string(acnFile.getActionId());
		
		//stworz foldery posrednie			
		fs::create_directories(newDst);

		//do nowej lokacji kopiujemy plik CLU i VSQ, 
		//ale sprawdzamy czy przypadkiem nie istenija zeby nie kopiowac dwa razy
		{
			//clu src
			const fs::path cluFilePathSrc(srcPath.string() + "/" + acnDirIter->path().stem().string() + cluExt);
			if (fs::exists(cluFilePathSrc) && fs::is_regular_file(cluFilePathSrc))
			{
				//clu dst
				const fs::path cluFilePathDst(newDst + "/" + cluFilePathSrc.filename().string());
				if (!fs::exists(cluFilePathDst) || !fs::is_regular_file(cluFilePathDst))
				{
					try
					{
						std::cout << "Copying: " << cluFilePathSrc.string() << " => " << cluFilePathDst.string() << std::endl;
						fs::copy_file(cluFilePathSrc, cluFilePathDst, fs::copy_option::overwrite_if_exists);
					}
					catch (const fs::filesystem_error& e)
					{
						std::cerr << "Error: " << e.what() << std::endl;
					}
				}
			}

			//vsq src
			std::vector<std::string> acnSplitted;
			boost::split(acnSplitted, acnDirIter->path().stem().string(), boost::is_any_of("."));
			const fs::path vsqFilePathSrc(srcPath.string() + "/" + acnSplitted[0] + "." + acnSplitted[1] + "." + acnSplitted[2] + "." + acnSplitted[3] + "." + acnSplitted[4] + vsqExt);
			if (fs::exists(vsqFilePathSrc) && fs::is_regular_file(vsqFilePathSrc))
			{
				//vsq dst
				const fs::path vsqFilePathDst(newDst + "/" + vsqFilePathSrc.filename().string());
				if (!fs::exists(vsqFilePathDst) || !fs::is_regular_file(vsqFilePathDst))
				{
					try
					{
						std::cout << "Copying: " << vsqFilePathSrc.string() << " => " << vsqFilePathDst.string() << std::endl;
						fs::copy_file(vsqFilePathSrc, vsqFilePathDst, fs::copy_option::overwrite_if_exists);
					}
					catch (const fs::filesystem_error& e)
					{
						std::cerr << "Error: " << e.what() << std::endl;
					}
				}
			}
		}
	}
}