#include "Renderer/TreeNodes/VertexDeclarationNode.h"

#include "Renderer/Traverser/Traverser.h"
#include "Renderer/Resources/RenderShader.h"
#include "Renderer/Resources/Mesh.h"

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

    void VertexDeclarationNode::initialize(GLuint vertexDeclaration)
    {
      m_vertexDeclaration = vertexDeclaration;

      glBindVertexArray(m_vaoIndex);

      unsigned int vertexStride = 0;

      if(m_vertexDeclaration & Mesh::MODEL_POSITION) 
      {
        glVertexAttribFormat(RenderShader::POSITION, 3, GL_FLOAT, GL_FALSE, vertexStride);
        glVertexAttribBinding(RenderShader::POSITION, 0);
        vertexStride += sizeof(util::Vector<float, 3>);
      }

      if(m_vertexDeclaration & Mesh::MODEL_TEXTURE0) 
      {
        glVertexAttribFormat(RenderShader::TEXTURE0, 2, GL_FLOAT, GL_FALSE, vertexStride);
        glVertexAttribBinding(RenderShader::TEXTURE0, 0);
        vertexStride += sizeof(util::Vector<float, 2>);
      }

      if(m_vertexDeclaration & Mesh::MODEL_NORMAL) 
      {
        glVertexAttribFormat(RenderShader::NORMAL, 3, GL_FLOAT, GL_FALSE, vertexStride);
        glVertexAttribBinding(RenderShader::NORMAL, 0);
        vertexStride += sizeof(util::Vector<float, 3>);
      }

      if(m_vertexDeclaration & Mesh::MODEL_BINORMAL) 
      {
        glVertexAttribFormat(RenderShader::BINORMAL, 3, GL_FLOAT, GL_FALSE, vertexStride);
        glVertexAttribBinding(RenderShader::BINORMAL, 0);
        vertexStride += sizeof(util::Vector<float, 3>);
      }

      if(m_vertexDeclaration & Mesh::MODEL_BONE_WEIGHTS) 
      {
        glVertexAttribFormat(RenderShader::BONEWEIGHTS, 4, GL_FLOAT, GL_FALSE, vertexStride);
        glVertexAttribBinding(RenderShader::BONEWEIGHTS, 0);
        vertexStride += sizeof(util::Vector<float, 4>);
      }

      if(m_vertexDeclaration & Mesh::MODEL_BONE_WEIGHTS) 
      {
        glVertexAttribFormat(RenderShader::BONEINDICES, 4, GL_FLOAT, GL_FALSE, vertexStride);
        glVertexAttribBinding(RenderShader::BONEINDICES, 0);
        vertexStride += sizeof(util::Vector<float, 4>);
      }

      if(m_vertexDeclaration & Mesh::MODEL_COLOR) 
      {
        glVertexAttribFormat(RenderShader::COLOR, 4, GL_FLOAT, GL_FALSE, vertexStride);
        glVertexAttribBinding(RenderShader::COLOR, 0);
        vertexStride += sizeof(util::Vector<float, 4>);
      }

      glBindVertexArray(0);
    }

    void VertexDeclarationNode::setVertexArray() const
    {
      glBindVertexArray(m_vaoIndex);

      if(m_vertexDeclaration & Mesh::MODEL_POSITION)
      {
        glEnableVertexAttribArray(RenderShader::POSITION);
      }

      if(m_vertexDeclaration & Mesh::MODEL_TEXTURE0)
      {
        glEnableVertexAttribArray(RenderShader::TEXTURE0);
      }

      if(m_vertexDeclaration & Mesh::MODEL_TEXTURE1)
      {
        glEnableVertexAttribArray(RenderShader::TEXTURE1);
      }

      if(m_vertexDeclaration & Mesh::MODEL_TEXTURE2)
      {
        glEnableVertexAttribArray(RenderShader::TEXTURE2);
      }

      if(m_vertexDeclaration & Mesh::MODEL_TEXTURE3)
      {
        glEnableVertexAttribArray(RenderShader::TEXTURE3);
      }

      if(m_vertexDeclaration & Mesh::MODEL_NORMAL)
      {
        glEnableVertexAttribArray(RenderShader::NORMAL);
      }

      if(m_vertexDeclaration & Mesh::MODEL_BINORMAL)
      {
        glEnableVertexAttribArray(RenderShader::BINORMAL);
      }

      if(m_vertexDeclaration & Mesh::MODEL_BONE_INDICES)
      {
        glEnableVertexAttribArray(RenderShader::BONEINDICES);
      }

      if(m_vertexDeclaration & Mesh::MODEL_BONE_WEIGHTS)
      {
        glEnableVertexAttribArray(RenderShader::BONEWEIGHTS);
      }

      if(m_vertexDeclaration & Mesh::MODEL_COLOR)
      {
        glEnableVertexAttribArray(RenderShader::COLOR);
      }
    }

    void VertexDeclarationNode::unsetVertexArray() const
    {
      if(m_vertexDeclaration & Mesh::MODEL_POSITION)
      {
        glDisableVertexAttribArray(RenderShader::POSITION);
      }

      if(m_vertexDeclaration & Mesh::MODEL_TEXTURE0)
      {
        glDisableVertexAttribArray(RenderShader::TEXTURE0);
      }

      if(m_vertexDeclaration & Mesh::MODEL_TEXTURE1)
      {
        glDisableVertexAttribArray(RenderShader::TEXTURE1);
      }

      if(m_vertexDeclaration & Mesh::MODEL_TEXTURE2)
      {
        glDisableVertexAttribArray(RenderShader::TEXTURE2);
      }

      if(m_vertexDeclaration & Mesh::MODEL_TEXTURE3)
      {
        glDisableVertexAttribArray(RenderShader::TEXTURE3);
      }

      if(m_vertexDeclaration & Mesh::MODEL_NORMAL)
      {
        glDisableVertexAttribArray(RenderShader::NORMAL);
      }

      if(m_vertexDeclaration & Mesh::MODEL_BINORMAL)
      {
        glDisableVertexAttribArray(RenderShader::BINORMAL);
      }

      if(m_vertexDeclaration & Mesh::MODEL_BONE_INDICES)
      {
        glDisableVertexAttribArray(RenderShader::BONEINDICES);
      }

      if(m_vertexDeclaration & Mesh::MODEL_BONE_WEIGHTS)
      {
        glDisableVertexAttribArray(RenderShader::BONEWEIGHTS);
      }

      if(m_vertexDeclaration & Mesh::MODEL_COLOR)
      {
        glDisableVertexAttribArray(RenderShader::COLOR);
      }

      glBindVertexArray(0);
    }

    bool VertexDeclarationNode::isMesh(GLuint m_vertexDeclaration) const
    {
      return m_vertexDeclaration == m_vertexDeclaration;
    }
	}
}
