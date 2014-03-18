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
      createHash();
    }

    SkinnedGeometryContainer::~SkinnedGeometryContainer()
    {
    }

    bool SkinnedGeometryContainer::operator == (const SkinnedGeometryContainer& o)
    {
      return m_hash == o.m_hash;
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

    void SkinnedGeometryContainer::createHash()
    {
      std::vector<unsigned char> data(sizeof(void*) * 3 + sizeof(unsigned int) * 2);

      unsigned int id;

      memcpy(&data[0], &m_trfMatrix, sizeof(void*));
      memcpy(&data[sizeof(void*) * 1], &m_boneTransformMatrices, sizeof(void*));
      memcpy(&data[sizeof(void*) * 2], &m_inverseBindPoseMatrices, sizeof(void*));
      id = m_materialHandle.getID();
      memcpy(&data[sizeof(void*) * 3], &id, sizeof(unsigned int));
      id = m_meshHandle.getID();
      memcpy(&data[sizeof(void*) * 3 + sizeof(unsigned int)], &id, sizeof(unsigned int));
      
      m_hash = MurmurHash64A(&data[0], data.size(), 0);
    }
  }
}