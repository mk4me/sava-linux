#pragma once
#ifndef NullVisualizer_h__
#define NullVisualizer_h__

#include "IVisualizer.h"

namespace clustering
{
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