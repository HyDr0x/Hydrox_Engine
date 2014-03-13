#ifndef LIGHTCONTAINER_H_
#define LIGHTCONTAINER_H_

#include <Utilities/Math/Math.hpp>

#include "XBar/DLLExport.h"

namespace he
{
	namespace xBar
	{
    class GRAPHICAPI LightContainer
    {
    public:

      LightContainer();
      ~LightContainer();

      LightContainer* clone();

      bool operator ==(const LightContainer& o);

      util::Matrix<float, 4> getTransformationMatrix();

    private:

      util::Matrix<float, 4> m_trfMatrix;
    };
  }
}

#endif