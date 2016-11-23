#include <utils/timer.h>
#ifndef _WIN32
#include <sys/time.h>
#endif

unsigned utils::getTickCount()
{
#ifdef _WIN32
    return GetTickCount();
#else
    struct timeval tv;
    gettimeofday(&tv, 0);
    return unsigned((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
#endif
}