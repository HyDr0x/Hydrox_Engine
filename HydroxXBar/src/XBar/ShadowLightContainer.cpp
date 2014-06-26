#include "XBar/ShadowLightContainer.h"

#include <DataBase/Light.h>

namespace he
{
  namespace xBar
  {
    ShadowLightContainer::ShadowLightContainer(const db::ShadowLight& lightData) :
      m_lightData(lightData)
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

    void ShadowLightContainer::createHash()
    {
      std::vector<unsigned char> data(sizeof(m_lightData));

      unsigned int offset = 0;

      offset = convertToRawData(m_lightData, &data[0], offset);

      m_hash = MurmurHash64A(&data[0], data.size(), 0);
    }
  }
}