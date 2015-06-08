#ifndef ARENDERPASS_H_
#define ARENDERPASS_H_

#include <list>

#include <Utilities/Math/Math.hpp>

#include <XBar/StaticGeometryContainer.h>

#include <DataBase/ResourceManager.hpp>

#include <Shader/ShaderContainer.h>

#include "Renderer/Renderpass/ShaderNode.h"
#include "Renderer/Renderpass/VertexDeclarationNode.h"
#include "Renderer/Renderpass/TextureNode.h"

#include "Renderer/Renderdata/RenderDataFactory.h"

namespace he
{
  namespace renderer
  {
    class RenderOptions;

    class ARenderpass
    {
    public:

      ARenderpass();
      virtual ~ARenderpass();

      virtual void initialize(util::SingletonManager *singletonManager, sh::ShaderContainer::Renderpass renderPass);

      virtual bool insertGeometry(util::SharedPointer<IRenderContainer> renderContainer);
      virtual bool removeGeometry(util::SharedPointer<IRenderContainer> renderContainer);

      virtual void drawRenderpass() const = 0;

    protected:

      util::SharedPointer<RenderOptions> m_options;

      sh::ShaderContainer::Renderpass m_renderPass;
      util::SharedPointer<sh::ShaderContainer> m_shaderContainer;

      std::vector<ShaderNode> m_shaderNodes;
      std::vector<std::vector<VertexDeclarationNode>> m_vertexDeclarationNodes;
      std::vector<std::vector<TextureNode>> m_textureNodes;
      std::vector<std::vector<util::SharedPointer<IRenderContainer>>> m_renderContainer;
    };
  }
}

#endif
