#include "CMtx.hxx"
#include <errno.h>

#ifdef THREAD_DBG
#include <iostream>
#endif

CMtx::CMtx() : m_mutex()
{
    if(pthread_mutex_init(&m_mutex, NULL))
    {
#ifdef THREAD_DBG
        std::cerr << "Failed to create mutex: " << this << std::endl;
#endif
    }

}

CMtx::~CMtx()
{
    if(pthread_mutex_destroy(&m_mutex))
    {
#ifdef THREAD_DBG
        std::cerr << "Failed to destroy mutex: " << this << std::endl;
#endif
    }
}

void CMtx::lock()
{
    if(pthread_mutex_lock(&m_mutex))
    {
#ifdef THREAD_DBG
        std::cerr << "Failed to lock mutex: " << this << std::endl;
#endif
    }
}

void CMtx::unlock()
{
    if(pthread_mutex_unlock(&m_mutex))
    {
#ifdef THREAD_DBG
        std::cerr << "Failed to unlock mutex: " << this << std::endl;
#endif
    }
}

bool CMtx::tryLock()
{
    bool ret = false;

    int opResult = pthread_mutex_trylock(&m_mutex);
    if(!opResult)
    {
        ret = true;
    }
    else if(EBUSY != opResult)
    {
#ifdef THREAD_DBG
        std::cerr << "Failed to try lock mutex: " << this << std::endl;
#endif
    }
    return ret;
}

