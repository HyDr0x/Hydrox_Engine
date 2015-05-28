#include "Saver/HEFSaver.h"

#include <map>
#include <fstream>
#include <iostream>

#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>

#include <SceneGraph/Scene/Scene.h>
#include <SceneGraph/TreeNodes/GroupNode.h>

#include "Saver/NodeExtractionTraverser.h"

#include "Saver/MeshSaver.h"
#include "Saver/MaterialSaver.h"
#include "Saver/ILDevilSaver.h"
#include "Saver/CreateDirectory.h"

namespace he
{
  namespace saver
  {
    HEFSaver::~HEFSaver()
    {
    }

    void HEFSaver::save(std::string path, std::string filename, const sg::Scene& scene, util::SingletonManager *singletonManager)
    {
      std::string filenameWithoutEnding = filename.substr(0, filename.size() - 4);
      NodeExtractionTraverser extractionTraverser(scene.getTreeNodeAllocator(), filenameWithoutEnding, m_resourceMap);
      extractionTraverser.doTraverse(scene.getTreeNodeAllocator()[scene.getRootNode()]);

      writeToFile(path, filename, scene, singletonManager);
    }

    void HEFSaver::writeToFile(std::string path, std::string filename, const sg::Scene& scene, util::SingletonManager *singletonManager)
    {
      if(path.back() != '/')
      {
        path.push_back('/');
      }

      std::ofstream fileStream;
      fileStream.open(path + filename, std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);

      if(!fileStream.is_open())
      {
        if(!createDirectory(path))
        {
          std::clog << "Error, couldn't create directory: " << path << std::endl;
          return;
        }
        else
        {
          fileStream.open(path + filename, std::ofstream::out | std::ofstream::trunc);
        }
      }
      
      fileStream << m_resourceMap["Meshes"].size() << std::endl;
      for(std::map<util::ResourceHandle, std::string, util::Less>::iterator it = m_resourceMap["Meshes"].begin(); it != m_resourceMap["Meshes"].end(); it++)
      {
        fileStream << it->second << std::endl;
        
        MeshSaver::save(path, it->second, it->first, singletonManager);
      }

      fileStream << std::endl;

      fileStream << m_resourceMap["Materials"].size() << std::endl;
      for(std::map<util::ResourceHandle, std::string, util::Less>::iterator it = m_resourceMap["Materials"].begin(); it != m_resourceMap["Materials"].end(); it++)
      {
        fileStream << it->second << std::endl;

        MaterialSaver::save(path, it->second, it->first, singletonManager);
      }

      fileStream << std::endl;

      fileStream << m_resourceMap["Textures"].size() << std::endl;
      for(std::map<util::ResourceHandle, std::string, util::Less>::iterator it = m_resourceMap["Textures"].begin(); it != m_resourceMap["Textures"].end(); it++)
      {
        fileStream << it->second << std::endl;

        ILDevilSaver::save(path, it->second, it->first, singletonManager);
      }

      write(fileStream, scene.getTreeNodeAllocator(), singletonManager);

      fileStream << scene.getRootNode().nodeType << std::endl;
      fileStream << scene.getRootNode().index << std::endl;

      fileStream.close();
    }

    void HEFSaver::write(std::ostream& stream, const sg::TreeNodeAllocator& allocator, util::SingletonManager *singletonManager)
    {
      stream << (unsigned int)sg::NODENUMBER << std::endl;

      unsigned int nodeBlockSize = allocator.getNodeBlockSize();
      stream << nodeBlockSize << std::endl;

      for(unsigned int k = 0; k < sg::NODENUMBER; k++)
      {
        unsigned int nodeBlockNumber = allocator.getNodeBlockNumber((sg::NodeType)k);
        stream << nodeBlockNumber << std::endl;

        stream << allocator.getExactNodeBlockSize((sg::NodeType)k) << std::endl;

        for(unsigned int i = 0; i < nodeBlockNumber; i++)
        {
          for(unsigned int j = 0; j < nodeBlockSize; j++)
          {
            sg::NodeIndex index = sg::NodeIndex(i * nodeBlockSize + j, (sg::NodeType)k);
            if(allocator.contains(index))
            {
              allocator[index].write(stream, m_resourceMap);
            }
          }
        }
        stream << std::endl;
      }
    }
  }
}