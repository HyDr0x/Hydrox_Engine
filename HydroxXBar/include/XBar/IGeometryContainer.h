#ifndef IGEOMETRYCONTAINER_H_
#define IGEOMETRYCONTAINER_H_

#include <Utilities/Math/Math.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Miscellaneous/Flags.hpp>

#include "XBar/DLLExport.h"
#include "XBar/AContainer.h"

namespace he
{
  namespace xBar
  {
    enum GRAPHICAPI RenderNodeType
    {
      SKINNEDNODE = 1,
      INDEXEDNODE = 2,
    };

    class GRAPHICAPI IGeometryContainer : public AContainer
    {
    public:

      IGeometryContainer(util::Flags<RenderNodeType> nodeType, util::Matrix<float, 4> *trfMatrix, util::ResourceHandle materialHandle, util::ResourceHandle meshHandle) :
        m_nodeType(nodeType),
        m_trfMatrix(trfMatrix),
        m_materialHandle(materialHandle),
        m_meshHandle(meshHandle)
      {
      }

      virtual ~IGeometryContainer() = 0
      {
      }

      bool IGeometryContainer::operator == (const IGeometryContainer& other) const
      {
        return m_hash == other.getHash();
      }

      util::ResourceHandle IGeometryContainer::getMaterialHandle() const
      {
        return m_materialHandle;
      }

      util::ResourceHandle IGeometryContainer::getMeshHandle() const
      {
        return m_meshHandle;
      }

      util::Matrix<float, 4> IGeometryContainer::getTransformationMatrix() const
      {
        return *m_trfMatrix;
      }

      util::Flags<RenderNodeType> IGeometryContainer::getNodeType() const
      {
        return m_nodeType;
      }

    protected:

      virtual void createHash() = 0;

      util::Flags<RenderNodeType> m_nodeType;

      util::Matrix<float, 4> *m_trfMatrix;

      util::ResourceHandle m_materialHandle;
      util::ResourceHandle m_meshHandle;
    };
  }
}

#endif