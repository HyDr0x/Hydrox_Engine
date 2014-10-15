#include "DataBase/Light.h"

#include <cmath>

namespace he
{
  namespace db
  {
    Light::Light() :
      position(util::vec4f(pow(2.0f, 23), pow(2.0f, 23), pow(2.0f, 23), 0.0f)),
      direction(util::vec4f::identity()),
      color(util::vec4f(1.0f, 1.0f, 1.0f, 0.0f)),
      intensity(0.0f),
      constAttenuation(0.0f),
      linearAttenuation(0.0f),
      quadricAttenuation(0.0f)
    {
    }

    ShadowLight::ShadowLight() :
      position(util::vec4f(pow(2.0f, 23), pow(2.0f, 23), pow(2.0f, 23), 0.0f)),
      direction(util::vec4f::identity()),
      color(util::vec4f(1.0f, 1.0f, 1.0f, 0.0f)),
      intensity(0.0f),
      constAttenuation(0.0f),
      linearAttenuation(0.0f),
      quadricAttenuation(0.0f)
    {
    }
  }
}