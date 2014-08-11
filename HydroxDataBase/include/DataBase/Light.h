#ifndef LIGHT_H_
#define LIGHT_H_

#include "DLLExport.h"

#include <Utilities/Math/Math.hpp>

namespace he
{
  enum LightType
  {
    POINTLIGHT,
    DIRECTIONALLIGHT,
    SPOTLIGHT,
  };

  namespace db
  {
    struct GRAPHICAPI Light
    {
    public:

      Light();

      util::Vector<float, 4> position;//w component = spotLightExponent
      util::Vector<float, 4> direction;//w component = spotLightCutoff

      util::Vector<float, 4> color;//w component = shadow algorithm

      float intensity;

      float constAttenuation;
      float linearAttenuation;
      float quadricAttenuation;
    };

    struct GRAPHICAPI ShadowLight
    {
    public:

      ShadowLight();

      util::Vector<float, 4> position;//w component = spotLightExponent
      util::Vector<float, 4> direction;//w component = spotLightCutoff

      util::Vector<float, 4> color;//w component = shadow algorithm

      float intensity;

      float constAttenuation;
      float linearAttenuation;
      float quadricAttenuation;

      util::Matrix<float, 4> viewProj;
      util::Matrix<float, 4> invViewProj;
    };
  }
}

#endif
