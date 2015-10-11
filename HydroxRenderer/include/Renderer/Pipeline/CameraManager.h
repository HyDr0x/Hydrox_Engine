#ifndef CAMERAMANAGER_H_
#define CAMERAMANAGER_H_

#include <vector>

#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Math/Math.hpp>
#include <Utilities/Pointer/SharedPointer.h>

#include "Renderer/DLLExport.h"

namespace he
{
  namespace renderer
  {
    struct CameraData
    {
      util::Matrix<float, 4> viewMatrix;
      util::Matrix<float, 4> projectionMatrix;
      util::Matrix<float, 4> viewProjectionMatrix;
      util::Matrix<float, 4> invViewProjectionMatrix;
      util::vec3f cameraPosition;
      unsigned int width;
      unsigned int height;
      float znear;
      float zfar;
    };

    class GRAPHICAPI CameraManager : public util::SingletonBehaviour
    {
    public:

      CameraManager();
      ~CameraManager();

      unsigned int addCamera(CameraData& camera);
      void deleteCamera(unsigned int id);

      unsigned int getCameraNumber() const;

      CameraData& getCamera(unsigned int id);
      std::vector<CameraData>& getCameras();

    private:

      std::vector<CameraData> m_cameras;
    };
  }
}

#endif