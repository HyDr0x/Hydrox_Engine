#include "XBar/SkinnedGeometryContainer.h"

namespace he
{
  namespace xBar
  {
    SkinnedGeometryContainer::SkinnedGeometryContainer(std::vector<util::Matrix<float, 4>> *boneTransformMatrices,
                                                       std::vector<util::Matrix<float, 4>> *inverseBindPoseMatrices, 
                                                       util::Flags<RenderNodeType> nodeType,
                                                       util::Matrix<float, 4> *trfMatrix, 
                                                       util::ResourceHandle materialHandle, 
                                                       util::ResourceHandle meshHandle) : 
                                                       IGeometryContainer(nodeType, trfMatrix, materialHandle, meshHandle),
      m_boneTransformMatrices(boneTransformMatrices),
      m_inverseBindPoseMatrices(inverseBindPoseMatrices)
    {
      createHash();
    }

    SkinnedGeometryContainer::~SkinnedGeometryContainer()
    {
    }

    bool SkinnedGeometryContainer::operator == (const IGeometryContainer& other) const
    {
      return m_hash == other.getHash();
    }

    std::vector<util::Matrix<float, 4>>* SkinnedGeometryContainer::getBoneTransformMatrices() const
    {
      return m_boneTransformMatrices;
    }

    std::vector<util::Matrix<float, 4>>* SkinnedGeometryContainer::getInverseBindPoseTransformMatrices() const
    {
      return m_inverseBindPoseMatrices;
    }

    void SkinnedGeometryContainer::createHash()
    {
      unsigned int id;

      std::vector<unsigned char> data(sizeof(m_trfMatrix) + sizeof(id) + sizeof(id) + sizeof(m_boneTransformMatrices) + sizeof(m_inverseBindPoseMatrices));

      unsigned int offset = 0;
      offset = convertToRawData(m_trfMatrix, &data[0], offset);
      id = m_meshHandle.getID();
      offset = convertToRawData(id, &data[0], offset);
      id = m_materialHandle.getID();
      offset = convertToRawData(id, &data[0], offset);
      offset = convertToRawData(m_boneTransformMatrices, &data[0], offset);
      offset = convertToRawData(m_inverseBindPoseMatrices, &data[0], offset);

      m_hash = MurmurHash64A(&data[0], data.size(), 0);
    }
  }
}