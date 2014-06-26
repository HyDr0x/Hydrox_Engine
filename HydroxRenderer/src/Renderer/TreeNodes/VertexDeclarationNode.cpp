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

      if(m_vertexDeclaration & db::Mesh::MODEL_POSITION) 
      {
        glVertexAttribFormat(db::RenderShader::POSITION, 3, GL_FLOAT, GL_FALSE, vertexStride);
        glVertexAttribBinding(db::RenderShader::POSITION, 0);
        vertexStride += sizeof(util::Vector<float, 3>);
      }

      if(m_vertexDeclaration & db::Mesh::MODEL_TEXTURE0) 
      {
        glVertexAttribFormat(db::RenderShader::TEXTURE0, 2, GL_FLOAT, GL_FALSE, vertexStride);
        glVertexAttribBinding(db::RenderShader::TEXTURE0, 0);
        vertexStride += sizeof(util::Vector<float, 2>);
      }

      if(m_vertexDeclaration & db::Mesh::MODEL_NORMAL) 
      {
        glVertexAttribFormat(db::RenderShader::NORMAL, 3, GL_FLOAT, GL_FALSE, vertexStride);
        glVertexAttribBinding(db::RenderShader::NORMAL, 0);
        vertexStride += sizeof(util::Vector<float, 3>);
      }

      if(m_vertexDeclaration & db::Mesh::MODEL_BINORMAL) 
      {
        glVertexAttribFormat(db::RenderShader::BINORMAL, 3, GL_FLOAT, GL_FALSE, vertexStride);
        glVertexAttribBinding(db::RenderShader::BINORMAL, 0);
        vertexStride += sizeof(util::Vector<float, 3>);
      }

      if(m_vertexDeclaration & db::Mesh::MODEL_BONE_WEIGHTS) 
      {
        glVertexAttribFormat(db::RenderShader::BONEWEIGHTS, 4, GL_FLOAT, GL_FALSE, vertexStride);
        glVertexAttribBinding(db::RenderShader::BONEWEIGHTS, 0);
        vertexStride += sizeof(util::Vector<float, 4>);
      }

      if(m_vertexDeclaration & db::Mesh::MODEL_BONE_WEIGHTS) 
      {
        glVertexAttribFormat(db::RenderShader::BONEINDICES, 4, GL_FLOAT, GL_FALSE, vertexStride);
        glVertexAttribBinding(db::RenderShader::BONEINDICES, 0);
        vertexStride += sizeof(util::Vector<float, 4>);
      }

      if(m_vertexDeclaration & db::Mesh::MODEL_COLOR) 
      {
        glVertexAttribFormat(db::RenderShader::COLOR, 4, GL_FLOAT, GL_FALSE, vertexStride);
        glVertexAttribBinding(db::RenderShader::COLOR, 0);
        vertexStride += sizeof(util::Vector<float, 4>);
      }

      glBindVertexArray(0);
    }

    void VertexDeclarationNode::setVertexArray() const
    {
      glBindVertexArray(m_vaoIndex);

      if(m_vertexDeclaration & db::Mesh::MODEL_POSITION)
      {
        glEnableVertexAttribArray(db::RenderShader::POSITION);
      }

      if(m_vertexDeclaration & db::Mesh::MODEL_TEXTURE0)
      {
        glEnableVertexAttribArray(db::RenderShader::TEXTURE0);
      }

      if(m_vertexDeclaration & db::Mesh::MODEL_TEXTURE1)
      {
        glEnableVertexAttribArray(db::RenderShader::TEXTURE1);
      }

      if(m_vertexDeclaration & db::Mesh::MODEL_TEXTURE2)
      {
        glEnableVertexAttribArray(db::RenderShader::TEXTURE2);
      }

      if(m_vertexDeclaration & db::Mesh::MODEL_TEXTURE3)
      {
        glEnableVertexAttribArray(db::RenderShader::TEXTURE3);
      }

      if(m_vertexDeclaration & db::Mesh::MODEL_NORMAL)
      {
        glEnableVertexAttribArray(db::RenderShader::NORMAL);
      }

      if(m_vertexDeclaration & db::Mesh::MODEL_BINORMAL)
      {
        glEnableVertexAttribArray(db::RenderShader::BINORMAL);
      }

      if(m_vertexDeclaration & db::Mesh::MODEL_BONE_INDICES)
      {
        glEnableVertexAttribArray(db::RenderShader::BONEINDICES);
      }

      if(m_vertexDeclaration & db::Mesh::MODEL_BONE_WEIGHTS)
      {
        glEnableVertexAttribArray(db::RenderShader::BONEWEIGHTS);
      }

      if(m_vertexDeclaration & db::Mesh::MODEL_COLOR)
      {
        glEnableVertexAttribArray(db::RenderShader::COLOR);
      }
    }

    void VertexDeclarationNode::unsetVertexArray() const
    {
      if(m_vertexDeclaration & db::Mesh::MODEL_POSITION)
      {
        glDisableVertexAttribArray(db::RenderShader::POSITION);
      }

      if(m_vertexDeclaration & db::Mesh::MODEL_TEXTURE0)
      {
        glDisableVertexAttribArray(db::RenderShader::TEXTURE0);
      }

      if(m_vertexDeclaration & db::Mesh::MODEL_TEXTURE1)
      {
        glDisableVertexAttribArray(db::RenderShader::TEXTURE1);
      }

      if(m_vertexDeclaration & db::Mesh::MODEL_TEXTURE2)
      {
        glDisableVertexAttribArray(db::RenderShader::TEXTURE2);
      }

      if(m_vertexDeclaration & db::Mesh::MODEL_TEXTURE3)
      {
        glDisableVertexAttribArray(db::RenderShader::TEXTURE3);
      }

      if(m_vertexDeclaration & db::Mesh::MODEL_NORMAL)
      {
        glDisableVertexAttribArray(db::RenderShader::NORMAL);
      }

      if(m_vertexDeclaration & db::Mesh::MODEL_BINORMAL)
      {
        glDisableVertexAttribArray(db::RenderShader::BINORMAL);
      }

      if(m_vertexDeclaration & db::Mesh::MODEL_BONE_INDICES)
      {
        glDisableVertexAttribArray(db::RenderShader::BONEINDICES);
      }

      if(m_vertexDeclaration & db::Mesh::MODEL_BONE_WEIGHTS)
      {
        glDisableVertexAttribArray(db::RenderShader::BONEWEIGHTS);
      }

      if(m_vertexDeclaration & db::Mesh::MODEL_COLOR)
      {
        glDisableVertexAttribArray(db::RenderShader::COLOR);
      }

      glBindVertexArray(0);
    }

    bool VertexDeclarationNode::isMesh(GLuint m_vertexDeclaration) const
    {
      return m_vertexDeclaration == m_vertexDeclaration;
    }
  }
}
