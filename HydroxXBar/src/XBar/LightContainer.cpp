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

    bool LightContainer::operator ==(const LightContainer& o)
    {
      return true;
    }

    util::Matrix<float, 4> LightContainer::getTransformationMatrix()
    {
      return m_trfMatrix;
    }
  }
}