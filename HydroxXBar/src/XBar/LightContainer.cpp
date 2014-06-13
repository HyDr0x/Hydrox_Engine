#include "XBar/LightContainer.h"

#include <DataBase/Light.h>

namespace he
{
  namespace xBar
  {
    LightContainer::LightContainer(const db::Light& lightData) : m_lightData(lightData)
    {
      createHash();
    }

    LightContainer::~LightContainer()
    {
    }

    bool LightContainer::operator == (const LightContainer& other) const
    {
      return m_hash == other.m_hash;
    }

    const db::Light * const LightContainer::getLightData() const
    {
      return &m_lightData;
    }

    void LightContainer::createHash()
    {
      std::vector<unsigned char> data(sizeof(m_lightData));

      unsigned int offset = 0;

      offset = convertToRawData(m_lightData, &data[0], offset);

      m_hash = MurmurHash64A(&data[0], data.size(), 0);
    }
  }
}