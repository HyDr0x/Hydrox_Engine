#include "Saver/HEFSaver.h"

#include <map>
#include <fstream>

#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <SceneGraph/Scene/Scene.h>

#include <Utilities/Miscellaneous/ResourceHandle.h>

#include "Saver/NodeExtractionTraverser.h"
#include "Saver/NodeLinkTraverser.h"

#include "Saver/MaterialSaver.h"
#include "Saver/ILDevilSaver.h"
#include "Saver/CreateDirectory.h"

namespace he
{
	namespace saver
	{
    void HEFSaver::save(std::string path, std::string filename, sg::Scene* scene, util::SingletonManager *singletonManager)
    {
      m_wrapperMapper = NodeWrapperMapper();

      std::string filenameWithoutEnding = filename.substr(0, filename.size() - 4);
      NodeExtractionTraverser extractionTraverser(filenameWithoutEnding, m_wrapperMapper, singletonManager);
      extractionTraverser.doTraverse(scene->getRootNode());

      NodeLinkTraverser linkTraverser(m_wrapperMapper);
      linkTraverser.doTraverse(scene->getRootNode());

      writeToFile(path, filename, singletonManager);
    }

    void HEFSaver::writeToFile(std::string path, std::string filename, util::SingletonManager *singletonManager)
    {
      if(path.back() != '/')
      {
        path.push_back('/');
      }

      std::ofstream fileStream;
      fileStream.open(path + filename, std::ofstream::out | std::ofstream::trunc);

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

      fileStream << m_wrapperMapper.geoNodes.size() << std::endl;
      fileStream << std::endl;
      for(unsigned int i = 0; i < m_wrapperMapper.geoNodes.size(); i++)
      {
        fileStream << m_wrapperMapper.geoNodes[i] << std::endl;
      }

      fileStream << m_wrapperMapper.animatedGeoNodes.size() << std::endl;
      fileStream << std::endl;
      for(unsigned int i = 0; i < m_wrapperMapper.animatedGeoNodes.size(); i++)
      {
        fileStream << m_wrapperMapper.animatedGeoNodes[i] << std::endl;
      }

      fileStream << m_wrapperMapper.transformNodes.size() << std::endl;
      fileStream << std::endl;
      for(unsigned int i = 0; i < m_wrapperMapper.transformNodes.size(); i++)
      {
        fileStream << m_wrapperMapper.transformNodes[i] << std::endl;
      }

      fileStream << m_wrapperMapper.animatedTransformNodes.size() << std::endl;
      fileStream << std::endl;
      for(unsigned int i = 0; i < m_wrapperMapper.animatedTransformNodes.size(); i++)
      {
        fileStream << m_wrapperMapper.animatedTransformNodes[i] << std::endl;
      }

      fileStream << m_wrapperMapper.billboardNodes.size() << std::endl;
      fileStream << std::endl;
      for(unsigned int i = 0; i < m_wrapperMapper.billboardNodes.size(); i++)
      {
        fileStream << m_wrapperMapper.billboardNodes[i] << std::endl;
      }

      fileStream << m_wrapperMapper.lodNodes.size() << std::endl;
      fileStream << std::endl;
      for(unsigned int i = 0; i < m_wrapperMapper.lodNodes.size(); i++)
      {
        fileStream << m_wrapperMapper.lodNodes[i] << std::endl;
      }

      fileStream << m_wrapperMapper.lightNodes.size() << std::endl;
      fileStream << std::endl;
      for(unsigned int i = 0; i < m_wrapperMapper.lightNodes.size(); i++)
      {
        fileStream << m_wrapperMapper.lightNodes[i] << std::endl;
      }

      fileStream << m_wrapperMapper.particleNodes.size() << std::endl;
      fileStream << std::endl;
      for(unsigned int i = 0; i < m_wrapperMapper.particleNodes.size(); i++)
      {
        fileStream << m_wrapperMapper.particleNodes[i] << std::endl;
      }

      fileStream << std::endl;

      fileStream << m_wrapperMapper.meshes.size() << std::endl;
      for(unsigned int i = 0; i < m_wrapperMapper.meshes.size(); i++)
      {
        renderer::Mesh& mesh = m_wrapperMapper.meshes[i];
        fileStream << mesh.getBBMax() << std::endl;
        fileStream << mesh.getBBMin() << std::endl;
        fileStream << mesh.getPrimitiveType() << std::endl;
        fileStream << mesh.getPrimitiveCount() << std::endl;
        fileStream << mesh.getIndexCount() << std::endl;
        fileStream << mesh.getVertexCount() << std::endl;
        fileStream << mesh.getVertexDeclarationFlags() << std::endl;
        fileStream << mesh.getVertexStride() << std::endl;
        fileStream << mesh.getVBOSize() << std::endl;
        for(unsigned int i = 0; i < mesh.getIndexCount(); i++)
        {
          fileStream << mesh.getIndexBuffer()[i] << std::endl;
        }
        for(unsigned int i = 0; i < mesh.getVBOSize(); i++)
        {
          fileStream << GLuint(mesh.getVBOBuffer()[i]) << std::endl;
        }
        fileStream << std::endl;
      }

      fileStream << std::endl;

      fileStream << m_wrapperMapper.materialFileNames.size() << std::endl;
      for(unsigned int i = 0; i < m_wrapperMapper.materialFileNames.size(); i++)
      {
        fileStream << m_wrapperMapper.materialFileNames[i] << std::string(".material") << std::endl;
      }

      for(std::map<util::ResourceHandle, unsigned int, NodeWrapperMapper::Less>::iterator it = m_wrapperMapper.materialMap.begin(); it != m_wrapperMapper.materialMap.end(); it++)
      {
        MaterialSaver::save(path, m_wrapperMapper.materialFileNames[it->second], it->first, singletonManager);
      }

      fileStream << std::endl;

      fileStream << m_wrapperMapper.billboardTextureFileNames.size() << std::endl;
      for(unsigned int i = 0; i < m_wrapperMapper.billboardTextureFileNames.size(); i++)
      {
        fileStream << m_wrapperMapper.billboardTextureFileNames[i] << std::endl << std::string(".png");
      }

      for(std::map<util::ResourceHandle, unsigned int, NodeWrapperMapper::Less>::iterator it = m_wrapperMapper.billboardTextureMap.begin(); it != m_wrapperMapper.billboardTextureMap.end(); it++)
      {
        ILDevilSaver::save(path, m_wrapperMapper.billboardTextureFileNames[it->second], it->first, singletonManager);
      }

      fileStream.close();
    }
  }
}