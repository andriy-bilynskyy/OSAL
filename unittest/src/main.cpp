#include <sstream>
#include <cstdlib>
#include "CThread.hxx"
#include "CLogger.hxx"
#include "CMtx.hxx"
#include "CTiming.hxx"
#include "CSemaphore.hxx"


class Sensor : public CThreadBase
{
public:

    Sensor(std::string name, uint16_t msgSise, uint16_t msgCnt) : CThreadBase(name, msgSise, msgCnt)
    {
        run();
    }

public:

    void threadFunction(void)
    {
        for(uint16_t i = 0; i < 100; i++)
        {
            std::stringstream ss;
            int sensorValue = std::rand() % 1000;
            ss << name() << " data is " << sensorValue << " : " <<
                         writeMessage("/actuator",
                                      reinterpret_cast<uint8_t*>(&sensorValue), sizeof(sensorValue),
                                      100u)
                                                                                       << std::endl;
            CLogger() << ss.str();
            CTiming::sleep_mS(100);

        }
    }
};

class Actuator : public CThreadBase
{
public:

    Actuator(std::string name, uint16_t msgSise, uint16_t msgCnt) :
                                                                  CThreadBase(name, msgSise, msgCnt)
    {
        run();
    }

public:

    void threadFunction(void)
    {
        bool iterate = true;
        for(;iterate;)
        {
            int sensorValue = 0;
            int16_t size = 0;
            size = readMessage(reinterpret_cast<uint8_t*>(&sensorValue), sizeof(sensorValue), 1000);
            std::stringstream ss;
            if(0 > size)
            {
                ss << name() << " : data timeout" << std::endl;
                iterate = false;
            }
            else
            {
                if(sizeof(int) == size)
                {
                    ss << name() << " : received " <<  sensorValue << std::endl;
                }
                else
                {
                    ss << name() << " : received strange message!" << std::endl;
                }
            }
            CLogger() << ss.str();
        }
    }
};



int main()
{
    //mq_unlink("/sensor");
    //mq_unlink("/actuator");

    CLogger().setStream(std::cout);

    Sensor      sen("/sensor", 0, 0);
    Actuator    act("/actuator", sizeof(int), 10);

    sen.join();
    act.join();

    return 0;
}



