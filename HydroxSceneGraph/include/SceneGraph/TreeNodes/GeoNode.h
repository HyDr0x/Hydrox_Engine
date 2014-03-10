#ifndef GEONODE_H_
#define GEONODE_H_

#include "SceneGraph/DLLExport.h"

#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Signals/EventManager.h>
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

      GeoNode(util::EventManager *eventManager, util::ResourceHandle meshHandle, util::ResourceHandle materialHandle, bool transparency, const std::string& nodeName, GroupNode* parent = nullptr, TreeNode* nextSibling = nullptr);
      GeoNode& operator=(const GeoNode& sourceNode);
      virtual TreeNode& operator=(const TreeNode& sourceNode);
      virtual ~GeoNode();

      virtual TreeNode* clone() const;

      virtual bool ascendTraverse(Traverser* traverser);
      virtual bool preTraverse(Traverser* traverser);
      virtual void postTraverse(Traverser* traverser);

      virtual bool isGeoNode();

      void setMeshHandle(util::ResourceHandle meshHandle);
      util::ResourceHandle getMeshHandle() const;

      void setMaterialHandle(util::ResourceHandle materialHandle);
      util::ResourceHandle getMaterialHandle() const;

      util::Matrix<float, 4> getTransformationMatrix() const;
      void setTransformationMatrix(const util::Matrix<float, 4>& trfMatrix);

      void setRenderable(bool renderable);
	    bool getRenderable() const;

	    bool getTransparency() const;

    protected:

      util::EventManager *m_eventManager;

      util::Matrix<float, 4> m_trfMatrix;//the tranformation util::Matrix

      util::ResourceHandle m_materialHandle;
      util::ResourceHandle m_meshHandle;//index which points into the array of the modelmanager

      bool m_renderable;//boolean which decides if the mesh is being drawn or not
      bool m_transparency;
    };
	}
}

#endif
