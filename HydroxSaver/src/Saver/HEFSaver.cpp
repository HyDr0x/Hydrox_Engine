#include "Saver/HEFSaver.h"

#include <map>
#include <fstream>

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

    void HEFSaver::save(std::string path, std::string filename, sg::Scene* scene, util::SingletonManager *singletonManager)
    {
      std::string filenameWithoutEnding = filename.substr(0, filename.size() - 4);
      NodeExtractionTraverser extractionTraverser(filenameWithoutEnding, m_wrapperMapper, singletonManager);
      extractionTraverser.doTraverse(scene->getRootNode());

      for(unsigned int i = 0; i < m_wrapperMapper.treeNodes.size(); i++)
      {
        m_wrapperMapper.treeNodes[i]->convertPointerToIndices(m_wrapperMapper.nodeMap);
      }

      writeToFile(path, filename, singletonManager);
    }

    void HEFSaver::writeToFile(std::string path, std::string filename, util::SingletonManager *singletonManager)
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
          std::cerr << "Error, couldn't create directory: " << path << std::endl;
          return;
        }
        else
        {
          fileStream.open(path + filename, std::ofstream::out | std::ofstream::trunc);
        }
      }

      fileStream << m_wrapperMapper.treeNodes.size() << std::endl;
      for(unsigned int i = 0; i < m_wrapperMapper.treeNodes.size(); i++)
      {
        fileStream << *m_wrapperMapper.treeNodes[i] << std::endl;
      }
      
      fileStream << m_wrapperMapper.resourceMap["Meshes"].size() << std::endl;
      for(std::map<util::ResourceHandle, std::string, NodeWrapperMapper::Less>::iterator it = m_wrapperMapper.resourceMap["Meshes"].begin(); it != m_wrapperMapper.resourceMap["Meshes"].end(); it++)
      {
        fileStream << it->second << std::endl;
        MeshSaver::save(path, it->second, it->first, singletonManager);
      }

      fileStream << std::endl;

      fileStream << m_wrapperMapper.resourceMap["Materials"].size() << std::endl;
      for(std::map<util::ResourceHandle, std::string, NodeWrapperMapper::Less>::iterator it = m_wrapperMapper.resourceMap["Materials"].begin(); it != m_wrapperMapper.resourceMap["Materials"].end(); it++)
      {
        fileStream << it->second << std::endl;
        MaterialSaver::save(path, it->second, it->first, singletonManager);
      }

      fileStream << std::endl;

      fileStream << m_wrapperMapper.resourceMap["Textures"].size() << std::endl;
      for(std::map<util::ResourceHandle, std::string, NodeWrapperMapper::Less>::iterator it = m_wrapperMapper.resourceMap["Textures"].begin(); it != m_wrapperMapper.resourceMap["Textures"].end(); it++)
      {
        fileStream << it->second << std::endl;
        ILDevilSaver::save(path, it->second, it->first, singletonManager);
      }

      fileStream.close();
    }
  }
}