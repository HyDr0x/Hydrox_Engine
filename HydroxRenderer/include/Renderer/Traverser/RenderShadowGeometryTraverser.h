#ifndef RENDERSHADOWGEOMETRYTRAVERSER_H_
#define RENDERSHADOWGEOMETRYTRAVERSER_H_

#include <vector>

#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <DataBase/ResourceManager.hpp>

#include "Renderer/Traverser/Traverser.h"
#include "Renderer/Pipeline/RenderOptions.h"

namespace he
{
  namespace renderer
  {
    class RenderShadowGeometryTraverser : public Traverser
    {
    public:

      RenderShadowGeometryTraverser(util::SingletonManager *singletonManager, int viewProjectionIndex);
      virtual ~RenderShadowGeometryTraverser();

      virtual bool preTraverse(VertexDeclarationNode* treeNode);
      virtual void postTraverse(VertexDeclarationNode* treeNode);

      virtual bool preTraverse(ShaderNode* treeNode);
      virtual void postTraverse(ShaderNode* treeNode);

      virtual bool preTraverse(TextureNode* treeNode);
      virtual void postTraverse(TextureNode* treeNode);

      virtual bool preTraverse(RenderNode* treeNode);
      virtual void postTraverse(RenderNode* treeNode);

    private:

      db::ModelManager *m_modelManager;
      db::MaterialManager *m_materialManager;
      db::RenderShaderManager *m_renderShaderManager;
      db::TextureManager *m_textureManager;

      RenderOptions *m_options;

      int m_viewProjectionIndex;
    };
  }
}

#endif
