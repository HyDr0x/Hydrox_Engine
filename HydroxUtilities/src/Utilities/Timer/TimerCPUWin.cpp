#ifdef _WIN32

#include "Utilities/Timer/TimerCPUWin.h"

#ifndef _WINDOWS_
#include <windows.h>
#endif

namespace he
{
  namespace util
  {
    CPUTimer::CPUTimer() : m_cpuTime(0)
    {
    }

    CPUTimer::~CPUTimer()
    {
    }

    time CPUTimer::getTimeDifference()
    {
      LARGE_INTEGER newTime;
      QueryPerformanceCounter(&newTime);
      m_cpuTime = time(double(newTime.QuadPart - m_cpuTime) / m_cpuFrequency);

      return m_cpuTime;
    }

    void CPUTimer::start()
    {
      LARGE_INTEGER tmpTime;
      QueryPerformanceFrequency(&tmpTime);
      m_cpuFrequency = double(tmpTime.QuadPart) / 1000000.0;//to get micro seconds

      QueryPerformanceCounter(&tmpTime);
      m_cpuTime = tmpTime.QuadPart;
    }

    void CPUTimer::stop()
    {
      LARGE_INTEGER newTime;
      QueryPerformanceCounter(&newTime);
      m_cpuTime = time(double(newTime.QuadPart - m_cpuTime) / m_cpuFrequency);
    }

    time CPUTimer::getTime()
    {
      return m_cpuTime;
    }
  }
}

#endif