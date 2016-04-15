#include "CTiming.hxx"
#include <time.h>
#include <unistd.h>

#ifdef THREAD_DBG
#include <iostream>
#endif

uint32_t CTiming::getTime()
{
    struct timespec ts;
    if(clock_gettime(CLOCK_REALTIME, &ts))
    {
#ifdef THREAD_DBG
        std::cerr << "Failed get time" << std::endl;
#endif
    }
    return ts.tv_sec * 1000u + ts.tv_nsec/1000000u;
}

void CTiming::sleep_mS(uint32_t time)
{
    if(usleep(time * 1000u))
    {
#ifdef THREAD_DBG
        std::cerr << "Failed to sleep" << std::endl;
#endif
    }
}

void CTiming::sleep_uS(uint32_t time)
{
    if(usleep(time))
    {
#ifdef THREAD_DBG
        std::cerr << "Failed to sleep" << std::endl;
#endif
    }
}

