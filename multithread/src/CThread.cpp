#include "CThread.hxx"
#include <signal.h>
#include <time.h>
#include <errno.h>

#ifdef THREAD_DBG
#include <iostream>
#include <string.h>
#endif


CThreadBase::CThreadBase(std::string name, uint16_t msgSise, uint16_t msgCnt) :
             m_mutex(), m_thread(), m_queue(), m_queueCreated(false), m_name(name)
{
    int mtxRes = pthread_mutex_init(&m_mutex, NULL);
    if(mtxRes)
    {
#ifdef THREAD_DBG
        std::cerr << "Failed to create thread mutex: " << m_name << " : " << this << " - "
                  << strerror(mtxRes) << std::endl;
#endif
    }
    else
    {
        pthread_mutex_lock(&m_mutex);
        if(msgSise && msgCnt)
        {
            struct mq_attr attr;
            attr.mq_flags   = 0;
            attr.mq_maxmsg  = msgCnt;
            attr.mq_msgsize = msgSise;
            attr.mq_curmsgs = 0;
            m_queue = mq_open(m_name.c_str(), O_CREAT | O_EXCL | O_RDONLY, 0600, &attr);
            if(-1 == m_queue)
            {
#ifdef THREAD_DBG
                std::cerr << "Failed to create thread queue: " << m_name << " : " << this << " - "
                          << strerror(errno) << std::endl;
#endif
            }
            else
            {
                m_queueCreated = true;

            }
        }
        pthread_mutex_unlock(&m_mutex);
    }
}

CThreadBase::~CThreadBase()
{
    pthread_mutex_lock(&m_mutex);
    if(!pthread_kill(m_thread, 0))
    {
        int thrRes = pthread_cancel(m_thread);
        if(thrRes)
        {
#ifdef THREAD_DBG
            std::cerr << "Failed to destroy thread: " << m_name << " : " << this << " - "
                      << strerror(thrRes) << std::endl;
#endif
        }
    }
    if(m_queueCreated)
    {
        if(-1 == mq_close(m_queue))
        {
#ifdef THREAD_DBG
            std::cerr << "Failed to destroy thread queue: " << m_name << " : " << this << " - "
                      << strerror(errno) << std::endl;
#endif
        }
        if(mq_unlink(m_name.c_str()))
        {
#ifdef THREAD_DBG
            std::cerr << "Failed to remove thread queue: " << m_name << " : " << this << " - "
                      << strerror(errno) << std::endl;
#endif
        }
    }
    pthread_mutex_unlock(&m_mutex);
    int mtxRes = pthread_mutex_destroy(&m_mutex);
    if(mtxRes)
    {
#ifdef THREAD_DBG
        std::cerr << "Failed to destroy thread mutex: " << m_name << " : " << this << " - "
                  << strerror(mtxRes) << std::endl;
#endif
    }
}

void CThreadBase::wrapperThreadFunction(CThreadBase * obj)
{
    obj->threadFunction();
}

bool CThreadBase::isAlive()
{
    bool ret = false;

    if(!pthread_kill(m_thread, 0))
    {
        ret = true;
    }
    return ret;
}

void CThreadBase::join()
{
    int thrRes = pthread_join(m_thread, NULL);
    if(thrRes)
    {
#ifdef THREAD_DBG
        std::cerr << "Failed to join thread: " << m_name << " : " << this << " - "
                  << strerror(thrRes) << std::endl;
#endif
    }
}

std::string CThreadBase::name()
{
    return m_name;
}

bool
CThreadBase::writeMessage(std::string name, uint8_t buf[], uint16_t size, uint32_t timeout) const
{
    bool ret = false;
    mqd_t queue = mq_open(name.c_str(), O_WRONLY);
    if(-1 == queue)
    {
        if(ENOENT != errno)
        {
#ifdef THREAD_DBG
            std::cerr << "Failed to create queue to send: " << m_name << " : " << this << " - "
                      << strerror(errno) << " receiver: " << name << std::endl;
#endif
        }
    }
    else
    {
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
                result = mq_timedsend(queue, (char *)buf, size, 0, &ts);
            }while(-1 == result && EINTR == errno);
            if(!result)
            {
                ret = true;
            }
            else
            {
#ifdef THREAD_DBG
                std::cerr << "Failed to send queue: " << m_name << " : " << this << " - "
                          << strerror(errno) << " receiver: " << name << std::endl;
#endif
            }
        }
        if(-1 == mq_close(queue))
        {
#ifdef THREAD_DBG
            std::cerr << "Failed to destroy queue to send: " << m_name << " : " << this << " - "
                      << strerror(errno) << " receiver: " << name << std::endl;
#endif
        }
    }
    return ret;
}

int16_t CThreadBase::readMessage(uint8_t buf[], uint16_t size, uint32_t timeout)
{
    int16_t ret = -1;
    pthread_mutex_lock(&m_mutex);
    if(m_queueCreated)
    {
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
            do
            {
                ret = mq_timedreceive(m_queue, (char *)buf, size, 0, &ts);
            }while(-1 == ret && EINTR == errno);
            if(-1 == ret && ETIMEDOUT != errno)
            {
#ifdef THREAD_DBG
                std::cerr << "Failed to receive thread queue: " << m_name << " : " << this << " - "
                          << strerror(errno) << std::endl;
#endif
            }
        }
    }
    pthread_mutex_unlock(&m_mutex);
    return ret;
}

void CThreadBase::run()
{
    pthread_mutex_lock(&m_mutex);
    int thrRes = pthread_create(&m_thread, NULL, (void* (*)(void*))wrapperThreadFunction, this);
    if(thrRes)
    {
#ifdef THREAD_DBG
        std::cerr << "Failed to create thread: " << m_name << " : " << this << " - "
                  << strerror(thrRes) << std::endl;
#endif
    }
    pthread_mutex_unlock(&m_mutex);
}
