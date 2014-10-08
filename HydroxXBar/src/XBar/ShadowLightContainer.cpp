#include "XBar/ShadowLightContainer.h"

#include <DataBase/Light.h>

namespace he
{
  namespace xBar
  {
    ShadowLightContainer::ShadowLightContainer(const db::ShadowLight& lightData, bool reflectiveShadow) :
      m_lightData(lightData),
      m_reflectiveShadow(reflectiveShadow)
    {
    }

    ShadowLightContainer::~ShadowLightContainer()
    {
    }

    bool ShadowLightContainer::operator == (const ShadowLightContainer& other) const
    {
      return m_hash == other.m_hash;
    }

    const db::ShadowLight * const ShadowLightContainer::getLightData() const
    {
      return &m_lightData;
    }

    bool ShadowLightContainer::isReflectiveShadow() const
    {
      return m_reflectiveShadow;
    }

    void ShadowLightContainer::createHash()
    {
      std::vector<unsigned char> data(sizeof(m_lightData));

      unsigned int offset = 0;

      offset = convertToRawData(m_lightData, &data[0], offset);

      m_hash = MurmurHash64A(&data[0], data.size(), 0);
    }
  }
}