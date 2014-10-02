#include <XBar/StaticGeometryContainer.h>
#include <XBar/SkinnedGeometryContainer.h>

#include "Renderer/TreeNodes/TreeNode.h"
#include "Renderer/TreeNodes/GroupNode.h"
#include "Renderer/TreeNodes/VertexDeclarationNode.h"
#include "Renderer/TreeNodes/ShaderNode.h"
#include "Renderer/TreeNodes/TextureNode.h"
#include "Renderer/TreeNodes/RenderNodeDecorator/IRenderNode.h"

#include <DataBase/Mesh.h>

namespace he
{
  namespace renderer
  {
    template<typename InsertTraverser> InsertSkinnedGeometryTraverser<InsertTraverser>::InsertSkinnedGeometryTraverser(const xBar::SkinnedGeometryContainer& geometryContainer, const RenderOptions& options, util::SingletonManager *singletonManager) :
      InsertTraverser(geometryContainer, options, singletonManager)
    {
      db::Mesh* mesh = m_singletonManager->getService<db::ModelManager>()->getObject(m_geometryContainer.getMeshHandle());

      if(mesh->getIndexCount() > 0)
      {
        m_nodeType = util::Flags<RenderNodeType>(SKINNEDNODE | INDEXEDNODE);
      }
      else
      {
        m_nodeType = util::Flags<RenderNodeType>(SKINNEDNODE);
      }
    }

    template<typename InsertTraverser> InsertSkinnedGeometryTraverser<InsertTraverser>::~InsertSkinnedGeometryTraverser()
    {
    }
  }
}