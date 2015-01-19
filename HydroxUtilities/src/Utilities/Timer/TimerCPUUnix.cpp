#ifndef _WIN32

#include "Utilities/Timer/TimerCPUUnix.h"

namespace he
{
  namespace util
  {
    CPUTimer::CPUTimer() : m_timerName(""), m_showTimerWhenDying(false)
    {
    }

    CPUTimer::CPUTimer(std::string &timerName) : m_timerName(timerName), m_showTimerWhenDying(true)
    {
      m_cpuTime = clock();
    }

    CPUTimer::~CPUTimer()
    {
      if(m_showTimerWhenDying)
      {
        m_cpuTime = clock() - m_cpuTime;
        m_cpuTime /= (CLOCKS_PER_SEC * 0.001f);

        std::cout << m_timerName << std::endl;
        std::cout << "CPU Time: " << m_cpuTime << " ms" << std::endl;
      }
    }

    time CPUTimer::getTimeDifference()
    {
      m_cpuTime = clock() - m_cpuTime;
      m_cpuTime /= (CLOCKS_PER_SEC * 0.001f);

      m_showTimerWhenDying = false;

      return m_cpuTime;
    }

    void CPUTimer::start()
    {
      m_cpuTime = clock();
    }

    void CPUTimer::stop()
    {
      m_cpuTime = clock() - m_cpuTime;
      m_cpuTime /= (CLOCKS_PER_SEC * 0.001f);
    }

    time CPUTimer::getTime()
    {
      return m_cpuTime;
    }
  }
}

#endif