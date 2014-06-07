#ifndef LIGHTCONTAINER_H_
#define LIGHTCONTAINER_H_

#include <Utilities/Math/Math.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>

#include "XBar/DLLExport.h"

#include "XBar/AContainer.h"

namespace he
{
  namespace xBar
  {
    class GRAPHICAPI LightContainer : public AContainer
    {
    public:

      LightContainer(util::Matrix<float, 4> *trfMatrix, util::ResourceHandle lightHandle);
      ~LightContainer();

      bool operator == (const LightContainer& other) const;

      util::ResourceHandle getLightHandle() const;
      util::Matrix<float, 4> getTransformationMatrix() const;

    private:

      void createHash();

      util::Matrix<float, 4> *m_trfMatrix;
      util::ResourceHandle m_lightHandle;
    };
  }
}

#endif