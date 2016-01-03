#include "Utilities/Timer/Measurement.h"

namespace he
{
  namespace util
  {
    Measurement::Measurement() : m_measuredFrameNumber(0), m_frameCounter(0)
    {
    }

    void Measurement::createMeasurement(unsigned int measuredFrameNumber)
    {
      m_measuredFrameNumber = measuredFrameNumber;

      m_cpuAccumulatedTime.resize(m_measuredFrameNumber, 0);
      m_gpuAccumulatedTime.resize(m_measuredFrameNumber, 0);
    }

    void Measurement::stopMeasurement()
    {
      m_frameCounter = m_measuredFrameNumber;
    }

    void Measurement::resetMeasurement()
    {
      m_frameCounter = 0;

      for(unsigned int i = 0; i < m_cpuAccumulatedTime.size(); i++)
      {
        m_cpuAccumulatedTime[i] = 0;
        m_gpuAccumulatedTime[i] = 0;
      }
    }

    void Measurement::begin()
    {
      if(m_frameCounter < m_measuredFrameNumber)
      {
        m_gpuTimer.start();
        m_cpuTimer.start();
      }
    }

    void Measurement::end()
    {
      if(m_frameCounter < m_measuredFrameNumber)
      {
        m_cpuAccumulatedTime[m_frameCounter] = m_cpuTimer.getTimeDifference();
        m_gpuAccumulatedTime[m_frameCounter] = m_gpuTimer.getTimeDifference();

          m_frameCounter++;
      }
    }

    bool Measurement::running()
    {
      return m_frameCounter != 0 && !finished();
    }

    bool Measurement::finished()
    {
      return m_frameCounter == m_measuredFrameNumber;
    }

    float Measurement::getAveragedCPUResult()
    {
      float summedTimes = 0.0f;

      for(unsigned int i = 0; i < m_cpuAccumulatedTime.size(); i++)
      {
        summedTimes += m_cpuAccumulatedTime[i];
      }

      return summedTimes / float(m_frameCounter);
    }

    float Measurement::getAveragedGPUResult()
    {
      float summedTimes = 0.0f;

      for(unsigned int i = 0; i < m_gpuAccumulatedTime.size(); i++)
      {
        summedTimes += m_gpuAccumulatedTime[i];
      }

      return summedTimes / float(m_frameCounter);
    }

    std::vector<time> Measurement::getCPUSingleMeasurements()
    {
      return m_cpuAccumulatedTime;
    }

    std::vector<GLuint> Measurement::getGPUSingleMeasurements()
    {
      return m_gpuAccumulatedTime;
    }
  }
}