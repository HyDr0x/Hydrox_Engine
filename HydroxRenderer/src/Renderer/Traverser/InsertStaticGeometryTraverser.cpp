#include "Renderer/Traverser/InsertStaticGeometryTraverser.h"

#include <XBar/StaticGeometryContainer.h>
#include <XBar/SkinnedGeometryContainer.h>

#include "Renderer/TreeNodes/TreeNode.h"
#include "Renderer/TreeNodes/GroupNode.h"
#include "Renderer/TreeNodes/VertexDeclarationNode.h"
#include "Renderer/TreeNodes/ShaderNode.h"
#include "Renderer/TreeNodes/TextureNode.h"
#include "Renderer/TreeNodes/RenderNodeDecorator/IRenderNode.h"

#include "Renderer/Resources/Mesh.h"

namespace he
{
	namespace renderer
	{
    InsertStaticGeometryTraverser::InsertStaticGeometryTraverser(xBar::StaticGeometryContainer& geometryContainer, const RenderOptions& options, util::SingletonManager *singletonManager, std::list<IRenderNode*>& renderNodes) : 
      InsertGeometryTraverser(options, singletonManager, renderNodes),
      m_geometryContainer(geometryContainer)
    {
      Mesh* mesh = m_singletonManager->getService<ModelManager>()->getObject(m_geometryContainer.getMeshHandle());

      if(mesh->getIndexCount() > 0)
      {
        m_nodeType = util::Flags<RenderNodeType>(INDEXEDNODE);
      }

      initialize(mesh, m_geometryContainer.getMaterialHandle());
    }

    InsertStaticGeometryTraverser::~InsertStaticGeometryTraverser()
    {
    }

    bool InsertStaticGeometryTraverser::preTraverse(IRenderNode* treeNode)
    {
      m_inserted = false;
      if(treeNode->insertGeometry(m_geometryContainer))
      {
        m_stopTraversal = true;
        m_inserted = true;
      }

      return false;
    }
	}
}