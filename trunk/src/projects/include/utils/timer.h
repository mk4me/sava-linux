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
#include <boost/chrono.hpp>
#include <chrono>

namespace utils {

    struct timer : boost::timer::cpu_timer
    {
        typedef boost::timer::nanosecond_type nanosecond_type;
        typedef nanosecond_type microsecond_type;
        typedef nanosecond_type millisecond_type;

        using time_point = decltype(std::chrono::high_resolution_clock::now());
        //using time_diff = std::chrono::high_resolution_clock::duration;
        using clock = std::chrono::high_resolution_clock;

        //void sleep( millisecond_type ms );
        nanosecond_type getnsTick() const { return elapsed().wall; }
        microsecond_type getusTick() const { return getnsTick() / 1000; }
        millisecond_type getmsTick() const { return getusTick() / 1000; }

        void start();
        int sinceStart();
        double secondsSinceStart();

    private:
        std::unique_ptr<time_point> current;
    };
    typedef boost::shared_ptr<timer> timerPtr;
    typedef boost::shared_ptr<const timer> timerConstPtr;

    class scoped_timer
    {
    public:
        scoped_timer(const std::string& preMessage, const std::string& postMessage);
        virtual ~scoped_timer();
    private:
        timer t;
        std::string pre, post;
    };

    unsigned getTickCount();
}
#endif
