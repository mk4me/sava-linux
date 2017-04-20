#include "PathAnalyzer.h"

#include <sequence/Cluster.h>

#include <utils/FileLock.h>
#include <utils/Filesystem.h>

#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>

clustering::PathAnalyzer::PathAnalyzer() : m_MinProcessTime(0)
{

}

clustering::PathAnalyzer::~PathAnalyzer()
{

}

void clustering::PathAnalyzer::setFiles(const std::string& inputSequence, const std::string& inputPaths, const std::string& outputClusterPattern, const std::string& outputClusterExtension)
{
	m_InputSequence = inputSequence;
	m_InputPaths = inputPaths;
	m_OutputClusterPattern = outputClusterPattern;
	m_OutputClusterExtension = outputClusterExtension;
	m_OutputClusterId = 0;
}

std::string clustering::PathAnalyzer::getInFileName() const
{
	return m_InputPaths;
}

std::string clustering::PathAnalyzer::getVideoFileName() const
{
	return m_InputSequence;
}

std::string clustering::PathAnalyzer::getOutFileName(unsigned id) const
{
	return m_OutputClusterPattern + std::to_string(id) + m_OutputClusterExtension;
}

float clustering::PathAnalyzer::match(const cv::Rect& R, const cv::Rect& A)
{
	return std::min(std::max(0, std::min(R.br().x, A.br().x) - std::max(R.tl().x, A.tl().x)) / float(A.width),
		std::max(0, std::min(R.br().y, A.br().y) - std::max(R.tl().y, A.tl().y)) / float(A.height));
}

std::shared_ptr<sequence::Cluster> clustering::PathAnalyzer::clusterglueshift(std::shared_ptr<sequence::Cluster> previousClu, std::shared_ptr<sequence::Cluster> currentClu, int shiftValue)
{
	std::map<size_t, cv::Point> currentFramesPositions = currentClu->getFramesPositions();
	std::map<size_t, cv::Point> previousFramesPositions = previousClu->getFramesPositions();
	auto currentNumFrames = currentFramesPositions.size();
	auto previousNumFrames = previousFramesPositions.size();
	auto currentFrameSize = currentClu->getFrameSize();
	auto previousFrameSize = previousClu->getFrameSize();
	auto x = std::max(previousFrameSize.height, currentFrameSize.height);
	auto y = std::max(previousFrameSize.width, currentFrameSize.width);
	cv::Size newFrameSize(x, y);
	int newId = previousClu->getClusterId();
	auto ShiftedCluster = std::make_shared<sequence::Cluster>(newId);
	ShiftedCluster->setFrameSize(newFrameSize);
	std::map<size_t, cv::Point> newFramesPositions;
	std::map<size_t, cv::Point>::iterator it = newFramesPositions.begin();
	for (auto i = 0 + shiftValue; i < previousNumFrames + shiftValue; ++i)
	{
		if (i<previousNumFrames)
		{
			auto insert1 = previousFramesPositions.at(i);
			newFramesPositions.insert(it, std::pair<size_t, cv::Point>(i - shiftValue, insert1));
		}
		else
		{
			auto insert2 = currentFramesPositions.at(i - currentNumFrames);
			newFramesPositions.insert(it, std::pair<size_t, cv::Point>(i - shiftValue, insert2));
		}
	}
	ShiftedCluster->setFramesPositions(newFramesPositions);
	return ShiftedCluster;
}

void clustering::PathAnalyzer::saveCluster(sequence::Cluster& cluster)
{
	
	std::string outFile = getOutFileName(m_OutputClusterId++);
	//std::string m_InputFolder = "Z:\\\WS2\\wynikitestukompresji\\";
	char conditional_char = outFile.at(outFile.length() - 7);
	int conditional_int = conditional_char - 48;
	if (conditional_int > 2)
	{
		std::vector<std::shared_ptr<sequence::Cluster>> previousClusters;
		std::vector<std::string> previousClustersNames;
		int previousVal = conditional_int - 2;
		for (int i = 0;; ++i)
		{
			//std::string filename = m_InputFolder + "192.168.201.7" + "." + "0" + "." + std::to_string(i) + ".clu";
			
			std::string filename = outFile.replace(outFile.end() - 5, outFile.end() - 4, boost::lexical_cast<std::string>(i));
			std::string filename = filename.replace(filename.end() - 7, filename.end() - 6, boost::lexical_cast<std::string>(previousVal));
			if (!boost::filesystem::exists(filename))
				break;
			previousClusters.push_back(std::make_shared<sequence::Cluster>(filename)); //WRZUC NA TABLICE WSZYSTKIE WYGENEROWANE KLASTRY Z PODANEGO FOLDERU WEJSCOWEGO
			previousClustersNames.push_back(filename); //WRZUC NA TABLICE WSZYSTKIE WYGENEROWANE KLASTRY Z PODANEGO FOLDERU WEJSCOWEGO


		//TUTAJ WCZYTAJ WSZYSTKIE PLIKI CLU, KTORE MAJA 1 DRUGIE OD KONCA, 192.168.201.7.1.0
		std::vector<std::shared_ptr<sequence::Cluster>> currentClusters;
		std::vector<std::string> currentClustersNames;
		int currentVal = conditional_int - 1;
		for (int i = 0;; ++i)
		{
			std::string filename = outFile.replace(outFile.end() - 5, outFile.end() - 4, boost::lexical_cast<std::string>(i));
			std::string filename = filename.replace(filename.end() - 7, filename.end() - 6, boost::lexical_cast<std::string>(currentVal));
			if (!boost::filesystem::exists(filename))
				break;
			currentClusters.push_back(std::make_shared<sequence::Cluster>(filename)); //WRZUC NA TABLICE WSZYSTKIE WYGENEROWANE KLASTRY Z PODANEGO FOLDERU WEJSCOWEGO
			currentClustersNames.push_back(filename);													  //Wyniki.open(m_ResultFilename, std::ios::ate);
		}

		int m_SequenceLength = 75; // ODHARDKODUJ!!
		float m_Threshold = 0.45;

		std::vector<int> bestMatches;
		//POPRAWIONE ZNAJDOWANIE
		//PONIZEJ KOLEJNO DLA KAZDEGO POPRZEDNIEGO KLASTRA SZUKA SIE SPOSROD
		//WSZYSTKICH KOLEJNYCH KLASTROW NAJLEPSZEGO DOPASOWANIA
		//A NASTEPNIE NAJLEPSZE DOPASOWANIE POD WARUNKIEM SPELNIENIA KRYTERIUM
		//WIEKSZE OD PROGU TRAFIA DO STRUKTURY BESTMATCHES, KTORA PRZECHOWUJE
		//NAZWY KLASTROW, KTORE ZOSTANA POLACZONE

		for (size_t a = 0; a < previousClusters.size(); ++a)
		{
			std::vector<float> consequentMatches;
			cv::Rect A = previousClusters[a]->getFrameAt(m_SequenceLength - 1);
			if (A.area() == 0)
				continue;

			for (size_t r = 0; r < currentClusters.size(); ++r)
			{
				cv::Rect R = currentClusters[r]->getFrameAt(0);
				//if (R.area() == 0)
				//	continue;


				consequentMatches.push_back(match(R, A));
				float ma = match(R, A);
				std::cout << "Previous cluster number: " << a << std::endl;
				std::cout << "Current cluster number: " << r << std::endl;
				std::cout << "Match: " << ma << std::endl;
			}
			std::vector<float>::iterator result;
			result = std::max_element(consequentMatches.begin(), consequentMatches.end());
			int dist = std::distance(consequentMatches.begin(), result);
			float bestMatch = consequentMatches.at(dist);
			if (bestMatch>m_Threshold) //JESLI DOPASOWANIE MNIEJSZE OD PROGU TO POMIN, JESLI NIE TO SKLEJ 2 KLASTRY
			{
				std::cout << "max element at: " << dist << '\n';
				bestMatches.push_back(dist);
			}
			else
			{
				bestMatches.push_back(-1);
			}
			consequentMatches.clear();

		}


		for (size_t i = previousClusters.size(); i > 0; --i)
		{
			if (previousClusters.size() != bestMatches.size())
			{
				std::cout << "Wrong sizes" << std::endl;
			}
			std::string nameP = previousClustersNames.back();
			previousClustersNames.pop_back();
			int bm = bestMatches.back();
			bestMatches.pop_back();
			if (bm == -1)
				continue;
			std::string nameC = currentClustersNames[bm];
			//::string nameC = "";
			//std::shared_ptr<sequence::Cluster> clu0("Z:\\WS2\\wynikitestukompresji\\192.168.201.7.0.0.clu");
			//std::string namePrevious = "Z:\\WS2\\wynikitestukompresji\\192.168.201.7.0."; 
			/*char ichar = boost::lexical_cast<int> (i);
			//std::string namePrevious2 = namePrevious + ichar + ".clu";
			std::string nameCurrent = "Z:\\WS2\\wynikitestukompresji\\192.168.201.7.1.";
			std::string nameCurrent2 = namePrevious + "cos"; //TUTAJ ZDEJMIJ Z VECTORA BEST MATCHES NAJLEPSZE DOPASOWANIE BESTMATCHES*/

			//std::shared_ptr<sequence::Cluster> clu0 = std::make_shared<sequence::Cluster>("Z:\\WS2\\wynikitestukompresji\\192.168.201.7.0.0.clu");
			std::shared_ptr<sequence::Cluster> clu0 = std::make_shared<sequence::Cluster>(nameP);
			//std::shared_ptr<sequence::Cluster> clu1 = std::make_shared<sequence::Cluster>("Z:\\WS2\\wynikitestukompresji\\192.168.201.7.1.0.clu"); //TUTAJ WEZ TE DWA Z GORY
			std::shared_ptr<sequence::Cluster> clu1 = std::make_shared<sequence::Cluster>(nameC);
			int a = clu0->getClusterId();
			cv::Size b = clu0->getFrameSize();
			auto c = clu0->getStartFrame(); //std::map<size_t, cv::Point> 
			size_t d = clu0->getEndFrame();
			auto e = clu0->getFramesPositions();
			int f = e.size();
			std::vector<std::string> pathnames = clu0->getPathNames();
			std::shared_ptr<sequence::Cluster> newShiftedClu25 = clusterglueshift(clu0, clu1, 25);
			//newShiftedClu25->save("Z:\\WS2\\wynikitestukompresji\\SecondWindowFolder\\192.168.201.7.0.0.clu");
			//ZROB TEZ PRZESUNIECIE O 50 I ZAPISZ
		}

	}
	utils::FileLock fileLock(outFile + ".loc");
	if (!fileLock.lock())
	{
		std::cerr << "PathAnalysis::save() can't lock file " << outFile << ".loc" << std::endl;
	}
	else
	{
		cluster.save(outFile);
	}
}

void clustering::PathAnalyzer::cleanup()
{
	try
	{
		if (utils::Filesystem::exists(getInFileName()))
			boost::filesystem::remove(getInFileName());
	}
	catch (...)
	{
		std::cerr << "clustering::PathAnalyzer::cleanup(): Can't remove file \"" << getInFileName() << "\"" << std::endl;
	}
}

void clustering::PathAnalyzer::waitIdle()
{
	// FIX na zbyt wolne odswiezanie systemu plikow
	if (m_MinProcessTime > 0)
	{
		auto elapsedTime = m_Timer.elapsed().wall / 1000;
		if (elapsedTime < m_MinProcessTime * 1000)
		{
			long long leftTime = m_MinProcessTime * 1000 - elapsedTime;
			boost::this_thread::sleep_for(boost::chrono::microseconds(leftTime));
		}
		m_Timer.start();
	}
}
