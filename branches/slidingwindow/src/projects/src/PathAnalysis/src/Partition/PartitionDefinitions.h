#pragma once
#ifndef PartitionRandomColor_h__
#define PartitionRandomColor_h__

#include "utils/RandomColor.h"

#include "opencv2/core.hpp"

#include <map>

namespace clustering
{
	namespace partition
	{
		typedef unsigned long long Time;
		typedef int Id;

		typedef std::map<Id, Id> PathClusters;

		typedef std::map<unsigned long long, utils::RandomColor> Palette;
	}
}

#endif // PartitionRandomColor_h__
