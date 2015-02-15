#include "SceneGraph/Traverser/DebugCacheTraverser.h"

#include <Utilities/PrimitiveGenerator/DiscGenerator.h>

#include "SceneGraph/Scene/TreeNodeAllocator.h"

#include "SceneGraph/TreeNodes/TransformNode.h"
#include "SceneGraph/TreeNodes/GeoNode.h"
#include "SceneGraph/TreeNodes/AnimatedGeoNode.h"

namespace he
{
  namespace sg
  {
    DebugCacheTraverser::DebugCacheTraverser(he::util::SharedPointer<he::sg::Scene> scene, util::SingletonManager *singletonManager, util::ResourceHandle debugMaterial) : Traverser(scene->getTreeNodeAllocator()), m_scene(scene)
    {
      m_eventManager = singletonManager->getService<util::EventManager>();
      m_modelManager = singletonManager->getService<db::ModelManager>();
      m_materialManager = singletonManager->getService<db::MaterialManager>();

      m_debugMaterial = debugMaterial;

      m_inverseScaling = 1.0f;
    }

    DebugCacheTraverser::~DebugCacheTraverser()
    {
    }

    bool DebugCacheTraverser::preTraverse(TransformNode& treeNode)
    {
      m_inverseScaling *= 1.0f / treeNode.getLocalScale();

      return true;
    }

    void DebugCacheTraverser::postTraverse(TransformNode& treeNode)
    {
      m_inverseScaling *= treeNode.getLocalScale();
    }

    bool DebugCacheTraverser::preTraverse(GeoNode& treeNode)
    {
      db::Mesh *mesh = m_modelManager->getObject(treeNode.getMeshHandle());
      
      if(mesh->getCacheCount() > 0)
      {
        std::vector<util::Cache> caches = mesh->getCaches();

        /*std::vector<util::vec3f> positions(caches.size());
        std::vector<util::vec3f> normals(caches.size());
        for(unsigned int i = 0; i < caches.size(); i++)
        {
          positions[i] = caches[i].position;
          normals[i] = caches[i].normal;
        }*/

        unsigned int vertexOffset;
        std::vector<util::vec3f> positions;
        std::vector<util::vec3f> normals;
        std::vector<db::Mesh::indexType> indices;

        for(unsigned int i = 0; i < caches.size(); i++)
        {
          std::vector<util::vec3f> cacheDiscPositions;
          std::vector<util::vec3f> cacheDiscNormals;
          std::vector<db::Mesh::indexType> cacheDiscIndices;

          vertexOffset = positions.size();

          util::DiscGenerator::generateDisc(m_inverseScaling * 0.05f, caches[i].normal, cacheDiscPositions, cacheDiscIndices, cacheDiscNormals, 20);

          for(unsigned int j = 0; j < cacheDiscPositions.size(); j++)
          {
            positions.push_back(cacheDiscPositions[j] + caches[i].position + 0.01f * caches[i].normal);
            normals.push_back(cacheDiscNormals[j]);
          }

          for(unsigned int j = 0; j < cacheDiscIndices.size(); j++)
          {
            indices.push_back(cacheDiscIndices[j] + vertexOffset);
          }
        }

        std::vector<VertexElements> vertexElements;
        vertexElements.push_back(db::Mesh::MODEL_POSITION);
        vertexElements.push_back(db::Mesh::MODEL_NORMAL);

        db::Mesh mesh(GL_TRIANGLES, positions.size(), indices, vertexElements);
        mesh.copyDataIntoGeometryBuffer(db::Mesh::MODEL_POSITION, 0, positions.size(), reinterpret_cast<const GLubyte*>(&positions[0]));
        mesh.copyDataIntoGeometryBuffer(db::Mesh::MODEL_NORMAL, 0, normals.size(), reinterpret_cast<const GLubyte*>(&normals[0]));
        mesh.generateCaches(0, 0, 0, false);

        util::ResourceHandle pointHandle = m_modelManager->addObject(mesh);
        m_debugGeoNodes[treeNode.getParent()] = GeoNode(m_eventManager, pointHandle, m_debugMaterial, treeNode.getNodeName() + std::string("_AsPoints"));
      }

      return true;
    }


    bool DebugCacheTraverser::preTraverse(AnimatedGeoNode& treeNode)
    {
      db::Mesh *mesh = m_modelManager->getObject(treeNode.getMeshHandle());

      if(mesh->getCacheCount() > 0)
      {
        std::vector<util::Cache> caches = mesh->getCaches();

        /*std::vector<util::vec3f> positions(caches.size());
        std::vector<util::vec3f> normals(caches.size());
        for(unsigned int i = 0; i < caches.size(); i++)
        {
        positions[i] = caches[i].position;
        normals[i] = caches[i].normal;
        }*/

        unsigned int vertexOffset;
        std::vector<util::vec3f> positions;
        std::vector<util::vec3f> normals;
        std::vector<db::Mesh::indexType> indices;

        for(unsigned int i = 0; i < caches.size(); i++)
        {
          std::vector<util::vec3f> cacheDiscPositions;
          std::vector<util::vec3f> cacheDiscNormals;
          std::vector<db::Mesh::indexType> cacheDiscIndices;

          vertexOffset = positions.size();

          util::DiscGenerator::generateDisc(m_inverseScaling * 0.05f, caches[i].normal, cacheDiscPositions, cacheDiscIndices, cacheDiscNormals, 20);

          for(unsigned int j = 0; j < cacheDiscPositions.size(); j++)
          {
            positions.push_back(cacheDiscPositions[j] + caches[i].position + 0.001f * caches[i].normal);
            normals.push_back(cacheDiscNormals[j]);
          }

          for(unsigned int j = 0; j < cacheDiscIndices.size(); j++)
          {
            indices.push_back(cacheDiscIndices[j] + vertexOffset);
          }
        }

        std::vector<VertexElements> vertexElements;
        vertexElements.push_back(db::Mesh::MODEL_POSITION);
        vertexElements.push_back(db::Mesh::MODEL_NORMAL);

        db::Mesh mesh(GL_TRIANGLES, positions.size(), indices, vertexElements);
        mesh.copyDataIntoGeometryBuffer(db::Mesh::MODEL_POSITION, 0, positions.size(), reinterpret_cast<const GLubyte*>(&positions[0]));
        mesh.copyDataIntoGeometryBuffer(db::Mesh::MODEL_NORMAL, 0, normals.size(), reinterpret_cast<const GLubyte*>(&normals[0]));
        mesh.generateCaches(0, 0, 0, false);

        util::ResourceHandle pointHandle = m_modelManager->addObject(mesh);
        m_debugGeoNodes[treeNode.getParent()] = GeoNode(m_eventManager, pointHandle, m_debugMaterial, treeNode.getNodeName() + std::string("_AsPoints"));
      }

      return true;
    }

    void DebugCacheTraverser::applyDebugNodes()
    {
      for(std::map<sg::NodeIndex, sg::GeoNode, sg::NodeIndex::Less>::iterator it = m_debugGeoNodes.begin(); it != m_debugGeoNodes.end(); it++)
      {
        m_scene->addChildNode(it->first, it->second);
      }
    }
  }
}