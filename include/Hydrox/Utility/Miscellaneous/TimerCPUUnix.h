#ifndef CPUTIMERUNIX_H_
#define CPUTIMERUNIX_H_

#include <iostream>
#include <string>
#include <time.h>

#include "Hydrox/DLLExport.h"

namespace he
{
  class GRAPHICAPI CPUTimerUnix
  {
  public:

    CPUTimerUnix(std::string &timerName);
    ~CPUTimerUnix();

    clock_t getTimeDifference();

  private:

    std::string m_timerName;

    clock_t m_cpuTime;

    bool m_showTimerWhenDying;
  };
}

#endif