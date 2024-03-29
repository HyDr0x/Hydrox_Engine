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
    //  #define CPUTIMER(name)
    //  #define OGLTIMER(name)
    //  #define CPUGETTIMEDIFF(i)
    //  #define OGLGETTIMEDIFF(i)
    //#else
      #ifdef _WIN32
        #define CPUTIMER(name, i) he::util::CPUTimerWin myTimer##i(std::string(name));
        #define CPUGETTIMEDIFF(i) myTimer##i.getTimeDifference()
      #else
        #define CPUTIMER(name, i) he::util::CPUTimerUnix myTimer##i(std::string(name));
        #define CPUGETTIMEDIFF(i) myTimer##i.getTimeDifference()
      #endif
      #define OGLTIMER(name, i) he::util::OGLTimer myTimer##i(std::string(name));
      #define OGLGETTIMEDIFF(i) myTimer##i.getTimeDifference()
    //#endif
	}
}

#endif
