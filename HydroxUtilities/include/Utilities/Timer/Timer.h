#ifndef TIMER_H_
#define TIMER_H_

#ifdef _WIN32
#include "Utilities/Timer/TimerCPUWin.h"
#else
#include "Utilities/Timer/TimerCPUUnix.h" 
#endif

#include "Utilities/Timer/TimerGPUOGL.h" 

namespace he
{
  namespace util
  {
    //#ifdef NDEBUG
    //  #define CPUTIMER(name, i)
    //  #define GPUTIMER(name, i)
    //  #define CPUGETTIMEDIFF(i)
    //  #define GPUGETTIMEDIFF(i)
    //#else
      #define CPUTIMER(name, i) he::util::CPUTimer myTimer##i(std::string(name));
      #define CPUGETTIMEDIFF(i) myTimer##i.getTimeDifference()
      #define GPUTIMER(name, i) he::util::OGLTimer myTimer##i(std::string(name));
      #define GPUGETTIMEDIFF(i) myTimer##i.getTimeDifference()
    //#endif
  }
}

#endif
