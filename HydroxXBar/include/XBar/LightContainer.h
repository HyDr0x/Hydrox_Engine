#ifndef LIGHTCONTAINER_H_
#define LIGHTCONTAINER_H_

#include <Utilities/Math/Math.hpp>

#include "XBar/DLLExport.h"

#include "XBar/AContainer.h"

namespace he
{
  namespace db
  {
    class Light;
  }

  namespace xBar
  {
    class GRAPHICAPI LightContainer : public AContainer
    {
    public:

      LightContainer(const db::Light& lightData);
      ~LightContainer();

      bool operator == (const LightContainer& other) const;

      const db::Light * const getLightData() const;

    private:

      void createHash();

      const db::Light& m_lightData;
    };
  }
}

#endif