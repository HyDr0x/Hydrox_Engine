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
    HEFSaver::~HEFSaver()
    {
      m_wrapperMapper.geoNodeMap.clear();
      m_wrapperMapper.animatedGeoNodeMap.clear();
      m_wrapperMapper.transformNodeMap.clear();
      m_wrapperMapper.animatedTransformNodeMap.clear();
      m_wrapperMapper.billboardNodeMap.clear();
      m_wrapperMapper.lodNodeMap.clear();
      m_wrapperMapper.lightNodeMap.clear();
      m_wrapperMapper.particleNodeMap.clear();

      m_wrapperMapper.geoNodes.clear();
      m_wrapperMapper.animatedGeoNodes.clear();
      m_wrapperMapper.transformNodes.clear();
      m_wrapperMapper.animatedTransformNodes.clear();
      m_wrapperMapper.billboardNodes.clear();
      m_wrapperMapper.lodNodes.clear();
      m_wrapperMapper.lightNodes.clear();
      m_wrapperMapper.particleNodes.clear();

      m_wrapperMapper.meshMap.clear();
      m_wrapperMapper.materialMap.clear();
      m_wrapperMapper.billboardTextureMap.clear();

      m_wrapperMapper.meshes.clear();
      m_wrapperMapper.materialFileNames.clear();
      m_wrapperMapper.billboardTextureFileNames.clear();
    }

    void HEFSaver::save(std::string path, std::string filename, sg::Scene* scene, util::SingletonManager *singletonManager)
    {
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

      NodeNumbers nodeNumbers;

      nodeNumbers.geoNodeSize = m_wrapperMapper.geoNodes.size();
      nodeNumbers.animatedGeoNodeSize = m_wrapperMapper.animatedGeoNodes.size();
      nodeNumbers.transformNodeSize = m_wrapperMapper.transformNodes.size();
      nodeNumbers.animatedTransformNodeSize = m_wrapperMapper.animatedTransformNodes.size();
      nodeNumbers.billboardNodeSize = m_wrapperMapper.billboardNodes.size();
      nodeNumbers.lodNodeSize = m_wrapperMapper.lodNodes.size();
      nodeNumbers.lightNodeSize = m_wrapperMapper.lightNodes.size();
      nodeNumbers.particleNodeSize = m_wrapperMapper.particleNodes.size();

      fileStream.write((char*)&nodeNumbers, sizeof(nodeNumbers));

      fileStream << std::endl;

      for(unsigned int i = 0; i < m_wrapperMapper.geoNodes.size(); i++)
      {
        fileStream << m_wrapperMapper.geoNodes[i];
      }

      for(unsigned int i = 0; i < m_wrapperMapper.animatedGeoNodes.size(); i++)
      {
        fileStream << m_wrapperMapper.animatedGeoNodes[i];
      }

      for(unsigned int i = 0; i < m_wrapperMapper.transformNodes.size(); i++)
      {
        fileStream << m_wrapperMapper.transformNodes[i];
      }

      for(unsigned int i = 0; i < m_wrapperMapper.animatedTransformNodes.size(); i++)
      {
        fileStream << m_wrapperMapper.animatedTransformNodes[i];
      }

      for(unsigned int i = 0; i < m_wrapperMapper.billboardNodes.size(); i++)
      {
        fileStream << m_wrapperMapper.billboardNodes[i];
      }

      for(unsigned int i = 0; i < m_wrapperMapper.lodNodes.size(); i++)
      {
        fileStream << m_wrapperMapper.lodNodes[i];
      }

      for(unsigned int i = 0; i < m_wrapperMapper.lightNodes.size(); i++)
      {
        fileStream << m_wrapperMapper.lightNodes[i];
      }

      for(unsigned int i = 0; i < m_wrapperMapper.particleNodes.size(); i++)
      {
        fileStream << m_wrapperMapper.particleNodes[i] << std::endl;
      }

      fileStream << m_wrapperMapper.meshes.size() << std::endl;
      for(unsigned int i = 0; i < m_wrapperMapper.meshes.size(); i++)
      {
        db::Mesh& mesh = m_wrapperMapper.meshes[i];

        MeshMetaData meshData;
        meshData.bbMax = mesh.getBBMax();
        meshData.bbMin = mesh.getBBMin();
        meshData.primitiveType = mesh.getPrimitiveType();
        meshData.primitiveCount = mesh.getPrimitiveCount();
        meshData.indexCount = mesh.getIndexCount();
        meshData.vertexCount = mesh.getVertexCount();
        meshData.vertexDeclaration = mesh.getVertexDeclarationFlags();
        meshData.vertexStride = mesh.getVertexStride();
        meshData.vboSize = mesh.getVBOSize();
        
        fileStream.write((char*)&meshData, sizeof(meshData));
        fileStream.write((char*)&mesh.getIndexBuffer()[0], sizeof(mesh.getIndexBuffer()[0]) * mesh.getIndexCount());
        fileStream.write((char*)&mesh.getVBOBuffer()[0], sizeof(mesh.getVBOBuffer()[0]) * mesh.getVBOSize());
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