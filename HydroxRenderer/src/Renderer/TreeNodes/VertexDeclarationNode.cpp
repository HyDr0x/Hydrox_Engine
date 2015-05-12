#include "Renderer/TreeNodes/VertexDeclarationNode.h"

#include "Renderer/Traverser/Traverser.h"
#include "Renderer/Traverser/ConstTraverser.h"
#include <Shader/RenderShader.h>
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

    void VertexDeclarationNode::initialize(sh::ShaderSlotFlags shaderVertexDeclaration,
                                           util::Flags<VertexElements> meshVertexDeclaration)
    {
      m_shaderVertexDeclaration = shaderVertexDeclaration;
      m_meshVertexDeclaration = meshVertexDeclaration;

      glBindVertexArray(m_vaoIndex);

      unsigned int vertexStride = 0;

      if(m_meshVertexDeclaration.toInt() & util::Flags<VertexElements>::convertToFlag(db::Mesh::MODEL_POSITION).toInt())
      {
        if(m_shaderVertexDeclaration.toInt() & util::Flags<VertexElements>::convertToFlag(db::Mesh::MODEL_POSITION).toInt())
        {
          glVertexAttribFormat(sh::RenderShader::POSITION, 3, GL_FLOAT, GL_FALSE, vertexStride);
          glVertexAttribBinding(sh::RenderShader::POSITION, 0);
        }

        vertexStride += db::Mesh::vertexDeclarationSize(db::Mesh::MODEL_POSITION);
      }

      if(m_meshVertexDeclaration.toInt() & util::Flags<VertexElements>::convertToFlag(db::Mesh::MODEL_TEXTURE0).toInt())
      {
        if(m_shaderVertexDeclaration.toInt() & util::Flags<VertexElements>::convertToFlag(db::Mesh::MODEL_TEXTURE0).toInt())
        {
          glVertexAttribFormat(sh::RenderShader::TEXTURE0, 2, GL_FLOAT, GL_FALSE, vertexStride);
          glVertexAttribBinding(sh::RenderShader::TEXTURE0, 0);
        }

        vertexStride += db::Mesh::vertexDeclarationSize(db::Mesh::MODEL_TEXTURE0);
      }

      if(m_meshVertexDeclaration.toInt() & util::Flags<VertexElements>::convertToFlag(db::Mesh::MODEL_TEXTURE1).toInt())
      {
        if(m_shaderVertexDeclaration.toInt() & util::Flags<VertexElements>::convertToFlag(db::Mesh::MODEL_TEXTURE1).toInt())
        {
          glVertexAttribFormat(sh::RenderShader::TEXTURE1, 2, GL_FLOAT, GL_FALSE, vertexStride);
          glVertexAttribBinding(sh::RenderShader::TEXTURE1, 0);
        }

        vertexStride += db::Mesh::vertexDeclarationSize(db::Mesh::MODEL_TEXTURE1);
      }

      if(m_meshVertexDeclaration.toInt() & util::Flags<VertexElements>::convertToFlag(db::Mesh::MODEL_TEXTURE2).toInt())
      {
        if(m_shaderVertexDeclaration.toInt() & util::Flags<VertexElements>::convertToFlag(db::Mesh::MODEL_TEXTURE2).toInt())
        {
          glVertexAttribFormat(sh::RenderShader::TEXTURE2, 2, GL_FLOAT, GL_FALSE, vertexStride);
          glVertexAttribBinding(sh::RenderShader::TEXTURE2, 0);
        }

        vertexStride += db::Mesh::vertexDeclarationSize(db::Mesh::MODEL_TEXTURE2);
      }

      if(m_meshVertexDeclaration.toInt() & util::Flags<VertexElements>::convertToFlag(db::Mesh::MODEL_TEXTURE3).toInt())
      {
        if(m_shaderVertexDeclaration.toInt() & util::Flags<VertexElements>::convertToFlag(db::Mesh::MODEL_TEXTURE3).toInt())
        {
          glVertexAttribFormat(sh::RenderShader::TEXTURE3, 2, GL_FLOAT, GL_FALSE, vertexStride);
          glVertexAttribBinding(sh::RenderShader::TEXTURE3, 0);
        }

        vertexStride += db::Mesh::vertexDeclarationSize(db::Mesh::MODEL_TEXTURE3);
      }

      if(m_meshVertexDeclaration.toInt() & util::Flags<VertexElements>::convertToFlag(db::Mesh::MODEL_NORMAL).toInt())
      {
        if(m_shaderVertexDeclaration.toInt() & util::Flags<VertexElements>::convertToFlag(db::Mesh::MODEL_NORMAL).toInt())
        {
          glVertexAttribFormat(sh::RenderShader::NORMAL, 3, GL_FLOAT, GL_FALSE, vertexStride);
          glVertexAttribBinding(sh::RenderShader::NORMAL, 0);
        }

        vertexStride += db::Mesh::vertexDeclarationSize(db::Mesh::MODEL_NORMAL);
      }

      if(m_meshVertexDeclaration.toInt() & util::Flags<VertexElements>::convertToFlag(db::Mesh::MODEL_BINORMAL).toInt())
      {
        if(m_shaderVertexDeclaration.toInt() & util::Flags<VertexElements>::convertToFlag(db::Mesh::MODEL_BINORMAL).toInt())
        {
          glVertexAttribFormat(sh::RenderShader::BINORMAL, 3, GL_FLOAT, GL_FALSE, vertexStride);
          glVertexAttribBinding(sh::RenderShader::BINORMAL, 0);
        }

        vertexStride += db::Mesh::vertexDeclarationSize(db::Mesh::MODEL_BINORMAL);
      }

      if(m_meshVertexDeclaration.toInt() & util::Flags<VertexElements>::convertToFlag(db::Mesh::MODEL_BONE_WEIGHTS).toInt())
      {
        if(m_shaderVertexDeclaration.toInt() & util::Flags<VertexElements>::convertToFlag(db::Mesh::MODEL_BONE_WEIGHTS).toInt())
        {
          glVertexAttribFormat(sh::RenderShader::BONEWEIGHTS, 4, GL_FLOAT, GL_FALSE, vertexStride);
          glVertexAttribBinding(sh::RenderShader::BONEWEIGHTS, 0);
        }

        vertexStride += db::Mesh::vertexDeclarationSize(db::Mesh::MODEL_BONE_WEIGHTS);
      }

      if(m_meshVertexDeclaration.toInt() & util::Flags<VertexElements>::convertToFlag(db::Mesh::MODEL_BONE_INDICES).toInt())
      {
        if(m_shaderVertexDeclaration.toInt() & util::Flags<VertexElements>::convertToFlag(db::Mesh::MODEL_BONE_INDICES).toInt())
        {
          glVertexAttribFormat(sh::RenderShader::BONEINDICES, 4, GL_FLOAT, GL_FALSE, vertexStride);
          glVertexAttribBinding(sh::RenderShader::BONEINDICES, 0);
        }

        vertexStride += db::Mesh::vertexDeclarationSize(db::Mesh::MODEL_BONE_INDICES);
      }

      if(m_meshVertexDeclaration.toInt() & util::Flags<VertexElements>::convertToFlag(db::Mesh::MODEL_COLOR).toInt())
      {
        if(m_shaderVertexDeclaration.toInt() & util::Flags<VertexElements>::convertToFlag(db::Mesh::MODEL_COLOR).toInt())
        {
          glVertexAttribFormat(sh::RenderShader::COLOR, 4, GL_FLOAT, GL_FALSE, vertexStride);
          glVertexAttribBinding(sh::RenderShader::COLOR, 0);
        }

        vertexStride += db::Mesh::vertexDeclarationSize(db::Mesh::MODEL_COLOR);
      }

      if(m_meshVertexDeclaration.toInt() & util::Flags<VertexElements>::convertToFlag(db::Mesh::MODEL_CACHEINDICES0).toInt())
      {
        if(m_shaderVertexDeclaration.toInt() & util::Flags<VertexElements>::convertToFlag(db::Mesh::MODEL_CACHEINDICES0).toInt())
        {
          glVertexAttribFormat(sh::RenderShader::CACHEINDICES0, 4, GLCACHEINDEXTYPE, GL_FALSE, vertexStride);
          glVertexAttribBinding(sh::RenderShader::CACHEINDICES0, 0);
        }

        vertexStride += db::Mesh::vertexDeclarationSize(db::Mesh::MODEL_CACHEINDICES0);
      }

      if(m_meshVertexDeclaration.toInt() & util::Flags<VertexElements>::convertToFlag(db::Mesh::MODEL_CACHEINDICES1).toInt())
      {
        if(m_shaderVertexDeclaration.toInt() & util::Flags<VertexElements>::convertToFlag(db::Mesh::MODEL_CACHEINDICES1).toInt())
        {
          glVertexAttribFormat(sh::RenderShader::CACHEINDICES1, 4, GLCACHEINDEXTYPE, GL_FALSE, vertexStride);
          glVertexAttribBinding(sh::RenderShader::CACHEINDICES1, 0);
        }

        vertexStride += db::Mesh::vertexDeclarationSize(db::Mesh::MODEL_CACHEINDICES1);
      }

      glBindVertexArray(0);
    }

    void VertexDeclarationNode::setVertexArray() const
    {
      glBindVertexArray(m_vaoIndex);

      for(unsigned int i = 0; i < db::Mesh::VERTEXDECLARATIONFLAGNUMBER; i++)
      {
        if(m_shaderVertexDeclaration.toInt() & util::Flags<VertexElements>::convertToFlag(i).toInt())
        {
          glEnableVertexAttribArray(i);
        }
      }
    }

    void VertexDeclarationNode::unsetVertexArray() const
    {
      for(unsigned int i = 0; i < db::Mesh::VERTEXDECLARATIONFLAGNUMBER; i++)
      {
        if(m_shaderVertexDeclaration.toInt() & util::Flags<VertexElements>::convertToFlag(i).toInt())
        {
          glDisableVertexAttribArray(i);
        }
      }

      glBindVertexArray(0);
    }

    bool VertexDeclarationNode::isMesh(util::Flags<VertexElements> meshVertexDeclaration) const
    {
      return m_meshVertexDeclaration == meshVertexDeclaration;
    }
  }
}
