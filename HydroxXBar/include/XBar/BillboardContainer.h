#ifndef BILLBOARDCONTAINER_H_
#define BILLBOARDCONTAINER_H_

#include <Utilities/Math/Math.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>

#include "XBar/DLLExport.h"

#include "XBar/AContainer.h"

namespace he
{
  namespace xBar
  {
    class GRAPHICAPI BillboardContainer : public AContainer
    {
    public:

      BillboardContainer(util::vec3f& translation, 
                         util::vec2f& scale, 
                         util::vec2ui& animNumber, 
                         util::vec2ui& animCount, 
                         util::vec2f& texStart, 
                         util::vec2f& texEnd, 
                         util::ResourceHandle textureHandle);

      ~BillboardContainer();

      bool operator == (const BillboardContainer& other) const;

      util::ResourceHandle getTextureHandle() const;

      util::vec3f getPosition() const;
      util::vec2f getScale() const;

      util::Matrix<float, 3> getTexTransformationMatrix() const;

    private:

      void createHash();

      util::vec2ui& m_animNumber;
      util::vec2ui& m_animCount;
      util::vec2f& m_texStart;
      util::vec2f& m_texEnd;

      util::vec2f& m_scale;
      util::vec3f& m_translate;

      util::ResourceHandle m_textureHandle;
    };
  }
}

#endif