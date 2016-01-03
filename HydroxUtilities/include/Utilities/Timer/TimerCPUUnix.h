#ifndef TIMERCPUUNIX_H_
#define TIMERCPUUNIX_H_

#include <iostream>
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
      ~CPUTimer();

      time getTimeDifference();

      void start();
      void stop();

      time getTime();

    private:

      time m_cpuTime;
    };
  }
}

#endif
