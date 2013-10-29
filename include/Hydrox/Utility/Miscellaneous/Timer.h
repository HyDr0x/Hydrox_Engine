#ifndef TIMER_H_
#define TIMER_H_

#include <iostream>
#include <time.h>
#include <string>

#include <GL\glew.h>
//#include <cuda_runtime.h>

#include "Hydrox/DLLExport.h"

class GRAPHICAPI Timer
{
public:

  enum TimerType
  {
    OpenGLTimer = 0x00000001,
    CudaTimer   = 0x00000010,
    CPUTimer    = 0x00000100,
  };

  Timer(std::string &timerName, unsigned int bitMask = OpenGLTimer);
  ~Timer();

private:

  //void handleError(cudaError_t cuError);

  unsigned int m_bitMask;

  std::string m_timerName;
  GLuint m_query;
  //cudaEvent_t m_startEvent, m_stopEvent;

  GLuint m_openGLTime;
  //float m_cudaTime;
  clock_t m_cpuTime;
};

#ifdef NDEBUG
#define TIMER(name, timerType)
#else
#define TIMER(name, timerType) Timer myTimer(std::string(name), timerType);
#endif

#endif