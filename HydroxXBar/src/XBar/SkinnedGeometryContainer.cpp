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

    std::vector<util::Matrix<float, 4>>* SkinnedGeometryContainer::getBoneTransformMatrices()
    {
      return m_boneTransformMatrices;
    }

    std::vector<util::Matrix<float, 4>>* SkinnedGeometryContainer::getInverseBindPoseTransformMatrices()
    {
      return m_inverseBindPoseMatrices;
    }

    void SkinnedGeometryContainer::createHash()
    {
      unsigned int id;

      std::vector<unsigned char> data(sizeof(m_trfMatrix) + sizeof(id) + sizeof(id) + sizeof(m_boneTransformMatrices) + sizeof(m_inverseBindPoseMatrices));

      std::copy((unsigned char*)&m_trfMatrix, (unsigned char*)&m_trfMatrix + sizeof(m_trfMatrix), &data[0]);

      id = m_materialHandle.getID();

      std::copy((unsigned char*)&id, (unsigned char*)&id + sizeof(id), &data[0] + sizeof(m_trfMatrix));

      id = m_meshHandle.getID();

      std::copy((unsigned char*)&id, (unsigned char*)&id + sizeof(id), &data[0] + sizeof(m_trfMatrix) + sizeof(id));
      std::copy((unsigned char*)m_boneTransformMatrices, (unsigned char*)m_boneTransformMatrices + sizeof(m_boneTransformMatrices), &data[0] + sizeof(m_trfMatrix) + sizeof(id) + sizeof(id));
      std::copy((unsigned char*)m_inverseBindPoseMatrices, (unsigned char*)m_inverseBindPoseMatrices + sizeof(m_inverseBindPoseMatrices), &data[0] + sizeof(m_trfMatrix) + sizeof(id) + sizeof(id) + sizeof(m_boneTransformMatrices));
      
      m_hash = MurmurHash64A(&data[0], data.size(), 0);
    }
  }
}