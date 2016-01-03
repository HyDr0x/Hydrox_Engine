#ifndef MEASUREMENT_H_
#define MEASUREMENT_H_

#include <vector>

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
    class GRAPHICAPI Measurement
    {
    public:

      Measurement();

      void createMeasurement(unsigned int measuredFrameNumber);

      void stopMeasurement();
      void resetMeasurement();

      void begin();
      void end();

      bool running();//did the measurement already started?
      bool finished();//is the measurement done?

      float getAveragedCPUResult();
      float getAveragedGPUResult();

      std::vector<time> getCPUSingleMeasurements();
      std::vector<GLuint> getGPUSingleMeasurements();

    private:

      CPUTimer m_cpuTimer;
      OGLTimer m_gpuTimer;
      unsigned int m_measuredFrameNumber;
      unsigned int m_frameCounter;
      std::vector<time> m_cpuAccumulatedTime;
      std::vector<GLuint> m_gpuAccumulatedTime;
    };
  }
}

#endif
