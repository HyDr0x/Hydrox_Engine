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

namespace he
{
	namespace saver
	{
    void HEFSaver::save(std::string path, std::string filename, sg::Scene* scene, util::SingletonManager *singletonManager)
    {
      m_wrapperMapper = NodeWrapperMapper();

      NodeExtractionTraverser extractionTraverser(filename, m_wrapperMapper, singletonManager);
      extractionTraverser.doTraverse(scene->getRootNode());

      NodeLinkTraverser linkTraverser(m_wrapperMapper);
      linkTraverser.doTraverse(scene->getRootNode());

      writeToFile(path, filename, singletonManager);
    }

    void HEFSaver::writeToFile(std::string path, std::string filename, util::SingletonManager *singletonManager)
    {
      std::ofstream fileStream;
      fileStream.open(path + filename + std::string(".hef"), std::ofstream::out | std::ofstream::trunc);

      fileStream << m_wrapperMapper.geoNodes.size() << std::endl;
      for(unsigned int i = 0; i < m_wrapperMapper.geoNodes.size(); i++)
      {
        fileStream << m_wrapperMapper.geoNodes[i] << std::endl;
      }

      fileStream << m_wrapperMapper.animatedGeoNodes.size() << std::endl;
      for(unsigned int i = 0; i < m_wrapperMapper.animatedGeoNodes.size(); i++)
      {
        fileStream << m_wrapperMapper.animatedGeoNodes[i] << std::endl;
      }

      fileStream << m_wrapperMapper.transformNodes.size() << std::endl;
      for(unsigned int i = 0; i < m_wrapperMapper.transformNodes.size(); i++)
      {
        fileStream << m_wrapperMapper.transformNodes[i] << std::endl;
      }

      fileStream << m_wrapperMapper.animatedTransformNodes.size() << std::endl;
      for(unsigned int i = 0; i < m_wrapperMapper.animatedTransformNodes.size(); i++)
      {
        fileStream << m_wrapperMapper.animatedTransformNodes[i] << std::endl;
      }

      fileStream << m_wrapperMapper.billboardNodes.size() << std::endl;
      for(unsigned int i = 0; i < m_wrapperMapper.billboardNodes.size(); i++)
      {
        fileStream << m_wrapperMapper.billboardNodes[i] << std::endl;
      }

      fileStream << m_wrapperMapper.lodNodes.size() << std::endl;
      for(unsigned int i = 0; i < m_wrapperMapper.lodNodes.size(); i++)
      {
        fileStream << m_wrapperMapper.lodNodes[i] << std::endl;
      }

      fileStream << m_wrapperMapper.lightNodes.size() << std::endl;
      for(unsigned int i = 0; i < m_wrapperMapper.lightNodes.size(); i++)
      {
        fileStream << m_wrapperMapper.lightNodes[i] << std::endl;
      }

      fileStream << m_wrapperMapper.particleNodes.size() << std::endl;
      for(unsigned int i = 0; i < m_wrapperMapper.particleNodes.size(); i++)
      {
        fileStream << m_wrapperMapper.particleNodes[i] << std::endl;
      }

      fileStream << std::endl;

      fileStream << m_wrapperMapper.meshMap.size() << std::endl;
      for(std::map<util::ResourceHandle, unsigned int, NodeWrapperMapper::Less>::iterator it = m_wrapperMapper.meshMap.begin(); it != m_wrapperMapper.meshMap.end(); it++)
      {
        renderer::Mesh& mesh = m_wrapperMapper.meshes[m_wrapperMapper.meshMap[it->first]];
        fileStream << mesh.getBBMax();
        fileStream << mesh.getBBMin();
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
          fileStream << mesh.getVBOBuffer()[i] << std::endl;
        }
        fileStream << std::endl;
      }

      fileStream << std::endl;

      fileStream << m_wrapperMapper.materialMap.size() << std::endl;
      for(std::map<util::ResourceHandle, unsigned int, NodeWrapperMapper::Less>::iterator it = m_wrapperMapper.materialMap.begin(); it != m_wrapperMapper.materialMap.end(); it++)
      {
        fileStream << m_wrapperMapper.materialFileNames[it->second] << std::string(".material") << std::endl;
        MaterialSaver::save(path, m_wrapperMapper.materialFileNames[it->second], it->first, singletonManager);
      }

      fileStream << std::endl;

      fileStream << m_wrapperMapper.billboardTextureMap.size() << std::endl;
      for(std::map<util::ResourceHandle, unsigned int, NodeWrapperMapper::Less>::iterator it = m_wrapperMapper.billboardTextureMap.begin(); it != m_wrapperMapper.billboardTextureMap.end(); it++)
      {
        fileStream << m_wrapperMapper.billboardTextureFileNames[it->second] << std::endl << std::string(".png");
        ILDevilSaver::save(path, m_wrapperMapper.billboardTextureFileNames[it->second], it->first, singletonManager);
      }

      fileStream.close();
    }
  }
}