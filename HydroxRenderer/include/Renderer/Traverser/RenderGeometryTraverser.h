#ifndef RENDERGEOMETRYTRAVERSER_H_
#define RENDERGEOMETRYTRAVERSER_H_

#include <vector>

#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include "Renderer/Traverser/Traverser.h"

#include <DataBase/ResourceManager.hpp>

namespace he
{
  namespace renderer
  {
    class IRenderNode;

    class RenderGeometryTraverser : public Traverser
    {
    public:

      RenderGeometryTraverser(util::SingletonManager *singletonManager, unsigned int viewProjectionIndex);
      virtual ~RenderGeometryTraverser();

      virtual bool preTraverse(VertexDeclarationNode* treeNode);
      virtual void postTraverse(VertexDeclarationNode* treeNode);

      virtual bool preTraverse(ShaderNode* treeNode);
      virtual void postTraverse(ShaderNode* treeNode);

      virtual bool preTraverse(TextureNode* treeNode);
      virtual void postTraverse(TextureNode* treeNode);

      virtual bool preTraverse(IRenderNode* treeNode);
      virtual void postTraverse(IRenderNode* treeNode);

    private:

      db::ModelManager *m_modelManager;
      db::MaterialManager *m_materialManager;
      db::RenderShaderManager *m_renderShaderManager;
      db::TextureManager *m_textureManager;

      unsigned int m_viewProjectionIndex;
    };
  }
}

#endif
