#ifndef DEBUGCACHETRAVERSER_H_
#define DEBUGCACHETRAVERSER_H_

#include <map>
#include <cassert>

#include <DataBase/ResourceManager.hpp>

#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>

#include "SceneGraph/DLLExport.h"

#include "SceneGraph/Scene/Scene.h"

#include "SceneGraph/Traverser/Traverser.h"

namespace he
{
  namespace sg
  {
    class GRAPHICAPI DebugCacheTraverser : public Traverser
    {
    public:

      DebugCacheTraverser(he::util::SharedPointer<he::sg::Scene> scene, util::SingletonManager *singletonManager, util::ResourceHandle debugMaterial);
      virtual ~DebugCacheTraverser();

      virtual bool preTraverse(TransformNode& treeNode) override;
      virtual void postTraverse(TransformNode& treeNode) override;

      virtual bool preTraverse(GeoNode& treeNode) override;

      virtual bool preTraverse(AnimatedGeoNode& treeNode) override;

      void applyDebugNodes();

    protected:

      DebugCacheTraverser(const DebugCacheTraverser&);
      DebugCacheTraverser& operator=(const DebugCacheTraverser&);

      util::SharedPointer<util::EventManager> m_eventManager;
      util::SharedPointer<db::ModelManager> m_modelManager;
      util::SharedPointer<db::MaterialManager> m_materialManager;

      util::ResourceHandle m_debugMaterial;

      float m_inverseScaling;

      he::util::SharedPointer<he::sg::Scene> m_scene;

      std::map<he::sg::NodeIndex, he::sg::GeoNode, he::sg::NodeIndex::Less> m_debugGeoNodes;
    };
  }
}

#endif
