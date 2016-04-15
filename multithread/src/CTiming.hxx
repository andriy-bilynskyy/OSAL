#ifndef CTIMING_H_
#define CTIMING_H_

#include <stdint.h>

class CTiming
{
public:

    static uint32_t getTime();
    static void sleep_mS(uint32_t time);
    static void sleep_uS(uint32_t time);
};

#endif /* CTIMING_H_ */
