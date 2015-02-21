#ifndef RENDERGEOMETRYTRAVERSER_H_
#define RENDERGEOMETRYTRAVERSER_H_

#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <DataBase/ResourceManager.hpp>

#include "Renderer/Traverser/Traverser.h"

#include "Renderer/Pipeline/SamplerObject.h"

namespace he
{
  namespace renderer
  {
    class RenderGeometryTraverser : public Traverser
    {
    public:

      RenderGeometryTraverser();
      virtual ~RenderGeometryTraverser();

      void initialize(util::SingletonManager *singletonManager, std::vector<util::SharedPointer<SamplerObject>> samplerObjects);

      virtual bool preTraverse(GroupNode* treeNode);

      virtual bool preTraverse(VertexDeclarationNode* treeNode);
      virtual void postTraverse(VertexDeclarationNode* treeNode);

      virtual bool preTraverse(ShaderNode* treeNode);
      virtual void postTraverse(ShaderNode* treeNode);

      virtual bool preTraverse(TextureNode* treeNode);
      virtual void postTraverse(TextureNode* treeNode);

      virtual bool preTraverse(RenderNode* treeNode);
      virtual void postTraverse(RenderNode* treeNode);

    private:

      util::SharedPointer<db::RenderShaderManager> m_renderShaderManager;
      util::SharedPointer<db::TextureManager> m_textureManager;

      std::vector<util::SharedPointer<SamplerObject>> m_samplerObjects;
    };
  }
}

#endif
