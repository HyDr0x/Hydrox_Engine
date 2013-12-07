#ifndef TIMER_H_
#define TIMER_H_

#ifdef _WIN32
  #include "Hydrox\Utility\Miscellaneous\TimerCPUWin.h" 
#else
  #include "Hydrox\Utility\Miscellaneous\TimerCPUUnix.h"
#endif

#include "Hydrox\Utility\Miscellaneous\TimerGPUOGL.h"

namespace he
{
  #ifdef NDEBUG
    #define CPUTIMER(name)
    #define OGLTIMER(name)
  #else
    #ifdef _WIN32
      #define CPUTIMER(name, i) CPUTimerWin myTimer##i(std::string(name));
      #define CPUGETTIMEDIFF(i) myTimer##i.getTimeDifference()
    #else
      #define CPUTIMER(name, i) CPUTimerUnix myTimer##i(std::string(name));
      #define CPUGETTIMEDIFF(i) myTimer##i.getTimeDifference()
    #endif
    #define OGLTIMER(name, i) OGLTimer myTimer##i(std::string(name));
    #define OGLGETTIMEDIFF(i) myTimer##i.getTimeDifference()
  #endif
}

#endif