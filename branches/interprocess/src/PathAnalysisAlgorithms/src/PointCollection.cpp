#include <cassert>
#include "motionanalysis/algorithms/PointCollection.h"
#include "motionanalysis/algorithms/auxmath.h"

BOOST_CLASS_EXPORT_IMPLEMENT(motion_analysis::PointCollection);

using namespace motion_analysis;

/// <summary>
/// See declaration for all the details.
/// </summary>
void motion_analysis::PointCollection::forwardBackwardFilter( float alpha, bool doForward /*= true */ )
{
    assert( points.empty() == false );

    if( points.size() == 1 ) {
        auto p = points.rbegin();
        p->tau_x = p->x; // eq. 11.x
        p->tau_y = p->y; // eq. 11.y

        p->tau_x_fov = p->x; // eq. 13.x
        p->tau_y_fov = p->y; // eq. 13.y
		
		p->dx = 0.0f;
		p->dy = 0.0f;
		p->dx_fov = 0.0f;
		p->dy_fov = 0.0f;

    } else {
		float weight = 0.1f;// std::max(1.0f / (float)(points.size() - 1), 1.0f / 10.0f);
		if (doForward)
		{
			// forward update (all elements)
			auto p_prev = points.begin();
			p_prev->tau_x_fov = p_prev->x; // eq. 13.x
			p_prev->tau_y_fov = p_prev->y; // eq. 13.y

			p_prev->dx_fov = 0;
			p_prev->dy_fov = 0;

			for (auto p = std::next(p_prev); p != points.end(); ++p, ++p_prev) {
				p->tau_x_fov = p_prev->tau_x_fov + alpha * (p->x - p_prev->tau_x_fov); // eq. 12.x
				p->tau_y_fov = p_prev->tau_y_fov + alpha * (p->y - p_prev->tau_y_fov); // eq. 12.y

				p->dx_fov = (p->x - p_prev->x) * weight + p_prev->dx_fov * (1.0f - weight);
				p->dy_fov = (p->y - p_prev->y) * weight + p_prev->dy_fov * (1.0f - weight);
			}
		}
		else
		{
			auto p = points.rbegin();
			auto p_prev = std::next(p);
			p->tau_x_fov = p_prev->tau_x_fov + alpha * (p->x - p_prev->tau_x_fov); // eq. 12.x
			p->tau_y_fov = p_prev->tau_y_fov + alpha * (p->y - p_prev->tau_y_fov); // eq. 12.y

			p->dx_fov = (p->x - p_prev->x) * weight + p_prev->dx_fov * (1.0f - weight);
			p->dy_fov = (p->y - p_prev->y) * weight + p_prev->dy_fov * (1.0f - weight);
		}

        // backward update (all elements), values temporarily placed in tau
        auto p_next = points.rbegin();
        p_next->tau_x = p_next->x; // eq. 11.x
        p_next->tau_y = p_next->y; // eq. 11.y

		p_next->dx = 0.0f;
		p_next->dy = 0.0f;

        for( auto p = std::next( p_next ); p != points.rend(); ++p, ++p_next ) {
            p->tau_x = p_next->tau_x + alpha * ( p->x - p_next->tau_x ); // eq. 10.x
            p->tau_y = p_next->tau_y + alpha * ( p->y - p_next->tau_y ); // eq. 10.y

			p->dx = (p_next->x - p->x) * weight + p_next->dx * (1.0f - weight);
			p->dy = (p_next->y - p->y) * weight + p_next->dy * (1.0f - weight);
        }

        // tau update (all elements)
        for( auto p = points.rbegin(); p != points.rend(); ++p ) {
            p->tau_x = p->tau_x - p->tau_x_fov; // eq. 9.x
            p->tau_y = p->tau_y - p->tau_y_fov; // eq. 9.y

			p->dx = (p->dx + p->dx_fov) * 0.5f;
			p->dy = (p->dy + p->dy_fov) * 0.5f;

			p->tau_x = p->dx;
			p->tau_y = p->dy;
        }
    }
}

/// <summary>
/// See declaration for all the details.
/// </summary>
bool motion_analysis::PointCollection::checkCorrectness()
{
    if( points.size() < 2 ) {
        return true;
    }

    auto p = points.begin();
    auto p_next = std::next( p );

    for( ; p_next != points.end(); ++p, ++p_next ) {
        
	//	if ( isnan(p->x) || isnan(p->y) || isnan(p->t) ) {
	//		return false;
	//	}
		
		if( p->t >= p_next->t ) {
            return false;
        }
    }

    return true;
}

