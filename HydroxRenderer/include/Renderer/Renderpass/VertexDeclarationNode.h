#ifndef VERTEXDECLARATIONNODE_H_
#define VERTEXDECLARATIONNODE_H_

#include <vector>

#include <GL/glew.h>

#include <Utilities/Math/Math.hpp>
#include <Utilities/Miscellaneous/Flags.hpp>
#include <Utilities/Pointer/UniquePointer.h>

#include <DataBase/Mesh.h>

#include <Shader/ShaderContainer.h>

namespace he
{
  namespace renderer
  {
    class VertexDeclarationNode
    {
    public:

      VertexDeclarationNode();
      VertexDeclarationNode(VertexDeclarationNode& other);
      ~VertexDeclarationNode();

      const VertexDeclarationNode& operator=(VertexDeclarationNode other);

      void initialize(sh::ShaderSlotFlags shaderVertexDeclaration, VertexElementFlags meshVertexDeclaration);

      void setVertexArray() const;
      void unsetVertexArray() const;

      bool isMesh(VertexElementFlags meshVertexDeclaration) const;

    private:

      util::UniquePointer<GLuint> m_vaoIndex;
      sh::ShaderSlotFlags m_shaderVertexDeclaration;
      VertexElementFlags m_meshVertexDeclaration;
    };
  }
}

#endif
