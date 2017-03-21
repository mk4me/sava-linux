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

    timer::timer(bool startClock, std::ostream& stream) :
        stream(stream)
    {
        if (startClock) {
            start();
        }

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

    void timer::print(const std::string& preMessage, const std::string& postMessage)
    {
        stream << preMessage << secondsSinceStart() << postMessage << std::endl;
    }


    scoped_timer::scoped_timer(const std::string& preMessage, const std::string& postMessage) :
        pre(preMessage), post(postMessage)
    {
        t.start();
    }
    scoped_timer::~scoped_timer() {
        t.print(pre, post);
    }
}