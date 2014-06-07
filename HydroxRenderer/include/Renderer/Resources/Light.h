#ifndef LIGHT_H_
#define LIGHT_H_

#include <Utilities/Math/Math.hpp>

#include "Renderer/DLLExport.h"
#include "Renderer/Resources/ManagedResource.h"


namespace he
{
  namespace renderer
  {
    class GRAPHICAPI Light : public ManagedResource
    {
    public:

      void free();

    private:

      util::Vector<float, 3> m_position;
      util::Vector<float, 3> m_spotLightDirection;

      float m_intensity;

      float m_spotLightExponent;
      float m_spotLightCutoff;

      float m_constAttenuation;
      float m_linearAttenuation;
      float m_quadricAttenuation;
    };
  }
}

#endif
