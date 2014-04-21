#include "Renderer/Pipeline/StringRenderer2D.h"

#include "Renderer/String/StringTexture2D.h"

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

    void StringRenderer2D::initialize(util::SingletonManager *singletonManager, util::ResourceHandle stringShaderHandle, unsigned char maxLayer)
    {
      m_renderShaderManager = singletonManager->getService<RenderShaderManager>();
      m_textureManager = singletonManager->getService<TextureManager>();

      registerRenderComponentSlots(singletonManager->getService<util::EventManager>());

      m_stringShaderHandle = stringShaderHandle;

      m_maxLayer = maxLayer;
      m_transparentStrings.resize(m_maxLayer);

      glGenVertexArrays(1, &m_stringVAO);
      glBindVertexArray(m_stringVAO);

      glVertexAttribFormat(RenderShader::POSITION, 2, GL_FLOAT, GL_FALSE, 0);
      glVertexAttribFormat(RenderShader::TEXTURE0, 2, GL_FLOAT, GL_FALSE, sizeof(util::Vector<float, 2>));

      glVertexAttribBinding(RenderShader::POSITION, 0);
      glVertexAttribBinding(RenderShader::TEXTURE0, 0);

      glEnableVertexAttribArray(RenderShader::POSITION);
      glEnableVertexAttribArray(RenderShader::TEXTURE0);

      glBindVertexArray(0);
    }

    void StringRenderer2D::render()
    {
      glBindVertexArray(m_stringVAO);

      glClear(GL_DEPTH_BUFFER_BIT);

      glEnable(GL_ALPHA_TEST);
      glAlphaFunc(GL_GREATER, 0.0f);

      StringTexture2D *renderString;
      Texture2D *renderTexture;
      RenderShader *stringShader = m_renderShaderManager->getObject(m_stringShaderHandle);

      stringShader->useShader();

      for (std::list<StringTexture2D*>::iterator stringIDIterator = m_opaqueStrings.begin(); stringIDIterator != m_opaqueStrings.end(); stringIDIterator++)
      {
        renderString = (*stringIDIterator);
        if (renderString->getRenderable())
        {
          renderTexture = m_textureManager->getObject(renderString->getFont().fontHandle);
          renderTexture->setTexture(0, 0);

          util::Matrix<float, 3> worldMatrix = renderString->getTransformationMatrix();
          float z = renderString->getLayer() / (float)m_maxLayer;
		      RenderShader::setUniform(1, GL_FLOAT_MAT3, &worldMatrix[0][0]);
          RenderShader::setUniform(5, GL_FLOAT, &z);

          renderString->render();
        }
	    }

      ////////////////////////////////////////////RENDER TRANSPARENT 2D StringTextures////////////////////////////////////////////

      for(unsigned int i = 0; i < m_transparentStrings.size(); i++)//resort all StringTextures according to their layer if their layer has been changed
      {
        for (std::list<StringTexture2D*>::iterator stringIDIterator = m_transparentStrings[i].begin(); stringIDIterator != m_transparentStrings[i].end(); stringIDIterator++)
        {
          renderString = (*stringIDIterator);
          if (renderString->getLayerChanged())
          {
            m_transparentStrings[renderString->getLayer()].push_back(renderString);
            renderString->stringSorted();
            stringIDIterator = m_transparentStrings[i].erase(stringIDIterator);

            if (stringIDIterator == m_transparentStrings[i].end())
            {
              break;
            }
          }
        }
      }

      glDepthMask(GL_FALSE);

      glEnable(GL_BLEND);
	    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      for(unsigned int i = 0; i < m_transparentStrings.size(); i++)
      {
        for (std::list<StringTexture2D*>::iterator stringIDIterator = m_transparentStrings[i].begin(); stringIDIterator != m_transparentStrings[i].end(); stringIDIterator++)
        {
          renderString = (*stringIDIterator);
          if (renderString->getRenderable())
          {		
            renderTexture = m_textureManager->getObject(renderString->getFont().fontHandle);
            renderTexture->setTexture(0, 0);

            util::Matrix<float, 3> worldMatrix = renderString->getTransformationMatrix();
            float z = renderString->getLayer() / (float)m_maxLayer;
            RenderShader::setUniform(1, GL_FLOAT_MAT3, &worldMatrix[0][0]);
            RenderShader::setUniform(5, GL_FLOAT, &z);

            renderString->render();
          }
	      }
      }

      stringShader->useNoShader();

      glBindVertexArray(0);

      glDisable(GL_BLEND);
      glDisable(GL_ALPHA_TEST);

      glDepthMask(GL_TRUE);
    }

    void StringRenderer2D::addRenderComponent(StringTexture2D* StringTexture2D)
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

    void StringRenderer2D::removeRenderComponent(StringTexture2D* StringTexture2D)
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

    void StringRenderer2D::registerRenderComponentSlots(util::EventManager *eventManager)
    {
      eventManager->addNewSignal<void (*)(StringTexture2D* StringTexture2D)>(util::EventManager::OnAddStringTexture2D);
      eventManager->addSlotToSignal<StringRenderer2D, void (*)(StringTexture2D* StringTexture2D), void (StringRenderer2D::*)(StringTexture2D* StringTexture2D)>(this, &StringRenderer2D::addRenderComponent, util::EventManager::OnAddStringTexture2D);

      eventManager->addNewSignal<void (*)(StringTexture2D* StringTexture2D)>(util::EventManager::OnRemoveStringTexture2D);
      eventManager->addSlotToSignal<StringRenderer2D, void (*)(StringTexture2D* StringTexture2D), void (StringRenderer2D::*)(StringTexture2D* StringTexture2D)>(this, &StringRenderer2D::removeRenderComponent, util::EventManager::OnRemoveStringTexture2D);
    }
	}
}
