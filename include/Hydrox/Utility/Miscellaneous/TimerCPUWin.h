#ifndef CPUTIMERWIN_H_
#define CPUTIMERWIN_H_

#include <iostream>
#include <string>
#include <windows.h>

#include "Hydrox/DLLExport.h"

namespace he
{
  class GRAPHICAPI CPUTimerWin
  {
  public:

    CPUTimerWin(std::string &timerName);
    ~CPUTimerWin();

    __int64 getTimeDifference();

  private:

    inline void calculateTimeDifference();

    std::string m_timerName;

    __int64 m_cpuTime;
    double m_cpuFrequency;

    bool m_showTimerWhenDying;
  };
}

#endif