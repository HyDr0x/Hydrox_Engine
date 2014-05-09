#ifndef LIGHTCONTAINER_H_
#define LIGHTCONTAINER_H_

#include <Utilities/Math/Math.hpp>

#include "XBar/DLLExport.h"

#include "XBar/AContainer.h"

namespace he
{
	namespace xBar
	{
    class GRAPHICAPI LightContainer : public AContainer
    {
    public:

      LightContainer();
      ~LightContainer();

      bool operator == (const LightContainer& o) const;

      util::Matrix<float, 4> getTransformationMatrix() const;

    private:

      void createHash();

      util::Matrix<float, 4> *m_trfMatrix;
    };
  }
}

#endif