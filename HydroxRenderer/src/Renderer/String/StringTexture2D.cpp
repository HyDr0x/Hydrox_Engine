#include "Renderer/String/StringTexture2D.h"

#include <vector>

#include <Utilities/Math/Math.hpp>

namespace he
{
  namespace renderer
  {
    StringTexture2D::StringTexture2D(util::EventManager *eventManager, const Font& font, const std::string& text, float width, float height) :
      m_font(font),
      m_layer(0),
      m_angle(0.0f),
      m_rtMatrix(util::Matrix<float, 3>::identity()),
      m_tlMatrix(util::Matrix<float, 3>::identity()),
      m_scMatrix(util::Matrix<float, 3>::identity()),
      m_renderable(true),
      m_transparency(false)
    {
      m_eventManager = eventManager;

      glGenBuffers(1, &m_vertexBufferIndex);
      glGenBuffers(1, &m_indexBufferIndex);

      fillBuffer(text, width, height);
    }

    StringTexture2D::StringTexture2D(const StringTexture2D& o)
    {
      m_eventManager = o.m_eventManager;

      m_font = o.m_font;

      m_text = o.m_text;

      m_triangleNumber = o.m_triangleNumber;

      m_rtMatrix = o.m_rtMatrix;
      m_tlMatrix = o.m_tlMatrix;
      m_scMatrix = o.m_scMatrix;

      m_angle = o.m_angle;

      m_renderable = o.m_renderable;
      m_transparency = o.m_transparency;

      m_layer = o.m_layer;

      glGenBuffers(1, &m_vertexBufferIndex);
      glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferIndex);
      glBufferData(GL_ARRAY_BUFFER, sizeof(util::Vector<float, 2>) * m_vertexNumber * 2, nullptr, GL_STATIC_DRAW);
        glBindBuffer(GL_COPY_READ_BUFFER, o.m_vertexBufferIndex);
        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_ARRAY_BUFFER, 0, 0, sizeof(util::Vector<float, 2>) * m_vertexNumber * 2);
        glBindBuffer(GL_COPY_READ_BUFFER, 0);
      glBindBuffer(GL_ARRAY_BUFFER, 0);

      glGenBuffers(1, &m_indexBufferIndex);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferIndex);
      glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned int) * m_triangleNumber * 3, nullptr, GL_STATIC_DRAW);
        glBindBuffer(GL_COPY_READ_BUFFER, o.m_indexBufferIndex);
        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_ARRAY_BUFFER, 0, 0, sizeof(unsigned int) * m_triangleNumber * 3);
        glBindBuffer(GL_COPY_READ_BUFFER, 0);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    StringTexture2D::~StringTexture2D()
    {
      glDeleteBuffers(1, &m_vertexBufferIndex);
      glDeleteBuffers(1, &m_indexBufferIndex);
    }

    StringTexture2D& StringTexture2D::operator=(const StringTexture2D& o)
    {
      m_eventManager = o.m_eventManager;

      m_font = o.m_font;

      m_text = o.m_text;

      m_triangleNumber = o.m_triangleNumber;

      m_rtMatrix = o.m_rtMatrix;
      m_tlMatrix = o.m_tlMatrix;
      m_scMatrix = o.m_scMatrix;

      m_angle = o.m_angle;

      m_renderable = o.m_renderable;
      m_transparency = o.m_transparency;

      m_layer = o.m_layer;

      glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferIndex);
      glBufferData(GL_ARRAY_BUFFER, sizeof(util::Vector<float, 2>) * m_vertexNumber * 2, nullptr, GL_STATIC_DRAW);
        glBindBuffer(GL_COPY_READ_BUFFER, o.m_vertexBufferIndex);
        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_ARRAY_BUFFER, 0, 0, sizeof(util::Vector<float, 2>) * m_vertexNumber * 2);
        glBindBuffer(GL_COPY_READ_BUFFER, 0);
      glBindBuffer(GL_ARRAY_BUFFER, 0);

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferIndex);
      glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned int) * m_triangleNumber * 3, nullptr, GL_STATIC_DRAW);
        glBindBuffer(GL_COPY_READ_BUFFER, o.m_indexBufferIndex);
        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_ARRAY_BUFFER, 0, 0, sizeof(unsigned int) * m_triangleNumber * 3);
        glBindBuffer(GL_COPY_READ_BUFFER, 0);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

      return *this;
    }

    void StringTexture2D::editString(const std::string& text, float width, float height)
    {
      fillBuffer(text, width, height);
    }

    void StringTexture2D::setRenderable(bool renderable)
    {
      if (!m_renderable && renderable)
      {
        m_eventManager->raiseSignal<void(*)(const StringTexture2D *stringTexture2D)>(util::EventManager::OnAddStringTexture2D)->execute(this);
      }
      else if (m_renderable && !renderable)
      {
        m_eventManager->raiseSignal<void(*)(const StringTexture2D *stringTexture2D)>(util::EventManager::OnRemoveStringTexture2D)->execute(this);
      }

      m_renderable = renderable;
    }

    bool StringTexture2D::getRenderable() const
    {
      return m_renderable;
    }

    void StringTexture2D::setTransparency(bool transparency)
    {
      if (m_renderable)
      {
        if (m_transparency != transparency)
        {
          m_eventManager->raiseSignal<void(*)(const StringTexture2D *stringTexture2D)>(util::EventManager::OnRemoveStringTexture2D)->execute(this);

          m_transparency = transparency;

          m_eventManager->raiseSignal<void(*)(const StringTexture2D *stringTexture2D)>(util::EventManager::OnAddStringTexture2D)->execute(this);
        }
      }
      else
      {
        m_transparency = transparency;
      }
    }

    bool StringTexture2D::getTransparency() const
    {
      return m_transparency;
    }

    void StringTexture2D::setLayer(unsigned char layer)
    {
      if (m_renderable && m_transparency)
      {
        if (m_layer != layer)
        {
          m_eventManager->raiseSignal<void(*)(const StringTexture2D *stringTexture2D)>(util::EventManager::OnRemoveStringTexture2D)->execute(this);

          m_layer = layer;

          m_eventManager->raiseSignal<void(*)(const StringTexture2D *stringTexture2D)>(util::EventManager::OnAddStringTexture2D)->execute(this);
        }
      }
      else
      {
        m_layer = layer;
      }
    }

    unsigned char StringTexture2D::getLayer() const
    {
      return m_layer;
    }

    void StringTexture2D::setTranslation(const util::Vector<float, 2>& v)
    {
      m_tlMatrix[2][0] = v[0];
      m_tlMatrix[2][1] = v[1];
    }

    void StringTexture2D::setTranslation(float x, float y)
    {
      m_tlMatrix[2][0] = x;
      m_tlMatrix[2][1] = y;
    }

    void StringTexture2D::addTranslation(const util::Vector<float, 2>& v)
    {
      m_tlMatrix[2][0] += v[0];
      m_tlMatrix[2][1] += v[1];
    }

    void StringTexture2D::addTranslation(float x, float y)
    {
      m_tlMatrix[2][0] += x;
      m_tlMatrix[2][1] += y;
    }

    void StringTexture2D::setScale(float s)
    {
      m_scMatrix[0][0] = s;
      m_scMatrix[1][1] = s;
    }

    void StringTexture2D::addScale(float s)
    {
      m_scMatrix[0][0] += s;
      m_scMatrix[1][1] += s;
    }

    void StringTexture2D::setScale(const util::Vector<float, 2>& s)
    {
      m_scMatrix[0][0] = s[0];
      m_scMatrix[1][1] = s[1];
    }

    void StringTexture2D::setScale(float sx, float sy)
    {
      m_scMatrix[0][0] = sx;
      m_scMatrix[1][1] = sy;
    }

    void StringTexture2D::addScale(const util::Vector<float, 2>& s)
    {
      m_scMatrix[0][0] += s[0];
      m_scMatrix[1][1] += s[1];
    }

    void StringTexture2D::addScale(float sx, float sy)
    {
      m_scMatrix[0][0] += sx;
      m_scMatrix[1][1] += sy;
    }

    void StringTexture2D::setRotation(float angle)
    {
      m_angle = angle;
      m_rtMatrix[0][0] = m_rtMatrix[1][1] = cos(m_angle);
      m_rtMatrix[1][0] = sin(m_angle);
      m_rtMatrix[0][1] = -m_rtMatrix[1][0];
    }

    void StringTexture2D::addRotation(float angle)
    {
      m_angle += angle;
      m_rtMatrix[0][0] = m_rtMatrix[1][1] = cos(m_angle);
      m_rtMatrix[1][0] = sin(m_angle);
      m_rtMatrix[0][1] = -m_rtMatrix[1][0];
    }

    util::Vector<float, 2> StringTexture2D::getPosition() const
    {
      return util::Vector<float, 2>(m_tlMatrix[2][0], m_tlMatrix[2][1]);
    }

    float StringTexture2D::getRotation() const
    {
      return m_angle;
    }

    util::Vector<float, 2> StringTexture2D::getScale() const
    {
      return util::Vector<float, 2>(m_scMatrix[0][0], m_scMatrix[1][1]);
    }

    util::Matrix<float, 3> StringTexture2D::getTransformationMatrix() const
    {
      return m_tlMatrix * m_rtMatrix * m_scMatrix;
    }

    Font StringTexture2D::getFont() const
    {
      return m_font;
    }

    std::string StringTexture2D::getText() const
    {
      return m_text;
    }

    void StringTexture2D::render() const
    {
      glBindVertexBuffer(0, m_vertexBufferIndex, 0, sizeof(util::Vector<float, 2>) * 2);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferIndex);
      glDrawElements(GL_TRIANGLES, m_triangleNumber * 3, GL_UNSIGNED_INT, nullptr);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
      glBindVertexBuffer(0, 0, 0, 0);
    }

    void StringTexture2D::fillBuffer(std::string text, float width, float height)
    {
      unsigned int letterNumber = text.size();
      
      m_vertexNumber = letterNumber * 4;

      std::vector<float> geometryData(m_vertexNumber * 4);

      float texWidth = 1.0f / m_font.letterNumberX;
      float texHeight = 1.0f / m_font.letterNumberY;

      util::Vector<float, 2> position;
      util::Vector<float, 2> texCoord;
      for (unsigned int i = 0; i < letterNumber; i++)
      {
        //first
        position[0] = i * width;
        position[1] = height;

        unsigned int decodedChar = m_font.lut[(unsigned char)text[i]];
        
        texCoord = util::Vector<float, 2>((decodedChar % m_font.letterNumberX) * texWidth, unsigned int(decodedChar * texWidth) * texHeight);

        int z = sizeof(util::Vector<float, 2>);
        std::copy(&position[0], &position[0] + 2, geometryData.begin() + (4 * i + 0) * 4);
        std::copy(&texCoord[0], &texCoord[0] + 2, geometryData.begin() + (4 * i + 0) * 4 + 2);

        //second
        position[0] = i * width;
        position[1] = 0;

        texCoord[1] += texHeight;

        std::copy(&position[0], &position[0] + 2, geometryData.begin() + (4 * i + 1) * 4);
        std::copy(&texCoord[0], &texCoord[0] + 2, geometryData.begin() + (4 * i + 1) * 4 + 2);

        //third
        position[0] = (i + 1) * width;
        position[1] = height;

        texCoord[0] += texWidth;
        texCoord[1] -= texHeight;

        std::copy(&position[0], &position[0] + 2, geometryData.begin() + (4 * i + 2) * 4);
        std::copy(&texCoord[0], &texCoord[0] + 2, geometryData.begin() + (4 * i + 2) * 4 + 2);

        //fourth
        position[0] = (i + 1) * width;
        position[1] = 0;

        texCoord[1] += texHeight;

        std::copy(&position[0], &position[0] + 2, geometryData.begin() + (4 * i + 3) * 4);
        std::copy(&texCoord[0], &texCoord[0] + 2, geometryData.begin() + (4 * i + 3) * 4 + 2);
      }

      glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferIndex);
      glBufferData(GL_ARRAY_BUFFER, sizeof(util::Vector<float, 2>) * m_vertexNumber * 2, &geometryData[0], GL_STATIC_DRAW);
      glBindBuffer(GL_ARRAY_BUFFER, 0);

      m_triangleNumber = letterNumber * 2;
      std::vector<unsigned int> indices(m_triangleNumber * 3);

      for (unsigned int i = 0; i < letterNumber; i++)
      {
        indices[i * 6 + 0] = i * 4;
        indices[i * 6 + 1] = i * 4 + 1;
        indices[i * 6 + 2] = i * 4 + 2;

        indices[i * 6 + 3] = i * 4 + 2;
        indices[i * 6 + 4] = i * 4 + 1;
        indices[i * 6 + 5] = i * 4 + 3;
      }

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferIndex);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_triangleNumber * 3, &indices[0], GL_STATIC_DRAW);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
  }
}