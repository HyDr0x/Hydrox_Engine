#ifndef RENDERINDEXGEOMETRYTRAVERSER_H_
#define RENDERINDEXGEOMETRYTRAVERSER_H_

#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <DataBase/ResourceManager.hpp>

#include "Renderer/Traverser/Traverser.h"

namespace he
{
  namespace renderer
  {
    class RenderIndexGeometryTraverser : public Traverser
    {
    public:

      RenderIndexGeometryTraverser();
      virtual ~RenderIndexGeometryTraverser();

      void initialize(util::SingletonManager *singletonManager);

      void setGlobalBufferResolution(unsigned int globalBufferResolution);

      virtual bool preTraverse(GroupNode* treeNode);

      virtual bool preTraverse(VertexDeclarationNode* treeNode);
      virtual void postTraverse(VertexDeclarationNode* treeNode);

      virtual bool preTraverse(ShaderNode* treeNode);
      virtual void postTraverse(ShaderNode* treeNode);

      virtual bool preTraverse(RenderNode* treeNode);
      virtual void postTraverse(RenderNode* treeNode);

    private:

      util::SharedPointer<db::ModelManager> m_modelManager;
      util::SharedPointer<db::MaterialManager> m_materialManager;
      util::SharedPointer<db::RenderShaderManager> m_renderShaderManager;
      util::SharedPointer<db::TextureManager> m_textureManager;

      unsigned int m_globalCacheOffset;
      unsigned int m_globalBufferResolution;
    };
  }
}

#endif
