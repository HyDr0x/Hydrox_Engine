#include "Renderer/Pipeline/StringRenderer2D.h"

#include "Renderer/String/StringTexture2D.h"

#include "Renderer/Pipeline/RenderOptions.h"

namespace he
{
  namespace renderer
  {
    StringRenderer2D::StringRenderer2D()
    {
    }

    StringRenderer2D::~StringRenderer2D()
    {
      m_opaqueStrings.clear();
      m_transparentStrings.clear();

      glDeleteVertexArrays(1, &m_stringVAO);
    }

    void StringRenderer2D::initialize(util::SingletonManager *singletonManager)
    {
      m_renderShaderContainer = singletonManager->getService<sh::ShaderContainer>();
      m_textureManager = singletonManager->getService<db::TextureManager>();

      m_stringShaderHandle = m_renderShaderContainer->getRenderShaderHandle(sh::ShaderContainer::STRING2D, sh::ShaderSlotFlags(3));

      registerRenderComponentSlots(singletonManager->getService<util::EventManager>());

      m_maxLayer = singletonManager->getService<RenderOptions>()->max2DLayer;
      m_transparentStrings.resize(m_maxLayer);

      glGenVertexArrays(1, &m_stringVAO);
      glBindVertexArray(m_stringVAO);

      glVertexAttribFormat(sh::RenderShader::POSITION, 2, GL_FLOAT, GL_FALSE, 0);
      glVertexAttribFormat(sh::RenderShader::TEXTURE0, 2, GL_FLOAT, GL_FALSE, sizeof(util::vec2f));

      glVertexAttribBinding(sh::RenderShader::POSITION, 0);
      glVertexAttribBinding(sh::RenderShader::TEXTURE0, 0);

      glEnableVertexAttribArray(sh::RenderShader::POSITION);
      glEnableVertexAttribArray(sh::RenderShader::TEXTURE0);

      glBindVertexArray(0);
    }

    void StringRenderer2D::render() const
    {
      glBindVertexArray(m_stringVAO);

      glClear(GL_DEPTH_BUFFER_BIT);

      glEnable(GL_ALPHA_TEST);
      glAlphaFunc(GL_GREATER, 0.0f);

      const StringTexture2D *renderString;
      db::Texture2D *renderTexture;
      const sh::RenderShader& stringShader = m_renderShaderContainer->getRenderShader(m_stringShaderHandle);

      stringShader.useShader();

      for (std::list<const StringTexture2D*>::const_iterator stringIDIterator = m_opaqueStrings.begin(); stringIDIterator != m_opaqueStrings.end(); stringIDIterator++)
      {
        renderString = (*stringIDIterator);
        if (renderString->getRenderable())
        {
          renderTexture = m_textureManager->getObject(renderString->getFont().fontHandle);
          renderTexture->setTexture(0, 0);

          util::Matrix<float, 3> worldMatrix = renderString->getTransformationMatrix();
          float z = renderString->getLayer() / (float)m_maxLayer;
          sh::RenderShader::setUniform(1, GL_FLOAT_MAT3, &worldMatrix[0][0]);
          sh::RenderShader::setUniform(5, GL_FLOAT, &z);

          renderString->render();
        }
      }

      ////////////////////////////////////////////RENDER TRANSPARENT 2D StringTextures////////////////////////////////////////////

      glDepthMask(GL_FALSE);

      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      for(unsigned int i = 0; i < m_transparentStrings.size(); i++)
      {
        for (std::list<const StringTexture2D*>::const_iterator stringIDIterator = m_transparentStrings[i].begin(); stringIDIterator != m_transparentStrings[i].end(); stringIDIterator++)
        {
          renderString = (*stringIDIterator);
          if (renderString->getRenderable())
          {    
            renderTexture = m_textureManager->getObject(renderString->getFont().fontHandle);
            renderTexture->setTexture(0, 0);

            util::Matrix<float, 3> worldMatrix = renderString->getTransformationMatrix();
            float z = renderString->getLayer() / (float)m_maxLayer;
            sh::RenderShader::setUniform(1, GL_FLOAT_MAT3, &worldMatrix[0][0]);
            sh::RenderShader::setUniform(5, GL_FLOAT, &z);

            renderString->render();
          }
        }
      }

      stringShader.useNoShader();

      glBindVertexArray(0);

      glDisable(GL_BLEND);
      glDisable(GL_ALPHA_TEST);

      glDepthMask(GL_TRUE);
    }

    void StringRenderer2D::addRenderComponent(const StringTexture2D* StringTexture2D)
    {
      if(StringTexture2D->getTransparency())
      {
        m_transparentStrings[StringTexture2D->getLayer()].push_back(StringTexture2D);
      }
      else
      {
        m_opaqueStrings.push_back(StringTexture2D);
      }
    }

    void StringRenderer2D::removeRenderComponent(const StringTexture2D* StringTexture2D)
    {
      if(StringTexture2D->getTransparency())
      {
        m_transparentStrings[StringTexture2D->getLayer()].remove(StringTexture2D);
      }
      else
      {
        m_opaqueStrings.remove(StringTexture2D);
      }
    }

    void StringRenderer2D::registerRenderComponentSlots(util::SharedPointer<util::EventManager> eventManager)
    {
      eventManager->addNewSignal<void (*)(const StringTexture2D* StringTexture2D)>(util::EventManager::OnAddStringTexture2D);
      eventManager->addSlotToSignal<StringRenderer2D, void (*)(const StringTexture2D* StringTexture2D), void (StringRenderer2D::*)(const StringTexture2D* StringTexture2D)>(this, &StringRenderer2D::addRenderComponent, util::EventManager::OnAddStringTexture2D);

      eventManager->addNewSignal<void (*)(const StringTexture2D* StringTexture2D)>(util::EventManager::OnRemoveStringTexture2D);
      eventManager->addSlotToSignal<StringRenderer2D, void (*)(const StringTexture2D* StringTexture2D), void (StringRenderer2D::*)(const StringTexture2D* StringTexture2D)>(this, &StringRenderer2D::removeRenderComponent, util::EventManager::OnRemoveStringTexture2D);
    }
  }
}
