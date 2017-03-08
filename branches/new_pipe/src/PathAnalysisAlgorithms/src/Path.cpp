#include "PathAnalysisAlgorithms/algorithms/Path.h"

BOOST_CLASS_EXPORT_IMPLEMENT(motion_analysis::Path);

using namespace motion_analysis;

/// <summary>
/// See declaration for all the details.
/// </summary>
void motion_analysis::Path::append( PathPositionType x, PathPositionType y, PathTimeType t, float alpha )
{
	// add point to the collection
    Point pnt(x, y, t, 1);
	//TangentialPoint tan;

    points.push_back( pnt );
	//tangentialPoints.push_back( tan );

    // perform filtering
    forwardBackwardFilter( alpha );

    assert( checkCorrectness() );
}

/// <summary>
/// See declaration for all the details.
/// </summary>
const ::size_t motion_analysis::Path::computeMemoryUsage()
{
	::size_t mem;
	mem = sizeof(Path) + this->points.size() * sizeof(point_type);

	return mem;
}

const ::size_t motion_analysis::Path::calculateHash() const
{
	size_t seed = 0;
	
	for (auto it = this->begin(); it != this->end(); ++it) {
		boost::hash_combine(seed, it->x );
		boost::hash_combine(seed, it->y );
		boost::hash_combine(seed, it->n );
		boost::hash_combine(seed, it->t );
	}

	return seed;
}
