#ifndef RENDERINDIRECTLIGHTGEOMETRYTRAVERSER_H_
#define RENDERINDIRECTLIGHTGEOMETRYTRAVERSER_H_

#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <DataBase/ResourceManager.hpp>

#include "Renderer/Traverser/Traverser.h"

#include "Renderer/Pipeline/RenderOptions.h"

namespace he
{
  namespace renderer
  {
    class RenderIndirectLightingGeometryTraverser : public Traverser
    {
    public:

      RenderIndirectLightingGeometryTraverser();
      virtual ~RenderIndirectLightingGeometryTraverser();

      void initialize(util::SingletonManager *singletonManager, util::SharedPointer<db::Texture2D> normalMap, util::SharedPointer<db::Texture2D> materialMap);

      void setGlobalBufferResolution(unsigned int globalBufferResolution);

      virtual bool preTraverse(GroupNode* treeNode);

      virtual bool preTraverse(VertexDeclarationNode* treeNode);
      virtual void postTraverse(VertexDeclarationNode* treeNode);

      virtual bool preTraverse(ShaderNode* treeNode);
      virtual void postTraverse(ShaderNode* treeNode);

      virtual bool preTraverse(RenderNode* treeNode);
      virtual void postTraverse(RenderNode* treeNode);

    private:

      util::SharedPointer<db::RenderShaderManager> m_renderShaderManager;

      util::SharedPointer<db::Texture2D> m_normalMap;
      util::SharedPointer<db::Texture2D> m_materialMap;

      unsigned int m_globalCacheOffset;
      unsigned int m_globalBufferResolution;
    };
  }
}

#endif
