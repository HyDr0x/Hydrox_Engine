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
    //  #define CPUTIMER(i)
    //  #define GPUTIMER(i)
    //  #define CPUGETTIMEDIFF(i)
    //  #define GPUGETTIMEDIFF(i)
    //#else
      #define CPUTIMER(i) he::util::CPUTimer myTimer##i;
      #define CPUGETTIMEDIFF(i) myTimer##i.getTimeDifference()
      #define GPUTIMER(i) he::util::OGLTimer myTimer##i;
      #define GPUGETTIMEDIFF(i) myTimer##i.getTimeDifference()
    //#endif
  }
}

#endif
