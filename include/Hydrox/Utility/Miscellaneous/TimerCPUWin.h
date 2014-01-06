#ifndef CPUTIMERWIN_H_
#define CPUTIMERWIN_H_

#include <iostream>
#include <string>
#include <windows.h>

#include "Hydrox/DLLExport.h"

namespace he
{
  typedef __int64 time;

  class GRAPHICAPI CPUTimerWin
  {
  public:

    CPUTimerWin(std::string &timerName);
    ~CPUTimerWin();

    time getTimeDifference();

  private:

    inline void calculateTimeDifference();

    std::string m_timerName;

    time m_cpuTime;
    double m_cpuFrequency;

    bool m_showTimerWhenDying;
  };
}

#endif
