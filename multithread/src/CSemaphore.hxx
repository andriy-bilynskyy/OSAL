#ifndef CSEMAPHORE_H_
#define CSEMAPHORE_H_

#include <semaphore.h>
#include <stdint.h>

class CSemaphore
{
public:

    CSemaphore();
    explicit CSemaphore(uint32_t value);
    ~CSemaphore();
    void post();
    void pend();
    bool pendWait(uint32_t timeout);

private:

    sem_t m_semaphore;
};

#endif /* CSEMAPHORE_H_ */
