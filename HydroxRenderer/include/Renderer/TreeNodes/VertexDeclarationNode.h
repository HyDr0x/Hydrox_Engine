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

      void setVertexArray() const;
      void unsetVertexArray() const;

      bool isMesh(GLuint vertexDeclaration) const;

    private:

      GLuint m_vaoIndex;
      GLuint m_vertexDeclaration;
    };
  }
}

#endif
