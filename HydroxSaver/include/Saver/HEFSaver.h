#ifndef HEFSAVER_H_
#define HEFSAVER_H_

#include <vector>
#include <string>
#include <map>

#include <Utilities/Miscellaneous/ResourceHandle.h>

#include <SceneGraph\TreeNodes\GeoNode.h>
#include <SceneGraph\TreeNodes\AnimatedGeoNode.h>
#include <SceneGraph\TreeNodes\TransformNode.h>
#include <SceneGraph\TreeNodes\AnimatedTransformNode.h>
#include <SceneGraph\TreeNodes\LODNode.h>
#include <SceneGraph\TreeNodes\BillboardNode.h>

#include "Saver/DLLExport.h"

namespace he
{
  namespace sg
  {
    class Scene;
  }

  namespace renderer
  {
    class Material;
    class Mesh;
    class RenderShader;
    class Texture2D;
  }

	namespace saver
	{
    class GRAPHICAPI HEFSaver
    {
    public:

      void save(std::string path, std::string filename, const sg::Scene* scene, std::map<util::ResourceHandle, std::string> textureNames = std::map<util::ResourceHandle, std::string>());

    private:

      std::vector<

      std::map<util::ResourceHandle, renderer::Material*> m_materialMap;
      std::map<util::ResourceHandle, renderer::Mesh*> m_meshMap;
      std::map<util::ResourceHandle, renderer::RenderShader*> m_renderShaderMap;
      std::map<util::ResourceHandle, renderer::Texture2D*> m_textureMap;
    };
	}
}

#endif
