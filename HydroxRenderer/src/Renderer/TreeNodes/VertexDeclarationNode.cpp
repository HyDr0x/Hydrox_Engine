#include "Renderer/TreeNodes/VertexDeclarationNode.h"

#include "Renderer/Traverser/Traverser.h"
#include "Renderer/Traverser/ConstTraverser.h"
#include <DataBase/RenderShader.h>
#include <DataBase/Mesh.h>

namespace he
{
  namespace renderer
  {
    VertexDeclarationNode::VertexDeclarationNode()
    {
      glGenVertexArrays(1, &m_vaoIndex);
    }

    VertexDeclarationNode::~VertexDeclarationNode()
    {
      glDeleteVertexArrays(1, &m_vaoIndex);
    }

    bool VertexDeclarationNode::preTraverse(Traverser* traverser)
    {
      return traverser->preTraverse(this);
    }

    void VertexDeclarationNode::postTraverse(Traverser* traverser)
    {
      traverser->postTraverse(this);
    }

    bool VertexDeclarationNode::preTraverse(ConstTraverser* traverser) const
    {
      return traverser->preTraverse(this);
    }

    void VertexDeclarationNode::postTraverse(ConstTraverser* traverser) const
    {
      traverser->postTraverse(this);
    }

    void VertexDeclarationNode::initialize(GLuint vertexDeclaration)
    {
      m_vertexDeclaration = vertexDeclaration;

      glBindVertexArray(m_vaoIndex);

      unsigned int vertexStride = 0;

      if(m_vertexDeclaration & db::Mesh::vertexDeclarationFlag(db::Mesh::MODEL_POSITION))
      {
        glVertexAttribFormat(db::RenderShader::POSITION, 3, GL_FLOAT, GL_FALSE, vertexStride);
        glVertexAttribBinding(db::RenderShader::POSITION, 0);
        vertexStride += db::Mesh::vertexDeclarationSize(db::Mesh::MODEL_POSITION);
      }

      if(m_vertexDeclaration & db::Mesh::vertexDeclarationFlag(db::Mesh::MODEL_TEXTURE0))
      {
        glVertexAttribFormat(db::RenderShader::TEXTURE0, 2, GL_FLOAT, GL_FALSE, vertexStride);
        glVertexAttribBinding(db::RenderShader::TEXTURE0, 0);
        vertexStride += db::Mesh::vertexDeclarationSize(db::Mesh::MODEL_TEXTURE0);
      }

      if(m_vertexDeclaration & db::Mesh::vertexDeclarationFlag(db::Mesh::MODEL_TEXTURE1))
      {
        glVertexAttribFormat(db::RenderShader::TEXTURE1, 2, GL_FLOAT, GL_FALSE, vertexStride);
        glVertexAttribBinding(db::RenderShader::TEXTURE1, 0);
        vertexStride += db::Mesh::vertexDeclarationSize(db::Mesh::MODEL_TEXTURE1);
      }

      if(m_vertexDeclaration & db::Mesh::vertexDeclarationFlag(db::Mesh::MODEL_TEXTURE2))
      {
        glVertexAttribFormat(db::RenderShader::TEXTURE2, 2, GL_FLOAT, GL_FALSE, vertexStride);
        glVertexAttribBinding(db::RenderShader::TEXTURE2, 0);
        vertexStride += db::Mesh::vertexDeclarationSize(db::Mesh::MODEL_TEXTURE2);
      }

      if(m_vertexDeclaration & db::Mesh::vertexDeclarationFlag(db::Mesh::MODEL_TEXTURE3))
      {
        glVertexAttribFormat(db::RenderShader::TEXTURE3, 2, GL_FLOAT, GL_FALSE, vertexStride);
        glVertexAttribBinding(db::RenderShader::TEXTURE3, 0);
        vertexStride += db::Mesh::vertexDeclarationSize(db::Mesh::MODEL_TEXTURE3);
      }

      if(m_vertexDeclaration & db::Mesh::vertexDeclarationFlag(db::Mesh::MODEL_NORMAL))
      {
        glVertexAttribFormat(db::RenderShader::NORMAL, 3, GL_FLOAT, GL_FALSE, vertexStride);
        glVertexAttribBinding(db::RenderShader::NORMAL, 0);
        vertexStride += db::Mesh::vertexDeclarationSize(db::Mesh::MODEL_NORMAL);
      }

      if(m_vertexDeclaration & db::Mesh::vertexDeclarationFlag(db::Mesh::MODEL_BINORMAL))
      {
        glVertexAttribFormat(db::RenderShader::BINORMAL, 3, GL_FLOAT, GL_FALSE, vertexStride);
        glVertexAttribBinding(db::RenderShader::BINORMAL, 0);
        vertexStride += db::Mesh::vertexDeclarationSize(db::Mesh::MODEL_BINORMAL);
      }

      if(m_vertexDeclaration & db::Mesh::vertexDeclarationFlag(db::Mesh::MODEL_BONE_WEIGHTS))
      {
        glVertexAttribFormat(db::RenderShader::BONEWEIGHTS, 4, GL_FLOAT, GL_FALSE, vertexStride);
        glVertexAttribBinding(db::RenderShader::BONEWEIGHTS, 0);
        vertexStride += db::Mesh::vertexDeclarationSize(db::Mesh::MODEL_BONE_WEIGHTS);
      }

      if(m_vertexDeclaration & db::Mesh::vertexDeclarationFlag(db::Mesh::MODEL_BONE_INDICES))
      {
        glVertexAttribFormat(db::RenderShader::BONEINDICES, 4, GL_FLOAT, GL_FALSE, vertexStride);
        glVertexAttribBinding(db::RenderShader::BONEINDICES, 0);
        vertexStride += db::Mesh::vertexDeclarationSize(db::Mesh::MODEL_BONE_INDICES);
      }

      if(m_vertexDeclaration & db::Mesh::vertexDeclarationFlag(db::Mesh::MODEL_COLOR))
      {
        glVertexAttribFormat(db::RenderShader::COLOR, 4, GL_FLOAT, GL_FALSE, vertexStride);
        glVertexAttribBinding(db::RenderShader::COLOR, 0);
        vertexStride += db::Mesh::vertexDeclarationSize(db::Mesh::MODEL_COLOR);
      }

      if(m_vertexDeclaration & db::Mesh::vertexDeclarationFlag(db::Mesh::MODEL_CACHEINDIZES0))
      {
        glVertexAttribFormat(db::RenderShader::CACHEINDIZES0, 4, GLCACHEINDEXTYPE, GL_FALSE, vertexStride);
        glVertexAttribBinding(db::RenderShader::CACHEINDIZES0, 0);
        vertexStride += db::Mesh::vertexDeclarationSize(db::Mesh::MODEL_CACHEINDIZES0);
      }

      if(m_vertexDeclaration & db::Mesh::vertexDeclarationFlag(db::Mesh::MODEL_CACHEINDIZES1))
      {
        glVertexAttribFormat(db::RenderShader::CACHEINDIZES1, 4, GLCACHEINDEXTYPE, GL_FALSE, vertexStride);
        glVertexAttribBinding(db::RenderShader::CACHEINDIZES1, 0);
        vertexStride += db::Mesh::vertexDeclarationSize(db::Mesh::MODEL_CACHEINDIZES1);
      }

      glBindVertexArray(0);
    }

    void VertexDeclarationNode::setVertexArray() const
    {
      glBindVertexArray(m_vaoIndex);

      for(unsigned int i = 0; i < db::Mesh::VERTEXDECLARATIONFLAGNUMBER; i++)
      {
        if(m_vertexDeclaration & db::Mesh::vertexDeclarationFlag(i))
        {
          glEnableVertexAttribArray(i);
        }
      }
    }

    void VertexDeclarationNode::unsetVertexArray() const
    {
      for(unsigned int i = 0; i < db::Mesh::VERTEXDECLARATIONFLAGNUMBER; i++)
      {
        if(m_vertexDeclaration & db::Mesh::vertexDeclarationFlag(i))
        {
          glDisableVertexAttribArray(i);
        }
      }

      glBindVertexArray(0);
    }

    bool VertexDeclarationNode::isMesh(GLuint vertexDeclaration) const
    {
      return m_vertexDeclaration == vertexDeclaration;
    }
  }
}
