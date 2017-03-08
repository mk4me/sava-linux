#pragma once

#include "PathClustering.h"

namespace motion_analysis {

	/// <summary>
	/// Optimised version of PathClustering class.
	/// </summary>
	class FastPathClustering : public PathClustering
	{
	public:

	protected:
		/// Performs assignment of newly introduced paths. Uses OpenMP.
		virtual void doAssignment() override;

	private:
		friend class boost::serialization::access;

		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{	
			ar & SERIALIZATION_BASE_OBJECT_NVP(PathClustering);
		}
	};
}

BOOST_CLASS_EXPORT_KEY(motion_analysis::FastPathClustering);