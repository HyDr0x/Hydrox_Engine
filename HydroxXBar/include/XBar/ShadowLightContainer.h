#ifndef SHADOWLIGHTCONTAINER_H_
#define SHADOWLIGHTCONTAINER_H_

#include <Utilities/Math/Math.hpp>

#include "XBar/DLLExport.h"

#include "XBar/AContainer.h"

namespace he
{
  namespace db
  {
    struct ShadowLight;
  }

  namespace xBar
  {
    class GRAPHICAPI ShadowLightContainer : public AContainer
    {
    public:

      ShadowLightContainer(const db::ShadowLight& lightData, bool reflectiveShadow);
      ~ShadowLightContainer();

      bool operator == (const ShadowLightContainer& other) const;

      const db::ShadowLight * const getLightData() const;

      bool isReflectiveShadow() const;

    private:

      void createHash();

      const db::ShadowLight& m_lightData;
      bool m_reflectiveShadow;
    };
  }
}

#endif