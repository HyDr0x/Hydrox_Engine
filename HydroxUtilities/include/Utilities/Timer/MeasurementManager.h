#ifndef MEASUREMENTMANAGER_H_
#define MEASUREMENTMANAGER_H_

#include <string>
#include <map>

#include "Utilities/Timer/Measurement.h"
#include "Utilities/Miscellaneous/SingletonBehaviour.hpp"

namespace he
{
  namespace util
  {
    class GRAPHICAPI MeasurementManager : public SingletonBehaviour
    {
    public:

      MeasurementManager();

      void addMeasurement(std::string name, unsigned int measuredFrameNumber);
      void removeMeasurement(std::string name);

      void stopMeasurement(std::string name);
      void resetMeasurement(std::string name);

      void begin(std::string name);
      void end(std::string name);

      bool running(std::string name);
      bool finished(std::string name);

      float getAveragedCPUResult(std::string name);
      float getAveragedGPUResult(std::string name);

      void outputCPUTimings(std::vector<std::string> measurementName, std::string filename);
      void outputGPUTimings(std::vector<std::string> measurementName, std::string filename);

    private:

      std::map<std::string, Measurement> m_measurements;
    };
  }
}

#endif
