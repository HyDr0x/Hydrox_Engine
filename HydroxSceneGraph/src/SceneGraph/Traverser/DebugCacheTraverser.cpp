#include "SceneGraph/Traverser/DebugCacheTraverser.h"

#include "SceneGraph/Scene/TreeNodeAllocator.h"

#include "SceneGraph/TreeNodes/TransformNode.h"
#include "SceneGraph/TreeNodes/GeoNode.h"
#include "SceneGraph/TreeNodes/AnimatedGeoNode.h"

namespace he
{
  namespace sg
  {
    DebugCacheTraverser::DebugCacheTraverser(const TreeNodeAllocator& allocator, util::SingletonManager *singletonManager, util::ResourceHandle debugMaterial) : ConstTraverser(allocator), m_scene(TreeNodeAllocator(), NodeIndex())
    {
      m_eventManager = singletonManager->getService<util::EventManager>();
      m_modelManager = singletonManager->getService<db::ModelManager>();
      m_materialManager = singletonManager->getService<db::MaterialManager>();

      m_debugMaterial = debugMaterial;

      TreeNodeAllocator newAllocator;
      NodeIndex rootNodeIndex = newAllocator.insert(TransformNode(util::Matrix<float, 4>::identity(), std::string("rootNode")));
      m_scene = Scene(newAllocator, rootNodeIndex);
      m_rootNodeIndex = m_scene.getRootNode();
    }

    DebugCacheTraverser::~DebugCacheTraverser()
    {
    }

    bool DebugCacheTraverser::preTraverse(const GeoNode& treeNode)
    {
      db::Mesh *mesh = m_modelManager->getObject(treeNode.getMeshHandle());
      
      if(mesh->getCacheCount() > 0)
      {
        std::vector<util::Cache> caches = mesh->getCaches();
        std::vector<util::vec3f> positions(caches.size());
        std::vector<util::vec3f> normals(caches.size());
        for(unsigned int i = 0; i < caches.size(); i++)
        {
          positions[i] = caches[i].position;
          normals[i] = caches[i].normal;
        }

        he::util::ResourceHandle pointHandle = m_modelManager->addObject(he::db::Mesh(GL_POINTS, positions, std::vector<util::Cache>(), std::vector<util::vec2ui>(), std::vector<db::Mesh::indexType>(), std::vector<std::vector<util::vec2f>>(4), normals));
        GeoNode geoNode(m_eventManager, pointHandle, m_debugMaterial, treeNode.getNodeName() + std::string("_AsPoints"));
        m_scene.addChildNode(m_rootNodeIndex, geoNode);
      }

      return true;
    }

    bool DebugCacheTraverser::preTraverse(const AnimatedGeoNode& treeNode)
    {
      db::Mesh *mesh = m_modelManager->getObject(treeNode.getMeshHandle());
      std::vector<util::Cache> caches = mesh->getCaches();

      std::vector<util::vec3f> positions(caches.size());
      std::vector<util::vec3f> normals(caches.size());
      for(unsigned int i = 0; i < caches.size(); i++)
      {
        positions[i] = caches[i].position;
        normals[i] = caches[i].normal;
      }

      he::util::ResourceHandle pointHandle = m_modelManager->addObject(he::db::Mesh(GL_POINTS, positions, std::vector<util::Cache>(), std::vector<util::vec2ui>(), std::vector<db::Mesh::indexType>(), std::vector<std::vector<util::vec2f>>(4), normals));
      GeoNode geoNode(m_eventManager, pointHandle, m_debugMaterial, treeNode.getNodeName() + std::string("_AsPoints"));
      m_scene.addChildNode(m_rootNodeIndex, geoNode);

      return true;
    }

    const Scene& DebugCacheTraverser::getDebugScene() const
    {
      return m_scene;
    }
  }
}