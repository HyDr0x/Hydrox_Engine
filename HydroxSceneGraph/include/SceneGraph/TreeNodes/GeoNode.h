#ifndef GEONODE_H_
#define GEONODE_H_

#include "SceneGraph/DLLExport.h"

#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Math/Math.hpp>
#include "SceneGraph/TreeNodes/TreeNode.h"

namespace he
{
	namespace sg
	{
    class Traverser;

    class GRAPHICAPI GeoNode : public TreeNode
    {
    public:

      GeoNode(util::ResourceHandle meshIndex, util::ResourceHandle materialIndex, bool renderable, const std::string& nodeName, GroupNode* parent = nullptr, TreeNode* nextSibling = nullptr);
      GeoNode& operator=(const GeoNode& sourceNode);
      virtual TreeNode& operator=(const TreeNode& sourceNode);
      virtual ~GeoNode();

      virtual TreeNode* clone() const;

      virtual bool ascendTraverse(Traverser* traverser);
      virtual bool preTraverse(Traverser* traverser);
      virtual void postTraverse(Traverser* traverser);

      virtual GeoNode* getNodeType();

      void setMeshIndex(util::ResourceHandle meshIndex);
      util::ResourceHandle getMeshIndex() const;

      void setMaterial(util::ResourceHandle materialIndex);
      util::ResourceHandle getMaterial() const;

      util::Matrix<float, 4> getTransformationMatrix() const;
      void setTransformationMatrix(const util::Matrix<float, 4>& trfMatrix);

      void setRenderable(bool renderable);
	    bool getRenderable() const;

    protected:

      util::Matrix<float, 4> m_trfMatrix;//the tranformation util::Matrix

      util::ResourceHandle m_materialIndex;
      util::ResourceHandle m_meshIndex;//index which points into the array of the modelmanager

      bool m_renderable;//boolean which decides if the mesh is being drawn or not
    };
	}
}

#endif
