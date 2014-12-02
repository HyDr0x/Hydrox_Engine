#ifndef RENDERINDEXGEOMETRYTRAVERSER_H_
#define RENDERINDEXGEOMETRYTRAVERSER_H_

#include <vector>

#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include "Renderer/Traverser/Traverser.h"

#include <DataBase/ResourceManager.hpp>

namespace he
{
  namespace renderer
  {
    class RenderIndexGeometryTraverser : public Traverser
    {
    public:

      RenderIndexGeometryTraverser(util::SingletonManager *singletonManager);
      virtual ~RenderIndexGeometryTraverser();

      virtual bool preTraverse(GroupNode* treeNode);

      virtual bool preTraverse(VertexDeclarationNode* treeNode);
      virtual void postTraverse(VertexDeclarationNode* treeNode);

      virtual bool preTraverse(ShaderNode* treeNode);
      virtual void postTraverse(ShaderNode* treeNode);

      virtual bool preTraverse(RenderNode* treeNode);
      virtual void postTraverse(RenderNode* treeNode);

    private:

      db::ModelManager *m_modelManager;
      db::MaterialManager *m_materialManager;
      db::RenderShaderManager *m_renderShaderManager;
      db::TextureManager *m_textureManager;

      unsigned int m_globalCacheOffset;
    };
  }
}

#endif
