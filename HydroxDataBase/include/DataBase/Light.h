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

      util::vec4f position;//w component = spotLightExponent
      util::vec4f direction;//w component = spotLightCutoff

      util::vec4f color;//w component = unused

      float luminousFlux;

      float constAttenuation;
      float linearAttenuation;
      float quadricAttenuation;
    };

    struct GRAPHICAPI ShadowLight
    {
    public:

      ShadowLight();

      util::Matrix<float, 4> viewProj;

      util::vec4f projectionParameter;//x = near, y = far, z = nearWidth, w = unused

      util::vec4f position;//w component = spotLightExponent
      util::vec4f direction;//w component = spotLightCutoff

      util::vec4f color;//w component = unused

      float luminousFlux;

      float constAttenuation;
      float linearAttenuation;
      float quadricAttenuation;
    };
  }
}

#endif
