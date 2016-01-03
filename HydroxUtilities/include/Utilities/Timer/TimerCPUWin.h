#ifndef TIMERCPUWIN_H_
#define TIMERCPUWIN_H_

#include <iostream>

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
      ~CPUTimer();

      time getTimeDifference();

      void start();
      void stop();

      time getTime();

    private:

      time m_cpuTime;
      double m_cpuFrequency;
    };
  }
}

#endif
