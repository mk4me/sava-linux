#include <cassert>
#include "motionanalysis/algorithms/PathCluster.h"
#include "motionanalysis/algorithms/auxmath.h"


BOOST_CLASS_EXPORT_IMPLEMENT(motion_analysis::PathCluster);

using namespace motion_analysis;

int motion_analysis::PathCluster::currentId = 0;

/// <summary>
/// See declaration for all the details.
/// </summary>
motion_analysis::PathCluster::PathCluster( std::shared_ptr<Path> & path ) : PointCollection( path->category ), bk( 1 ), current_bk(1), b( 0,0 ), bb( 0,0 ),  id(currentId++)
{
	assert( path->pointsCount() > 0 );

    this->elements.push_back( path );
    this->points = path->points;
//	this->tangentialPoints = path->tangentialPoints;

    assert( path->pointsCount() == this->pointsCount() );
    assert( path->rbegin()->t == this->rbegin()->t );
    assert( checkCorrectness() );
}

/// <summary>
/// See declaration for all the details.
/// </summary>
motion_analysis::PathCluster::PathCluster( const PathCluster& ref ) : PointCollection( ref ),  id(currentId++)
{
    this->b = ref.b;
    this->bb = ref.bb;
    this->bk = ref.bk;
	this->current_bk = ref.bk;
}

/// <summary>
/// See declaration for all the details.
/// </summary>
Point motion_analysis::PathCluster::getMean() const
{
	Point globalMean;
	
	// iterate over object paths
	for (auto p = elements.begin(); p != elements.end(); ++p) {
		float w = static_cast<float>((**p).pointsCount());
		Point mean = (**p).getMean();

		globalMean.x += w * mean.x;
		globalMean.y += w * mean.y;
		globalMean.t += static_cast<PathTimeType>(w * mean.t);
	}

	globalMean.x /= current_bk;
	globalMean.y /= current_bk;
	globalMean.t /= current_bk;

	return globalMean;
}

/// <summary>
/// See declaration for all the details.
/// </summary>
void motion_analysis::PathCluster::update( const std::shared_ptr<const Path> & path, const PathClusteringParams& params )
{
    assert( path->pointsCount() > 0 );
    assert( this->pointsCount() > 0 );
    assert( std::find( this->elements.begin(), this->elements.end(), path ) != this->elements.end() );

    const Point &p = *(path->rbegin());

    // check if cluster have been already extended in this frame
    if( path->rbegin()->t == this->rbegin()->t ) {
        // if yes - update last cluster point
        Point &z = *(this->rbegin());

        z.x = ( z.x * z.n + p.x - path->c.x ) / ( z.n + 1 );	// eq. 1.x
        z.y = ( z.y * z.n + p.y - path->c.y ) / ( z.n + 1 );	// eq. 1.y
        z.n = z.n + 1;										// eq. 2

    } else if( path->rbegin()->t > this->rbegin()->t ) { // check if cluster should be extended
        Point z( p.x - path->c.x, p.y - path->c.y, p.t, 1); // eq. 3, 4, 5, 6
		//TangentialPoint r;

        this->points.push_back( z );
		//this->tangentialPoints.push_back( r );
        
		assert( path->rbegin()->t == this->rbegin()->t );
    }

    forwardBackwardFilter( params.alpha, PathCluster::do_forward_filter_in_update );
    assert( checkCorrectness() );
}

/// <summary>
/// See declaration for all the details.
/// </summary>
void motion_analysis::PathCluster::assign( std::shared_ptr<Path> & path, const PathDistance& distance, const PathClusteringParams& params )
{
    assert( path->pointsCount() > 0 );
    assert( this->pointsCount() > 0 );
    assert( distance.length > 0 ); // only paths with non-zero overlap can be assigned to cluster

    elements.push_back( path );
    path->c = distance.dispertion; // eq. 19 and 20

    auto old_b = b;

    // update mean displacement
    b.x = ( b.x * bk + path->c.x ) / ( bk + 1 ); // eq. 23
    b.y = ( b.y * bk + path->c.y ) / ( bk + 1 ); // eq. 24

    // update variance
    bb.x = ( ( bb.x + sqr( old_b.x ) ) * bk + sqr( distance.dispertion.x ) ) / ( bk + 1 ) - sqr( b.x ); // eq. 25
    bb.y = ( ( bb.y + sqr( old_b.y ) ) * bk + sqr( distance.dispertion.y ) ) / ( bk + 1 ) - sqr( b.y ); // eq. 26

    auto &Z = this->points; // current cluster
    auto &P = path->points; // path to be assigned
   
	auto z = Z.begin();
    auto p = P.begin();

    // update cluster points and insert new ones when needed
    for( ; z != Z.end() && p != P.end(); ++z ) {
        while( p != P.end() && p->t < z->t ) {	// iterate over path points not present in cluster
            // add path point to the cluster
            Point pnt (p->x - path->c.x, p->y - path->c.y, p->t, 1); // eq. 28 
            z = Z.insert( z, pnt );
            ++p;
        }

        if( p != P.end() && ( p->t == z->t ) ) { // if there is an overlap at current point
            
			// added for motion visualisation aims (focused view)
			cv::Point_<PathPositionType> new_z;
			new_z.x = ( z->x * z->n + p->x - path->c.x ) / ( z->n + 1 );	// eq. 21.x
			new_z.y = ( z->y * z->n + p->y - path->c.y ) / ( z->n + 1 );	// eq. 21.y
			
			if (max_b.x < abs(z->x - p->x)) { max_b.x = abs(z->x - p->x); } 
			if (max_b.y < abs(z->y - p->y)) { max_b.y = abs(z->y - p->y); }
			
			// update cluster points
			z->x = new_z.x;
            z->y = new_z.y;

            ++( z->n );	// eq. 22
            ++p;
        }
    }

    ++( this->bk );
	++( this->current_bk );

    forwardBackwardFilter( params.alpha );
    assert( checkCorrectness() );
}

/// <summary>
/// See declaration for all the details.
/// </summary>
std::shared_ptr<PathCluster> motion_analysis::PathCluster::tryAssign( const Path& path, const cv::Point_<PathPositionType>& dispertion ) const {
    // make shallow copy of cluster
    auto out = std::shared_ptr<PathCluster>( new PathCluster( *this ) );

    // new values of mean displacement
    out->b.x = ( b.x * bk + dispertion.x ) / ( bk + 1 ); // eq. 23 (path.c.x is equal to dispertion.x)
    out->b.y = ( b.y * bk + dispertion.y ) / ( bk + 1 ); // eq. 24 (path.c.y is equal to dispertion.y)

    // new values of variance
    out->bb.x = ( ( bb.x + sqr( b.x ) ) * bk + sqr( dispertion.x ) ) / ( bk + 1 ) - sqr( out->b.x ); // eq. 25
    out->bb.y = ( ( bb.y + sqr( b.y ) ) * bk + sqr( dispertion.y ) ) / ( bk + 1 ) - sqr( out->b.y ); // eq. 26

    return out;
}

/// <summary>
/// See declaration for all the details.
/// </summary>
void motion_analysis::PathCluster::merge( const PathCluster& cluster, const PathDistance& distance, const PathClusteringParams& params )
{
	addMergedGaps(cluster.mergedGaps);

	auto &Z = this->points;	// current cluster
    auto &P = cluster.points; // other cluster
	
    auto z = Z.begin();
    auto p = P.begin();
	
    // reassign paths to current cluster
    this->elements.insert( this->elements.end(), cluster.elements.begin(), cluster.elements.end() );

    // update current points and insert new ones when needed
    for( ; z != Z.end() && p != P.end(); ++z) {
        while( p != P.end() && p->t < z->t ) {	// iterate over other cluster points not present in this cluster
            // add path point to the cluster
            Point pnt (p->x + cluster.b.x, p->y + cluster.b.y, p->t, p->n); // eq. 39, 40, 41 
            z = Z.insert( z, pnt );
            ++p;
        }

        if( p != P.end() && ( p->t == z->t ) ) { // if there is an overlap at current point
            z->x = ( ( z->x + this->b.x ) * z->n + ( p->x + cluster.b.x ) * p->n ) / ( z->n + p->n );	// eq. 39
            z->y = ( ( z->y + this->b.y ) * z->n + ( p->y + cluster.b.y ) * p->n ) / ( z->n + p->n );	// eq. 40
            z->n = z->n + p->n; // eq. 41
            ++p;

        } else {
            z->x = z->x + this->b.x;
            z->y = z->y + this->b.y;
        }
    }

    // weighted sum of all components
    this->bb.x = ( ( this->bb.x + sqr( this->b.x ) ) * this->bk + ( cluster.bb.x + sqr( cluster.b.x ) ) * cluster.bk ) / ( this->bk + cluster.bk ); // eq. 42
    this->bb.y = ( ( this->bb.y + sqr( this->b.y ) ) * this->bk + ( cluster.bb.y + sqr( cluster.b.y ) ) * cluster.bk ) / ( this->bk + cluster.bk ); // eq. 43
    this->bk = this->bk + cluster.bk; // eq. 44
    this->b.x = 0; // eq. 45
    this->b.y = 0; // eq. 45

	this->current_bk += cluster.current_bk;

    forwardBackwardFilter( params.alpha );

    assert( checkCorrectness() );
}

/// <summary>
/// See declaration for all the details.
/// </summary>
std::shared_ptr<PathCluster> motion_analysis::PathCluster::tryMerge( const PathCluster& cluster ) const {
    // make shallow copy of cluster
    auto out = std::shared_ptr<PathCluster>( new PathCluster( *this ) );

    // weighted sum of all components
    out->bb.x = ( ( this->bb.x + sqr( this->b.x ) ) * this->bk + ( cluster.bb.x + sqr( cluster.b.x ) ) * cluster.bk ) / ( this->bk + cluster.bk ); // eq. 42
    out->bb.y = ( ( this->bb.y + sqr( this->b.y ) ) * this->bk + ( cluster.bb.y + sqr( cluster.b.y ) ) * cluster.bk ) / ( this->bk + cluster.bk ); // eq. 43
    out->bk = this->bk + cluster.bk; // eq. 44
    out->b.x = 0; // eq. 45
    out->b.y = 0; // eq. 45

    return out;
}

/// <summary>
/// See declaration for all the details.
/// </summary>
bool motion_analysis::PathCluster::markPathEnd( const std::shared_ptr<Path> & path )
{
	path->removed = true;
	--current_bk;
	return (current_bk == 0);
}

/// <summary>
/// See declaration for all the details.
/// </summary>
bool motion_analysis::PathCluster::removePath( const std::shared_ptr<Path> & path )
{
	assert( std::find( this->elements.begin(), this->elements.end(), path ) != this->elements.end() );
	
	--current_bk;
	this->elements.remove( path );
	return ( elements.empty() == true );
}

const ::size_t motion_analysis::PathCluster::computeMemoryUsage()
{
	::size_t mem;
	mem = sizeof(PathCluster) + points.size() * sizeof(point_type);

	for (auto e = elements.begin(); e != elements.end(); ++e) {
		mem += (**e).computeMemoryUsage();
	}

	return mem;
}

const ::size_t motion_analysis::PathCluster::calculateHash() const
{
	size_t seed = 0;
	for (auto path = elements.begin(); path != elements.end(); ++path) {
		boost::hash_combine(seed, (**path).calculateHash() );
	}
	
	return seed; 
}

std::shared_ptr<PathCluster> motion_analysis::PathCluster::enlarge(PathTimeType pointsNum, bool front /*= false*/, bool back /*= true*/) const
{
	// make shallow copy of cluster
	auto out = std::shared_ptr<PathCluster>(new PathCluster(*this));

	auto &Z = out->points;
	auto ze = *Z.rbegin();
	auto zb = *Z.begin();
	
	if(front)
	{
		Z.resize(Z.size() + pointsNum);
		std::rotate(Z.rbegin(), Z.rbegin() + pointsNum, Z.rend());

		for (PathTimeType i = 1; i <= pointsNum; ++i)
		{
			Point p(zb.x - zb.dx * i, zb.y - zb.dy * i, zb.t - i, 0);
			p.tau_x = zb.tau_x;
			p.tau_y = zb.tau_y;
			p.dx = zb.dx;
			p.dy = zb.dy;
			Z[pointsNum - i] = p;
		}
	}

	if(back)
	{
		for (PathTimeType i = 1; i <= pointsNum; ++i)
		{
			Point p(ze.x + ze.dx * i, ze.y + ze.dy * i, ze.t + i, 0);
			p.tau_x = zb.tau_x;
			p.tau_y = zb.tau_y;
			p.dx = zb.dx;
			p.dy = zb.dy;
			Z.push_back(p);
		}
	}

	return out;
}

void motion_analysis::PathCluster::addMergedGap(const Point& a, const Point& b)
{
	mergedGaps.push_back(std::make_pair(a, b));
}

void motion_analysis::PathCluster::addMergedGaps(const std::list<std::pair<Point, Point>>& gaps)
{
	mergedGaps.insert(mergedGaps.end(), gaps.begin(), gaps.end());
}

cv::Rect motion_analysis::PathCluster::getContour(PathTimeType time, const cv::Size& margin /*= cv::Size()*/) const
{
	cv::Rect box(-1, -1, 2 * margin.width, 2 * margin.height);

	for (auto& path : elements)
	{
		auto currentPoint = path->rbegin();

		while (currentPoint != path->rend() && currentPoint->t > time)
			++currentPoint;

		if (currentPoint != path->rend() && currentPoint->t == time)
		{
			cv::Point p((int)currentPoint->x, (int)currentPoint->y);
			if (box.x == -1)
			{
				box.x = p.x - margin.width;
				box.y = p.y - margin.height;
			}
			else
			{
				if (box.x > p.x - margin.width) box.x = p.x - margin.width;
				if (box.x + box.width < p.x + margin.width) box.width = p.x + margin.width - box.x;
				if (box.y > p.y - margin.height) box.y = p.y - margin.height;
				if (box.y + box.height < p.y + margin.height) box.height = p.y + margin.height - box.y;
			}
		}
	}
	return box;
}

bool motion_analysis::PathCluster::do_forward_filter_in_update = false;
