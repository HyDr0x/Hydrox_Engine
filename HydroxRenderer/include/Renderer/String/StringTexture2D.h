#ifndef STRINGTEXTURE2D_H_
#define STRINGTEXTURE2D_H_

#include <vector>
#include <string>

#include <GL/glew.h>

#include <Utilities/Math/Math.hpp>
#include <Utilities/Signals/EventManager.h>
#include <Utilities/Pointer/SharedPointer.h>
#include <Utilities/Miscellaneous/ResourceHandle.h>

#include "Renderer/DLLExport.h"

namespace he
{
  namespace renderer
  {
    struct GRAPHICAPI Font
    {
      std::vector<unsigned int> lut;
      unsigned int letterNumberX;
      unsigned int letterNumberY;
      util::ResourceHandle fontHandle;
    };

    class GRAPHICAPI StringTexture2D
    {
    public:

      StringTexture2D(util::SharedPointer<util::EventManager> eventManager, const Font& font, const std::string& text, float width, float height);
      StringTexture2D(const StringTexture2D& other);
      ~StringTexture2D();

      StringTexture2D& operator=(const StringTexture2D& other);

      void editString(const std::string& text, float width, float height);

      void setRenderable(bool renderable);
      bool getRenderable() const;

      void setTransparency(bool transparency);
      bool getTransparency() const;

      void setLayer(unsigned char layer);
      unsigned char getLayer() const;

      void setTranslation(const util::vec2f& v);
      void setTranslation(float x, float y);

      void addTranslation(const util::vec2f& v);
      void addTranslation(float x, float y);

      void setScale(float s);
      void addScale(float s);

      void setScale(const util::vec2f& s);
      void setScale(float sx, float sy);
      void addScale(const util::vec2f& s);
      void addScale(float sx, float sy);

      void setRotation(float angle);
      void addRotation(float angle);

      util::vec2f getPosition() const;
      float getRotation() const;
      util::vec2f getScale() const;

      util::Matrix<float, 3> getTransformationMatrix() const;

      Font getFont() const;

      std::string getText() const;

      void render() const;

    private:

      void fillBuffer(std::string text, float width, float height);

      util::SharedPointer<util::EventManager> m_eventManager;

      Font m_font;

      GLuint m_vertexBufferIndex;
      GLuint m_indexBufferIndex;
      unsigned int m_vertexNumber;
      unsigned int m_triangleNumber;

      bool m_renderable;
      bool m_transparency;

      float m_angle;
      util::Matrix<float, 3> m_rtMatrix;
      util::Matrix<float, 3> m_tlMatrix;
      util::Matrix<float, 3> m_scMatrix;

      unsigned char m_layer;
      std::string m_text;
    };
  }
}

#endif
