#ifndef VERTEXDECLARATIONNODE_H_
#define VERTEXDECLARATIONNODE_H_

#include <vector>

#include <GL/glew.h>

#include <Utilities/Math/Math.hpp>

#include "Renderer/TreeNodes/GroupNode.h"

namespace he
{
	namespace renderer
  {
    class Traverser;

    class VertexDeclarationNode : public GroupNode
    {
    public:

      VertexDeclarationNode();
      ~VertexDeclarationNode();

      bool preTraverse(Traverser* traverser);
      void postTraverse(Traverser* traverser);

      void initialize(GLuint vertexDeclaration);

      bool isMesh(GLuint vertexDeclaration) const;

      GLuint getVertexDeclaration() const;

    private:

      GLuint m_vertexDeclaration;
    };
	}
}

#endif
