#include "XBar/LightContainer.h"

namespace he
{
	namespace xBar
	{
    LightContainer::LightContainer()
    {
    }

    LightContainer::~LightContainer()
    {
    }

    LightContainer* LightContainer::clone()
    {
      return new LightContainer();
    }

    util::Matrix<float, 4> LightContainer::getTransformationMatrix()
    {
      return *m_trfMatrix;
    }

    void LightContainer::createHash()
    {
      std::vector<unsigned char> data(sizeof(void*));

      memcpy(&data[0], m_trfMatrix, sizeof(void*));

      m_hash = MurmurHash64A(&data[0], data.size(), 0);
    }
  }
}