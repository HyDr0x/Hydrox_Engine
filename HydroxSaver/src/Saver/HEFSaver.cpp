#include "Saver/HEFSaver.h"

#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <SceneGraph/Scene/Scene.h>

#include "Saver/NodeExtractionTraverser.h"
#include "Saver/NodeLinkTraverser.h"

namespace he
{
	namespace saver
	{
    void HEFSaver::save(std::string path, std::string filename, sg::Scene* scene, util::SingletonManager *singletonManager)
    {
      NodeExtractionTraverser extractionTraverser(filename, m_wrapperMapper, singletonManager);
      extractionTraverser.doTraverse(scene->getRootNode());

      NodeLinkTraverser linkTraverser(m_wrapperMapper);
      linkTraverser.doTraverse(scene->getRootNode());
    }
  }
}