#include "Renderer/Pipeline/CameraManager.h"

namespace he
{
  namespace renderer
  {
    CameraManager::CameraManager()
    {
    }

    CameraManager::~CameraManager()
    {
    }

    unsigned int CameraManager::addCamera(CameraData& camera)
    {
      m_cameras.push_back(camera);

      return m_cameras.size() - 1;
    }

    unsigned int CameraManager::getCameraNumber() const
    {
      return m_cameras.size();
    }

    CameraData& CameraManager::getCamera(unsigned int id)
    {
      return m_cameras[id];
    }

    std::vector<CameraData>& CameraManager::getCameras()
    {
      return m_cameras;
    }
  }
}
