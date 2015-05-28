#ifndef TRIANGLEPICKINGTRAVERSER_H_
#define TRIANGLEPICKINGTRAVERSER_H_

#include <map>
#include <cassert>

#include <DataBase/ResourceManager.hpp>

#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Math/Triangle.h>

#include "SceneGraph/DLLExport.h"

#include "SceneGraph/Scene/Scene.h"

#include "SceneGraph/Traverser/Traverser.h"

namespace he
{
  namespace sg
  {
    class GRAPHICAPI TrianglePickingTraverser : public Traverser
    {
    public:

      TrianglePickingTraverser(TreeNodeAllocator& allocator, util::SingletonManager *singletonManager, util::vec3f linePoint, util::vec3f lineDirection);
      virtual ~TrianglePickingTraverser();

      virtual bool preTraverse(GeoNode& treeNode) override;

      virtual bool preTraverse(AnimatedGeoNode& treeNode) override;

      util::vec3f getCollisionPoint() const;

      util::Triangle getpickedTriangle() const;

      util::ResourceHandle getPickedMesh() const;

      NodeIndex getPickedGeoNode() const;

      void useTransformedCollisionPoint(bool useTransformedCollisionPoint);

    protected:

      TrianglePickingTraverser(const TrianglePickingTraverser&);
      TrianglePickingTraverser& operator=(const TrianglePickingTraverser&);

      bool calculateCollisionPoint(const util::Triangle& triangle, util::vec3f transformedLinePoint, util::vec3f transformedLineDirection, const sg::GeoNode& treeNode);

      util::SharedPointer<util::EventManager> m_eventManager;
      util::SharedPointer<db::ModelManager> m_modelManager;

      util::vec3f m_linePoint;
      util::vec3f m_lineDirection;

      float m_shortestDistance;
      util::vec3f m_collisionPoint;
      util::Triangle m_pickedTriangle;
      util::ResourceHandle m_pickedMeshHandle;
      NodeIndex m_pickedNode;

      bool m_useTransformedCollisionPoint;
    };
  }
}

#endif
