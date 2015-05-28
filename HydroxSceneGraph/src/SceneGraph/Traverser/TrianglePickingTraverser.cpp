#include "SceneGraph/Traverser/TrianglePickingTraverser.h"

#include <Utilities/PrimitiveGenerator/DiscGenerator.h>

#include <DataBase/Mesh.h>

#include "SceneGraph/Scene/TreeNodeAllocator.h"

#include "SceneGraph/TreeNodes/TransformNode.h"
#include "SceneGraph/TreeNodes/GeoNode.h"
#include "SceneGraph/TreeNodes/AnimatedGeoNode.h"

namespace he
{
  namespace sg
  {
    TrianglePickingTraverser::TrianglePickingTraverser(TreeNodeAllocator& allocator, util::SingletonManager *singletonManager, util::vec3f linePoint, util::vec3f lineDirection) :
      Traverser(allocator),
      m_linePoint(linePoint),
      m_lineDirection(lineDirection),
      m_pickedTriangle(util::vec3f::identity(), util::vec3f::identity(), util::vec3f::identity()),
      m_shortestDistance(FLT_MAX),
      m_useTransformedCollisionPoint(false)
    {
      m_eventManager = singletonManager->getService<util::EventManager>();
      m_modelManager = singletonManager->getService<db::ModelManager>();
    }

    TrianglePickingTraverser::~TrianglePickingTraverser()
    {
    }

    bool TrianglePickingTraverser::preTraverse(GeoNode& treeNode)
    {
      db::Mesh *mesh = m_modelManager->getObject(treeNode.getMeshHandle());

      util::Matrix<float, 4> inversemat = treeNode.getTransformationMatrix().invert();

      util::vec3f transformedLinePoint = inversemat * util::vec4f(m_linePoint[0], m_linePoint[1], m_linePoint[2], 1.0f);
      util::vec3f transformedLineDirection = (inversemat * util::vec4f(m_lineDirection[0], m_lineDirection[1], m_lineDirection[2], 0.0f)).normalize();

      if(mesh->getBoundingVolume().collisionLine(transformedLinePoint, transformedLineDirection))
      {
        bool collided = false;
        unsigned int vertexStride = mesh->getVertexStride();

        std::vector<GLubyte> vboData = mesh->getVBOBuffer();
        std::vector<db::Mesh::indexType> indices = mesh->getIndexBuffer();

        std::vector<util::vec3f> positions;

        for(unsigned int i = 0; i < mesh->getVertexCount(); i++)
        {
          positions.push_back(*reinterpret_cast<util::vec3f*>(&vboData[i * vertexStride]));
        }

        if(!indices.empty())
        {
          for(unsigned int i = 0; i < indices.size(); i += 3)
          {
            util::Triangle triangle(positions[indices[i]], positions[indices[i + 1]], positions[indices[i + 2]]);

            if(calculateCollisionPoint(triangle, transformedLinePoint, transformedLineDirection, treeNode))
            {
              collided = true;
            }
          }
        }
        else
        {
          for(unsigned int i = 0; i < positions.size(); i += 3)
          {
            util::Triangle triangle(positions[i], positions[i + 1], positions[i + 2]);

            if(calculateCollisionPoint(triangle, transformedLinePoint, transformedLineDirection, treeNode))
            {
              collided = true;
            }
          }
        }
      }

      return true;
    }

    bool TrianglePickingTraverser::preTraverse(AnimatedGeoNode& treeNode)
    {
      return preTraverse(dynamic_cast<GeoNode&>(treeNode));
    }

    util::vec3f TrianglePickingTraverser::getCollisionPoint() const
    {
      return m_collisionPoint;
    }

    util::Triangle TrianglePickingTraverser::getpickedTriangle() const
    {
      return m_pickedTriangle;
    }

    util::ResourceHandle TrianglePickingTraverser::getPickedMesh() const
    {
      return m_pickedMeshHandle;
    }

    NodeIndex TrianglePickingTraverser::getPickedGeoNode() const
    {
      return m_pickedNode;
    }

    void TrianglePickingTraverser::useTransformedCollisionPoint(bool useTransformedCollisionPoint)
    {
      m_useTransformedCollisionPoint = useTransformedCollisionPoint;
    }

    bool TrianglePickingTraverser::calculateCollisionPoint(const util::Triangle& triangle, util::vec3f transformedLinePoint, util::vec3f transformedLineDirection, const sg::GeoNode& treeNode)
    {
      util::vec3f output;
      if(triangle.collisionLine(transformedLinePoint, transformedLineDirection, output) && util::vec3f::dot(triangle.getNormal(), -1.0f * transformedLineDirection) > 0.0f)
      {
        util::vec3f collisionPoint = transformedLinePoint + output[0] * transformedLineDirection;
        util::vec3f transformedCollisionPoint = treeNode.getTransformationMatrix() * collisionPoint;
        float distance = (transformedCollisionPoint - m_linePoint).length();

        if(distance < m_shortestDistance && output[0] > 0.0f)
        {
          m_shortestDistance = distance;
          m_collisionPoint = m_useTransformedCollisionPoint ? transformedCollisionPoint : collisionPoint;
          m_pickedTriangle = triangle;
          m_pickedMeshHandle = treeNode.getMeshHandle();
          m_pickedNode = treeNode.getNodeIndex();

          return true;
        }
      }

      return false;
    }
  }
}