#ifndef CPUTIMERUNIX_H_
#define CPUTIMERUNIX_H_

#include <iostream>
#include <string>
#include <time.h>

#include "Hydrox/DLLExport.h"

namespace he
{
  typedef clock_t time;

  class GRAPHICAPI CPUTimerUnix
  {
  public:

    CPUTimerUnix(std::string &timerName);
    ~CPUTimerUnix();

    time getTimeDifference();

  private:

    std::string m_timerName;

    time m_cpuTime;

    bool m_showTimerWhenDying;
  };
}

#endif