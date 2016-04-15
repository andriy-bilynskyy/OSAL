#ifndef CTHREAD_H_
#define CTHREAD_H_

#include <pthread.h>
#include <mqueue.h>
#include <stdint.h>
#include <string>


class CThreadBase
{
public:

    CThreadBase(std::string name, uint16_t msgSise, uint16_t msgCnt);
    virtual ~CThreadBase();
    bool isAlive();
    void join();
    std::string name();
    bool writeMessage(std::string name, uint8_t buf[], uint16_t size, uint32_t timeout) const;

private:

    mutable pthread_mutex_t m_mutex;
    pthread_t       m_thread;
    mqd_t           m_queue;
    bool            m_queueCreated;
    std::string     m_name;

    static void wrapperThreadFunction(CThreadBase * obj);

    virtual void threadFunction(void) = 0;

protected:

    int16_t readMessage(uint8_t buf[], uint16_t size, uint32_t timeout);
    void run();
};

#endif /* CTHREAD_H_ */
