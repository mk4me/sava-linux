#pragma once
#ifndef IVisualizer_h__
#define IVisualizer_h__

#include <string>

namespace motion_analysis
{
	class PathClustering;
}

namespace clustering
{
	/// <summary>
	/// interfejs umo¿liwiaj¹cy definiowanie ró¿nych metod wizualizacji pomocnych podczas debugowania modu³u.
	/// </summary>
	class IVisualizer
	{
	public:
		virtual void loadVideo(const std::string& filename) = 0;
		virtual void visualize(motion_analysis::PathClustering& pathClustering) = 0;
		virtual void show() = 0;
	};
}

#endif // IVisualizer_h__