#ifndef HEFSAVER_H_
#define HEFSAVER_H_

#include <string>

#include "Saver/DLLExport.h"

#include <Utilities/Miscellaneous/ResourceHandle.h>

#include <SceneGraph/Scene/TreeNodeAllocator.h>
#include <SceneGraph/TreeNodes/TreeNode.h>

namespace he
{
  namespace util
  {
    class SingletonManager;
  }

  namespace sg
  {
    class Scene;
  }

  namespace saver
  {
    class GRAPHICAPI HEFSaver
    {
    public:

      ~HEFSaver();

      void save(std::string path, std::string filename, const sg::Scene& scene, util::SingletonManager *singletonManager);

    private:

      void writeToFile(std::string path, std::string filename, const sg::Scene& scene, util::SingletonManager *singletonManager);

      std::map<std::string, std::map<util::ResourceHandle, std::string, util::Less>> m_resourceMap;//first string: resource kind e.g. mesh, billboard etc. | resource handle: for unique mapping | second string: filename
    };
  }
}

#endif
