#ifndef STRINGTEXTURE2D_H_
#define STRINGTEXTURE2D_H_

#include <vector>
#include <string>

#include <GL\glew.h>

#include <Utilities/Math/Math.hpp>
#include <Utilities/Signals/EventManager.h>
#include <Utilities/Miscellaneous/ResourceHandle.h>

#include "Renderer/DLLExport.h"

namespace he
{
	namespace renderer
	{
    struct GRAPHICAPI Font
    {
      std::vector<unsigned int> lut;
      unsigned int width;
      unsigned int height;
      util::ResourceHandle fontHandle;
    };

    class GRAPHICAPI StringTexture2D
    {
    public:

      StringTexture2D(util::EventManager *eventManager, const Font& font, std::string text, float width, float height);
      StringTexture2D(const StringTexture2D& o);
      ~StringTexture2D();

      StringTexture2D& operator=(const StringTexture2D& o);

      void setRenderable(bool renderable);
      bool getRenderable() const;

      void setTransparency(bool transparency);
      bool getTransparency() const;

      void stringSorted();
      bool getLayerChanged();

      void setLayer(unsigned char layer);
      unsigned char getLayer();

      void setTranslation(util::Vector<float, 2> v);
      void setTranslation(float x, float y);

      void addTranslation(util::Vector<float, 2> v);
      void addTranslation(float x, float y);

      void setScale(float s);
      void addScale(float s);

      void setScale(util::Vector<float, 2> s);
      void setScale(float sx, float sy);
      void addScale(util::Vector<float, 2> s);
      void addScale(float sx, float sy);

      void setRotation(float angle);
      void addRotation(float angle);

      util::Vector<float, 2> getPosition();
      float getRotation();
      util::Vector<float, 2> getScale();

      util::Matrix<float, 3> getTransformationMatrix();

      const Font& getFont();

      void render();

    private:

      void generateBuffer(std::string text, float width, float height);

      util::EventManager *m_eventManager;

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

      bool m_layerChanged;
      unsigned char m_layer;
      std::string m_text;
    };
	}
}

#endif
