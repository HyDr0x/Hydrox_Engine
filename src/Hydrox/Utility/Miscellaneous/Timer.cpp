#include "Hydrox/Utility/Miscellaneous/Timer.h"

namespace he
{
  Timer::Timer(std::string &timerName, unsigned int bitMask) : m_timerName(timerName),
                                                                m_bitMask(bitMask)
  {
    switch(m_bitMask)
    {
    case OpenGLTimer:
      glGenQueries(1, &m_query);
      glBeginQuery(GL_TIME_ELAPSED, m_query);
      break;
    //case CudaTimer:
    //  handleError(cudaEventCreate(&m_startEvent, cudaEventBlockingSync));
    //  handleError(cudaEventCreate(&m_stopEvent, cudaEventBlockingSync));
    //  handleError(cudaEventRecord(m_startEvent));
    //  break;
    case CPUTimer:
      m_cpuTime = clock();
      break;
    }
  }

  Timer::~Timer()
  {
    std::cout << m_timerName << std::endl;
    switch(m_bitMask)
    {
    case OpenGLTimer:
      glEndQuery(GL_TIME_ELAPSED);
      glGetQueryObjectuiv(m_query, GL_QUERY_RESULT, &m_openGLTime);
      std::cout << "OpenGL Time: " << m_openGLTime / (1000000) << " ms" << std::endl;
      glDeleteQueries(1, &m_query);
      break;
    //case CudaTimer:
    //  handleError(cudaEventRecord(m_stopEvent));
    //  handleError(cudaEventSynchronize(m_stopEvent));
    //  handleError(cudaEventElapsedTime(&m_cudaTime, m_startEvent, m_stopEvent));
    //  std::cout << "Cuda Time: " << m_cudaTime << " ms" << std::endl;
    //  handleError(cudaEventDestroy(m_startEvent));
    //  handleError(cudaEventDestroy(m_stopEvent));
    //  break;
    case CPUTimer:
      m_cpuTime = clock() - m_cpuTime;
      std::cout << "CPU Time: " << m_cpuTime / (CLOCKS_PER_SEC * 0.001f) << " ms" << std::endl;
      break;
    }
  }

  //void Timer::handleError(cudaError_t cuError)
  //{
  //	if(cuError != cudaSuccess)
  //	{
  //		printf("Error: %s \n", cudaGetErrorString(cuError));
  //	}
  //}
}