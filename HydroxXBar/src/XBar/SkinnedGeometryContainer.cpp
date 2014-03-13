#include "XBar/SkinnedGeometryContainer.h"

namespace he
{
	namespace xBar
	{
    SkinnedGeometryContainer::SkinnedGeometryContainer(std::vector<util::Matrix<float, 4>> *boneTransformMatrices, 
                                                       std::vector<util::Matrix<float, 4>> *inverseBindPoseMatrices, 
                                                       util::Matrix<float, 4> *trfMatrix, 
                                                       util::ResourceHandle materialHandle, 
                                                       util::ResourceHandle meshHandle) : 
      StaticGeometryContainer(trfMatrix, materialHandle, meshHandle), 
      m_boneTransformMatrices(boneTransformMatrices),
      m_inverseBindPoseMatrices(inverseBindPoseMatrices)
    {
    }

    SkinnedGeometryContainer::~SkinnedGeometryContainer()
    {
    }

    SkinnedGeometryContainer* SkinnedGeometryContainer::clone()
    {
      return new SkinnedGeometryContainer(m_boneTransformMatrices, m_inverseBindPoseMatrices, m_trfMatrix, m_materialHandle, m_meshHandle);
    }

    bool SkinnedGeometryContainer::operator ==(const SkinnedGeometryContainer& o)
    {
      return m_inverseBindPoseMatrices == o.m_inverseBindPoseMatrices && m_boneTransformMatrices == o.m_boneTransformMatrices && StaticGeometryContainer::operator==(o);
    }

    std::vector<util::Matrix<float, 4>> SkinnedGeometryContainer::getSkinningMatrices()
    {
      std::vector<util::Matrix<float, 4>> skinningMatrices;
      skinningMatrices.resize(m_inverseBindPoseMatrices->size());

      for(unsigned int i = 0; i < skinningMatrices.size(); i++)
      {
        skinningMatrices[i] = (*m_boneTransformMatrices)[i] * (*m_inverseBindPoseMatrices)[i];
      }

      return skinningMatrices;
    }
  }
}