#include "Saver/NodeStreaming.h"

#include <fstream>

#include <Utilities/Math/Math.hpp>
#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <DataBase/ResourceManager.hpp>
#include <DataBase/Light.h>

#include <SceneGraph/Scene/TreeNodeAllocator.h>

#include <SceneGraph/TreeNodes/TreeNode.h>
#include <SceneGraph/TreeNodes/GroupNode.h>
#include <SceneGraph/TreeNodes/GeoNode.h>
#include <SceneGraph/TreeNodes/AnimatedGeoNode.h>
#include <SceneGraph/TreeNodes/TransformNode.h>
#include <SceneGraph/TreeNodes/AnimatedTransformNode.h>
#include <SceneGraph/TreeNodes/LODNode.h>
#include <SceneGraph/TreeNodes/BillboardNode.h>
#include <SceneGraph/TreeNodes/LightNode.h>
#include <SceneGraph/TreeNodes/ShadowLightNode.h>
#include <SceneGraph/TreeNodes/ParticleNode.h>

namespace he
{
  namespace saver
  {
    void write(std::ostream& stream, const sg::TreeNode& node, util::SingletonManager *singletonManager)
    {
      stream << (unsigned int)node.getNodeIndex().nodeType << std::endl;
      stream << node.getNodeIndex().index << std::endl;

      stream << node.getNodeName().c_str() << std::endl;

      stream << (unsigned int)node.getParent().nodeType << std::endl;
      stream << node.getParent().index << std::endl;

      stream << (unsigned int)node.getFirstChild().nodeType << std::endl;
      stream << node.getFirstChild().index << std::endl;

      stream << (unsigned int)node.getNextSibling().nodeType << std::endl;
      stream << node.getNextSibling().index << std::endl;
    }

    void write(std::ostream& stream, const sg::GeoNode& node, util::SingletonManager *singletonManager)
    {
      write(stream, (const sg::TreeNode&)node, singletonManager);
      stream << singletonManager->getService<db::ModelManager>()->getObject(node.getMeshHandle())->getHash() << std::endl;
      stream << singletonManager->getService<db::MaterialManager>()->getObject(node.getMaterialHandle())->getHash() << std::endl;
    }

    void write(std::ostream& stream, const sg::AnimatedGeoNode& node, util::SingletonManager *singletonManager)
    {
      write(stream, (const sg::GeoNode&)node, singletonManager);

      stream << node.getInverseBindPoseMatrices().size() << std::endl;
      if(node.getInverseBindPoseMatrices().size())
      {
        stream.write((char*)&node.getInverseBindPoseMatrices()[0][0][0], sizeof(node.getInverseBindPoseMatrices()[0]) * node.getInverseBindPoseMatrices().size());
        stream << std::endl;
      }
    }

    void write(std::ostream& stream, const sg::TransformNode& node, util::SingletonManager *singletonManager)
    {
      write(stream, (const sg::TreeNode&)node, singletonManager);

      stream << node.getLocalRotation() << std::endl;
      stream << node.getLocalPosition() << std::endl;
      stream << node.getLocalScale() << std::endl;
    }

    void write(std::ostream& stream, const sg::AnimatedTransformNode& node, util::SingletonManager *singletonManager)
    {
      write(stream, (const sg::TransformNode&)node, singletonManager);

      stream << node.getAnimationTracks().size() << std::endl;
      for(unsigned int i = 0; i < node.getAnimationTracks().size(); i++)
      {
        stream << node.getAnimationTracks()[i];
      }
      stream << node.getSkinnedMesh().index << std::endl;
      stream << node.getBoneIndex() << std::endl;
    }

    void write(std::ostream& stream, const sg::LODNode& node, util::SingletonManager *singletonManager)
    {
      write(stream, (const sg::TreeNode&)node, singletonManager);

      stream << node.getPosition() << std::endl;
      stream << node.getLODLevel() << std::endl;
    }

    void write(std::ostream& stream, const sg::BillboardNode& node, util::SingletonManager *singletonManager)
    {
      write(stream, (const sg::TreeNode&)node, singletonManager);

      stream << node.getAnimationNumber() << std::endl;
      stream << node.getCurrentAnimationFrame() << std::endl;
      stream << node.getTextureStart() << std::endl;
      stream << node.getTextureEnd() << std::endl;
      stream << node.getScale() << std::endl;
      stream << node.getPosition() << std::endl;
      stream << singletonManager->getService<db::TextureManager>()->getObject(node.getTextureHandle())->getHash() << std::endl;
    }

    void write(std::ostream& stream, const sg::LightNode& node, util::SingletonManager *singletonManager)
    {
      write(stream, (const sg::TreeNode&)node, singletonManager);

      stream << node.getColor() << std::endl;

      stream << node.getIntensity() << std::endl;

      stream << node.getSpotLightExponent() << std::endl;
      stream << node.getSpotLightCutoff() << std::endl;

      stream << node.getConstAttenuation() << std::endl;
      stream << node.getLinearAttenuation() << std::endl;
      stream << node.getQuadricAttenuation() << std::endl;

      stream << (unsigned int)node.getLightType() << std::endl;
    }

    void write(std::ostream& stream, const sg::ShadowLightNode& node, util::SingletonManager *singletonManager)
    {
      write(stream, (const sg::LightNode&)node, singletonManager);

      stream << node.getShadowProjectionMatrix() << std::endl;
    }

    void write(std::ostream& stream, const sg::ParticleNode& node, util::SingletonManager *singletonManager)
    {
      write(stream, (const sg::TreeNode&)node, singletonManager);

      stream << node.getTransformationMatrix() << std::endl;
    }

    void write(std::ostream& stream, const sg::TreeNodeAllocator& allocator, util::SingletonManager *singletonManager)
    {
      stream << (unsigned int)sg::NODENUMBER << std::endl;

      unsigned int nodeBlockSize = allocator.getNodeBlockSize();
      stream << nodeBlockSize << std::endl;

      for(unsigned int k = 0; k < sg::NODENUMBER; k++)
      {
        unsigned int nodeBlockNumber = allocator.getNodeBlockNumber((sg::NodeType)k);
        stream << nodeBlockNumber << std::endl;
        for(unsigned int i = 0; i < nodeBlockNumber; i++)
        {
          for(unsigned int j = 0; j < nodeBlockSize; j++)
          {
            sg::NodeIndex index = sg::NodeIndex(i * nodeBlockSize + j, (sg::NodeType)k);
            if(allocator.contains(index))
            {
              write(stream, allocator[index], singletonManager);
            }
          }
        }
      }
    }
  }
}