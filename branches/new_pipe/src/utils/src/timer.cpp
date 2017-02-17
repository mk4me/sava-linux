#include <utils/timer.h>
#ifndef _WIN32
#include <sys/time.h>
#endif

namespace utils{

unsigned getTickCount()
{
#ifdef _WIN32
    return GetTickCount();
#else
    struct timeval tv;
    gettimeofday(&tv, 0);
    return unsigned((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
#endif
}

    void timer::start()
    {
        current = std::make_unique<time_point>(clock::now());
    }
    int timer::sinceStart()
    {
        if (current) {
            auto end = clock::now();
            auto start = *current;
            auto diff = (end-start);
            return diff.count();
        } else {
            throw std::runtime_error("timer was not start");
        }
    }
    double timer::secondsSinceStart()
    {
        return sinceStart() * ((double) clock::period::num / clock::period::den);
    }


    scoped_timer::scoped_timer(const std::string& preMessage, const std::string& postMessage) :
        pre(preMessage), post(postMessage)
    {
        t.start();
    }
    scoped_timer::~scoped_timer() {
        std::cout << pre << t.secondsSinceStart() << post << std::endl;
    }
}