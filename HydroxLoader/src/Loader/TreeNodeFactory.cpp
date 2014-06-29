#include "Loader/TreeNodeFactory.h"

#include <sstream>
#include <fstream>
#include <iostream>

#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/PrimitiveGenerator/CubeGenerator.h>

#include <DataBase/Material.h>

#include "Loader/RenderShaderLoader.h"
#include "Loader/ILDevilLoader.h"

namespace he
{
  namespace loader
  {
    void TreeNodeFactory::registerCreateFunction(sg::NodeType nodeType, CreateTreeNode foo)
    {
      if(!m_createTreeNodeFunctions.count(nodeType))
      {
        m_createTreeNodeFunctions[nodeType] = foo;
      }
    }

    TreeNodeFactory::CreateTreeNode TreeNodeFactory::createTreeNode(sg::NodeType nodeType)
    {
      return m_createTreeNodeFunctions[nodeType];
    }
  }
}