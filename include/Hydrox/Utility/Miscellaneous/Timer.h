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
  //#ifdef NDEBUG
  //  #define CPUTIMER(name)
  //  #define OGLTIMER(name)
  //  #define CPUGETTIMEDIFF(i)
  //  #define OGLGETTIMEDIFF(i)
  //#else
    #ifdef _WIN32
      #define CPUTIMER(name, i) he::CPUTimerWin myTimer##i(std::string(name));
      #define CPUGETTIMEDIFF(i) myTimer##i.getTimeDifference()
    #else
      #define CPUTIMER(name, i) he::CPUTimerUnix myTimer##i(std::string(name));
      #define CPUGETTIMEDIFF(i) myTimer##i.getTimeDifference()
    #endif
    #define OGLTIMER(name, i) he::OGLTimer myTimer##i(std::string(name));
    #define OGLGETTIMEDIFF(i) myTimer##i.getTimeDifference()
  //#endif
}

#endif
