#include "CSemaphore.hxx"
#include <time.h>
#include <errno.h>

#ifdef THREAD_DBG
#include <iostream>
#endif

CSemaphore::CSemaphore() : m_semaphore()
{
    if(sem_init(&m_semaphore, 0, 1u))
    {
#ifdef THREAD_DBG
        std::cerr << "Failed to create semaphore: " << this << std::endl;
#endif
    }
}

CSemaphore::CSemaphore(uint32_t value) : m_semaphore()
{
    if(sem_init(&m_semaphore, 0, value))
    {
#ifdef THREAD_DBG
        std::cerr << "Failed to create semaphore: " << this << std::endl;
#endif
    }
}

CSemaphore::~CSemaphore()
{
    if(sem_destroy(&m_semaphore))
    {
#ifdef THREAD_DBG
        std::cerr << "Failed to destroy semaphore: " << this << std::endl;
#endif
    }
}

void CSemaphore::post()
{
    if(sem_post(&m_semaphore))
    {
#ifdef THREAD_DBG
        std::cerr << "Failed to post semaphore: " << this << std::endl;
#endif
    }
}

void CSemaphore::pend()
{
    int result = 0;
    do
    {
        result = sem_wait(&m_semaphore);
    }while(result && EINTR == errno);
    if(result)
    {
#ifdef THREAD_DBG
        std::cerr << "Failed to pend semaphore: " << this << std::endl;
#endif
    }
}

bool CSemaphore::pendWait(uint32_t timeout)
{
    bool ret = false;
    struct timespec ts;

    if(clock_gettime(CLOCK_REALTIME, &ts))
    {
#ifdef THREAD_DBG
        std::cerr << "Failed get time" << std::endl;
#endif
    }
    else
    {
        ts.tv_sec += timeout/1000u;
        ts.tv_nsec += 1000000uL * (timeout%1000u);
        if(ts.tv_nsec >= 1000000000L)
        {
            ts.tv_nsec -= 1000000000L;
            ts.tv_sec ++;
        }
        int result = 0;
        do
        {
            result = sem_timedwait(&m_semaphore, &ts);
        }while(result && EINTR == errno);
        if(!result)
        {
            ret = true;
        }
        else if (ETIMEDOUT != errno)
        {
#ifdef THREAD_DBG
            std::cerr << "Failed to pend semaphore: " << this << std::endl;
#endif
        }
    }

    return ret;
}

