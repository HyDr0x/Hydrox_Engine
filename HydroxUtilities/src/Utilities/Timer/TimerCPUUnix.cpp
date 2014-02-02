#include "Utilities/Timer/TimerCPUUnix.h"

namespace he
{
	namespace util
	{
    CPUTimerUnix::CPUTimerUnix(std::string &timerName) : m_timerName(timerName), m_showTimerWhenDying(true)
    {
      m_cpuTime = clock();
    }

    CPUTimerUnix::~CPUTimerUnix()
    {
      if(m_showTimerWhenDying)
      {
        m_cpuTime = clock() - m_cpuTime;
        m_cpuTime /= (CLOCKS_PER_SEC * 0.001f);

        std::cout << m_timerName << std::endl;
        std::cout << "CPU Time: " << m_cpuTime << " ms" << std::endl;
      }
    }

    time CPUTimerUnix::getTimeDifference()
    {
      m_cpuTime = clock() - m_cpuTime;
      m_cpuTime /= (CLOCKS_PER_SEC * 0.001f);

      m_showTimerWhenDying = false;

      return m_cpuTime;
    }
	}
}