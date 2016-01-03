#include "Utilities/Timer/MeasurementManager.h"

#include <fstream>

namespace he
{
  namespace util
  {
    MeasurementManager::MeasurementManager()
    {
    }

    void MeasurementManager::addMeasurement(std::string name, unsigned int measuredFrameNumber)
    {
      m_measurements[name] = Measurement();
      m_measurements[name].createMeasurement(measuredFrameNumber);
    }

    void MeasurementManager::removeMeasurement(std::string name)
    {
      m_measurements.erase(m_measurements.find(name));
    }

    void MeasurementManager::stopMeasurement(std::string name)
    {
      if(m_measurements.count(name) == 1)
      {
        m_measurements[name].stopMeasurement();
      }
    }

    void MeasurementManager::resetMeasurement(std::string name)
    {
      if(m_measurements.count(name) == 1)
      {
        m_measurements[name].resetMeasurement();
      }
    }

    void MeasurementManager::begin(std::string name)
    {
      if(m_measurements.count(name) == 1)
      {
        m_measurements[name].begin();
      }
    }

    void MeasurementManager::end(std::string name)
    {
      if(m_measurements.count(name) == 1)
      {
        m_measurements[name].end();
      }
    }

    bool MeasurementManager::running(std::string name)
    {
      if(m_measurements.count(name) == 1)
      {
        return m_measurements[name].running();
      }
      else
      {
        return false;
      }
    }

    bool MeasurementManager::finished(std::string name)
    {
      if(m_measurements.count(name) == 1)
      {
        return m_measurements[name].finished();
      }
      else
      {
        return false;
      }
    }

    float MeasurementManager::getAveragedCPUResult(std::string name)
    {
      return m_measurements[name].getAveragedCPUResult();
    }

    float MeasurementManager::getAveragedGPUResult(std::string name)
    {
      return m_measurements[name].getAveragedGPUResult();
    }

    void MeasurementManager::outputCPUTimings(std::vector<std::string> measurementName, std::string filename)
    {
      unsigned int lineNumber = 0;
      bool linesAvailable = false;

      std::ofstream output;

      do
      {
        output << lineNumber;

        time accumulatedTime = 0;
        for(unsigned int i = 0; i < measurementName.size(); i++)
        {
          std::map<std::string, Measurement>::iterator it = m_measurements.find(measurementName[i]);

          if(lineNumber < it->second.getCPUSingleMeasurements().size())
          {
            linesAvailable = true;

            accumulatedTime += it->second.getCPUSingleMeasurements()[lineNumber];
          }

          //output << std::string(" ") << accumulatedTime;

          lineNumber++;
        }

        output << std::endl;

      } while(linesAvailable);
    }

    void MeasurementManager::outputGPUTimings(std::vector<std::string> measurementName, std::string filename)
    {
      unsigned int lineNumber = 0;
      bool linesAvailable = false;

      std::ofstream output;

      do
      {
        output << lineNumber;

        GLuint accumulatedTime = 0;
        for(unsigned int i = 0; i < measurementName.size(); i++)
        {
          std::map<std::string, Measurement>::iterator it = m_measurements.find(measurementName[i]);

          if(lineNumber < it->second.getGPUSingleMeasurements().size())
          {
            linesAvailable = true;

            accumulatedTime += it->second.getCPUSingleMeasurements()[lineNumber];
          }

          //output << std::string(" ") << accumulatedTime;

          lineNumber++;
        }

        output << std::endl;

      } while(linesAvailable);
    }
  }
}