#ifndef VERTEXDECLARATIONNODE_H_
#define VERTEXDECLARATIONNODE_H_

#include <vector>

#include <GL/glew.h>

#include <Utilities/Math/Math.hpp>
#include <Utilities/Miscellaneous/Flags.hpp>

#include <DataBase/Mesh.h>

#include "Renderer/TreeNodes/GroupNode.h"

namespace he
{
  namespace renderer
  {
    class VertexDeclarationNode : public GroupNode
    {
    public:

      VertexDeclarationNode();
      ~VertexDeclarationNode();

      bool preTraverse(Traverser* traverser);
      void postTraverse(Traverser* traverser);

      bool preTraverse(ConstTraverser* traverser) const;
      void postTraverse(ConstTraverser* traverser) const;

      void initialize(util::Flags<VertexElements> shaderVertexDeclaration,
        util::Flags<VertexElements> meshVertexDeclaration);

      void setVertexArray() const;
      void unsetVertexArray() const;

      bool isMesh(util::Flags<VertexElements> meshVertexDeclaration) const;

    private:

      GLuint m_vaoIndex;
      util::Flags<VertexElements> m_shaderVertexDeclaration;
      util::Flags<VertexElements> m_meshVertexDeclaration;
    };
  }
}

#endif
