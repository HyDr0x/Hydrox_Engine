#include "Renderer/TreeNodes/VertexDeclarationNode.h"

#include "Renderer/Traverser/InsertGeometryTraverser.h"

namespace he
{
	namespace renderer
	{
    VertexDeclarationNode::VertexDeclarationNode()
    {
    }

    VertexDeclarationNode::~VertexDeclarationNode()
    {
    }

    bool VertexDeclarationNode::preTraverse(Traverser* traverser)
    {
      return traverser->preTraverse(this);
    }

    void VertexDeclarationNode::postTraverse(Traverser* traverser)
    {
      traverser->postTraverse(this);
    }

    void VertexDeclarationNode::initialize(GLuint vertexDeclaration)
    {
      m_vertexDeclaration = vertexDeclaration;
    }

    bool VertexDeclarationNode::isMesh(GLuint vertexDeclaration)
    {
      return vertexDeclaration == m_vertexDeclaration;
    }

    GLuint VertexDeclarationNode::getVertexDeclaration()
    {
      return m_vertexDeclaration;
    }
	}
}
