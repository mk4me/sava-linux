/********************************************************************
	created:	2012/07/31
	created:	31:7:2012   11:26
	filename: 	timer.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_UTILS__TIMER_H__
#define HEADER_GUARD_UTILS__TIMER_H__

#include <boost/timer/timer.hpp>
#include <boost/shared_ptr.hpp>

namespace utils {

    struct timer : boost::timer::cpu_timer
    {
        typedef boost::timer::nanosecond_type nanosecond_type;
        typedef nanosecond_type microsecond_type;
        typedef nanosecond_type millisecond_type;

        void sleep( millisecond_type ms );
        nanosecond_type getnsTick() const { return elapsed().wall; }
        microsecond_type getusTick() const { return getnsTick() / 1000; }
        millisecond_type getmsTick() const { return getusTick() / 1000; }
    };
    typedef boost::shared_ptr<timer> timerPtr;
    typedef boost::shared_ptr<const timer> timerConstPtr;

    unsigned getTickCount();
}
#endif
