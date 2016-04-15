#ifndef CLOGGER_H_
#define CLOGGER_H_

#include <iostream>
#include <pthread.h>


class CLogger
{
public:

    void setStream(std::ostream & os);
    template<typename T>
    CLogger& operator << (T const & t);

private:

    static std::ostream    * m_pOutStream;
    static pthread_mutex_t   m_mutex;
};

std::ostream    * CLogger::m_pOutStream = 0;
pthread_mutex_t   CLogger::m_mutex      = PTHREAD_MUTEX_INITIALIZER;

void CLogger::setStream(std::ostream & os)
{
    pthread_mutex_lock(&m_mutex);
    m_pOutStream = &os;
    pthread_mutex_unlock(&m_mutex);
}

template<typename T>
CLogger& CLogger::operator << (T const & t)
{
    pthread_mutex_lock(&m_mutex);
    if(m_pOutStream)
    {
        (*m_pOutStream) << t;
        m_pOutStream->flush();
    }
    pthread_mutex_unlock(&m_mutex);
    return *this;
}


#endif /* CLOGGER_H_ */
