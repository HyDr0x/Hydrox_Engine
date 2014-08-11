#include "DataBase/Light.h"

#include <cmath>

namespace he
{
  namespace db
  {
    Light::Light() :
      position(util::Vector<float, 4>(pow(2.0f, 23), pow(2.0f, 23), pow(2.0f, 23), 0.0f)),
      direction(util::Vector<float, 4>::identity()),
      color(util::Vector<float, 4>(1.0f, 1.0f, 1.0f, 0.0f)),
      intensity(0.0f),
      constAttenuation(0.0f),
      linearAttenuation(0.0f),
      quadricAttenuation(0.0f)
    {
    }

    ShadowLight::ShadowLight() :
      position(util::Vector<float, 4>(pow(2.0f, 23), pow(2.0f, 23), pow(2.0f, 23), 0.0f)),
      direction(util::Vector<float, 4>::identity()),
      color(util::Vector<float, 4>(1.0f, 1.0f, 1.0f, 0.0f)),
      intensity(0.0f),
      constAttenuation(0.0f),
      linearAttenuation(0.0f),
      quadricAttenuation(0.0f)
    {
    }
  }
}