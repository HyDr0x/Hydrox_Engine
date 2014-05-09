#ifndef SKINNEDGEOMETRYCONTAINER_H_
#define SKINNEDGEOMETRYCONTAINER_H_

#include <vector>

#include "XBar/StaticGeometryContainer.h"

#include "XBar/DLLExport.h"

namespace he
{
	namespace xBar
	{
    class GRAPHICAPI SkinnedGeometryContainer : public StaticGeometryContainer
    {
    public:

      SkinnedGeometryContainer(std::vector<util::Matrix<float, 4>> *boneTransformMatrices, 
                               std::vector<util::Matrix<float, 4>> *inverseBindPoseMatrices, 
                               util::Matrix<float, 4> *trfMatrix, 
                               util::ResourceHandle materialHandle, 
                               util::ResourceHandle meshHandle);

      virtual ~SkinnedGeometryContainer();

      bool operator == (const SkinnedGeometryContainer& o) const;

      std::vector<util::Matrix<float, 4>>* getBoneTransformMatrices() const;
      std::vector<util::Matrix<float, 4>>* getInverseBindPoseTransformMatrices() const;

    private:

      virtual void createHash();

      std::vector<util::Matrix<float, 4>> *m_boneTransformMatrices;
      std::vector<util::Matrix<float, 4>> *m_inverseBindPoseMatrices;
    };
  }
}

#endif