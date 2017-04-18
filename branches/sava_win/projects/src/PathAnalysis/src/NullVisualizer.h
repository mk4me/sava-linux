#pragma once
#ifndef NullVisualizer_h__
#define NullVisualizer_h__

#include "IVisualizer.h"

namespace clustering
{
	/// <summary>
	/// klasa u¿ywana gdy wizualizacja ma byæ wy³¹czona. Realizuje interfejs IVisualizer.
	/// </summary>
	/// <seealso cref="IVisualizer" />
	class NullVisualizer : public IVisualizer
	{
	public:
		virtual ~NullVisualizer() { }
		virtual void loadVideo(const std::string& filename) override { }
		virtual void visualize(motion_analysis::PathClustering& pathClustering) override { }
		virtual void show() override { }

	};
}

#endif // NullVisualizer_h__