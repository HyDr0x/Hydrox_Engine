#ifndef RENDERSHADOWGEOMETRYTRAVERSER_H_
#define RENDERSHADOWGEOMETRYTRAVERSER_H_

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

      RenderShadowGeometryTraverser();
      virtual ~RenderShadowGeometryTraverser();

      void initialize(util::SingletonManager *singletonManager);

      void setViewProjectionIndex(int viewProjectionIndex);

      virtual bool preTraverse(VertexDeclarationNode* treeNode);
      virtual void postTraverse(VertexDeclarationNode* treeNode);

      virtual bool preTraverse(ShaderNode* treeNode);
      virtual void postTraverse(ShaderNode* treeNode);

      virtual bool preTraverse(TextureNode* treeNode);
      virtual void postTraverse(TextureNode* treeNode);

      virtual bool preTraverse(RenderNode* treeNode);
      virtual void postTraverse(RenderNode* treeNode);

    private:

      util::SharedPointer<db::ModelManager> m_modelManager;
      util::SharedPointer<db::MaterialManager> m_materialManager;
      util::SharedPointer<db::RenderShaderManager> m_renderShaderManager;
      util::SharedPointer<db::TextureManager> m_textureManager;

      util::SharedPointer<RenderOptions> m_options;

      int m_viewProjectionIndex;
    };
  }
}

#endif
