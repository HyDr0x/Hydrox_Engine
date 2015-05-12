#ifndef TIMERCPUWIN_H_
#define TIMERCPUWIN_H_

#include <iostream>
#include <string>

#include "Utilities/DLLExport.h"

#define MICROSIGN unsigned char (230)

namespace he
{
  namespace util
  {
    typedef __int64 time;

    class GRAPHICAPI CPUTimer
    {
    public:

      CPUTimer();
      CPUTimer(std::string &timerName);
      ~CPUTimer();

      time getTimeDifference();

      void start();
      void stop();

      time getTime();

    private:

      std::string m_timerName;

      time m_cpuTime;
      double m_cpuFrequency;

      bool m_showTimerWhenDying;
    };
  }
}

#endif
