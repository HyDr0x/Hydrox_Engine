#ifndef TIMERCPUUNIX_H_
#define TIMERCPUUNIX_H_

#include <iostream>
#include <string>
#include <time.h>

#include "Utilities/DLLExport.h"

#define MICROSIGN unsigned short (0xC2B5) 

namespace he
{
  namespace util
  {
    typedef clock_t time;

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

      bool m_showTimerWhenDying;
    };
  }
}

#endif
