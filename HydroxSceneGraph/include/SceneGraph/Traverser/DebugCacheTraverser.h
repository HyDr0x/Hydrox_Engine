#ifndef DEBUGCACHETRAVERSER_H_
#define DEBUGCACHETRAVERSER_H_

#include <cassert>

#include <DataBase/ResourceManager.hpp>

#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>

#include "SceneGraph/DLLExport.h"

#include "SceneGraph/Scene/Scene.h"

#include "SceneGraph/Traverser/ConstTraverser.h"

namespace he
{
  namespace sg
  {
    class GRAPHICAPI DebugCacheTraverser : public ConstTraverser
    {
    public:

      DebugCacheTraverser(const TreeNodeAllocator& allocator, util::SingletonManager *singletonManager, util::ResourceHandle debugMaterial);
      virtual ~DebugCacheTraverser();

      virtual bool preTraverse(const GeoNode& treeNode) override;

      virtual bool preTraverse(const AnimatedGeoNode& treeNode) override;

      const Scene& getDebugScene() const;

    protected:

      DebugCacheTraverser(const DebugCacheTraverser&);
      DebugCacheTraverser& operator=(const DebugCacheTraverser&);

      util::SharedPointer<util::EventManager> m_eventManager;
      util::SharedPointer<db::ModelManager> m_modelManager;
      util::SharedPointer<db::MaterialManager> m_materialManager;

      util::ResourceHandle m_debugMaterial;
      NodeIndex m_rootNodeIndex;
      Scene m_scene;
    };
  }
}

#endif
