#include "Utilities/Timer/TimerCPUWin.h"

namespace he
{
	namespace util
	{
    CPUTimerWin::CPUTimerWin(std::string &timerName) : m_timerName(timerName), m_showTimerWhenDying(true)
    {
      LARGE_INTEGER tmpTime;
      QueryPerformanceFrequency(&tmpTime);
      m_cpuFrequency = double(tmpTime.QuadPart) / 1000000.0;//to get micro seconds

      QueryPerformanceCounter(&tmpTime);
      m_cpuTime = tmpTime.QuadPart;
    }

    CPUTimerWin::~CPUTimerWin()
    {
      if(m_showTimerWhenDying)
      {
        LARGE_INTEGER newTime;
        QueryPerformanceCounter(&newTime);
        m_cpuTime = double(newTime.QuadPart - m_cpuTime) / m_cpuFrequency; 

        std::cout << m_timerName << std::endl;
        std::cout << "CPU Time: " << m_cpuTime << " m" << MICROSIGN << std::endl;
      }
    }

    time CPUTimerWin::getTimeDifference()
    {
      LARGE_INTEGER newTime;
      QueryPerformanceCounter(&newTime);
      m_cpuTime = double(newTime.QuadPart - m_cpuTime) / m_cpuFrequency; 

      m_showTimerWhenDying = false;

      return m_cpuTime;
    }
	}
}