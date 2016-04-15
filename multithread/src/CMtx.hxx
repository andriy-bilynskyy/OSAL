#ifndef CMTX_H_
#define CMTX_H_

#include <pthread.h>

class CMtx
{
public:

    CMtx();
    ~CMtx();
    void lock();
    void unlock();
    bool tryLock();

private:

    pthread_mutex_t   m_mutex;
};

#endif /* CMTX_H_ */
