//#include "Loader/NodeStreaming.h"
//
//#include <Utilities/Math/Math.hpp>
//#include <Utilities/Signals/EventManager.h>
//
//#include <DataBase/ResourceManager.hpp>
//#include <DataBase/Light.h>
//
//#include <SceneGraph/Scene/TreeNodeAllocator.h>
//#include <SceneGraph/Scene/TreeNodeFactory.h>
//
//#include <SceneGraph/TreeNodes/TreeNode.h>
//#include <SceneGraph/TreeNodes/GroupNode.h>
//#include <SceneGraph/TreeNodes/GeoNode.h>
//#include <SceneGraph/TreeNodes/AnimatedGeoNode.h>
//#include <SceneGraph/TreeNodes/TransformNode.h>
//#include <SceneGraph/TreeNodes/AnimatedTransformNode.h>
//#include <SceneGraph/TreeNodes/LODNode.h>
//#include <SceneGraph/TreeNodes/BillboardNode.h>
//#include <SceneGraph/TreeNodes/LightNode.h>
//#include <SceneGraph/TreeNodes/ShadowLightNode.h>
//#include <SceneGraph/TreeNodes/ParticleNode.h>
//
//namespace he
//{
//  namespace loader
//  {
//    void read(std::istream& stream, sg::TreeNode& node, util::EventManager *eventManager, std::map<std::string, std::map<unsigned int, util::ResourceHandle>> resourceHandles)
//    {
//      std::string name;
//      std::getline(stream, name);
//      std::getline(stream, name);//getline because of white spaces
//      node.setNodeName(name);
//
//      unsigned int nodeTypes;
//      unsigned int nodeIndex;
//
//      stream >> nodeTypes;
//      stream >> nodeIndex;
//      node.setParent(sg::NodeIndex(nodeIndex, (sg::NodeType)nodeTypes));
//
//      stream >> nodeTypes;
//      stream >> nodeIndex;
//      if(nodeIndex != ~0)
//      {
//        ((sg::GroupNode&)node).setFirstChild(sg::NodeIndex(nodeIndex, (sg::NodeType)nodeTypes));
//      }
//
//      stream >> nodeTypes;
//      stream >> nodeIndex;
//      node.setNextSibling(sg::NodeIndex(nodeIndex, (sg::NodeType)nodeTypes));
//    }
//
//    void read(std::istream& stream, sg::GeoNode& node, util::EventManager *eventManager, std::map<std::string, std::map<unsigned int, util::ResourceHandle>> resourceHandles)
//    {
//      read(stream, (sg::TreeNode&)node, eventManager, resourceHandles);
//      unsigned int hash;
//
//      stream >> hash;
//      node.setMeshHandle(resourceHandles["Mesh"][hash]);
//
//      stream >> hash;
//      node.setMaterialHandle(resourceHandles["Material"][hash]);
//
//      node.setEventManager(eventManager);
//    }
//
//    void read(std::istream& stream, sg::AnimatedGeoNode& node, util::EventManager *eventManager, std::map<std::string, std::map<unsigned int, util::ResourceHandle>> resourceHandles)
//    {
//      read(stream, (sg::GeoNode&)node, eventManager, resourceHandles);
//
//      unsigned int inverseBindPoseMatricesSize;
//      stream >> inverseBindPoseMatricesSize;
//      
//      std::vector<util::Matrix<float, 4>> matrices(inverseBindPoseMatricesSize);
//      if(matrices.size())
//      {
//        std::getline(stream, std::string());
//        stream.read((char*)&matrices[0][0][0], sizeof(matrices[0]) * matrices.size());
//      }
//
//      node.setInverseBindPoseMatrices(matrices);
//    }
//
//    void read(std::istream& stream, sg::TransformNode& node, util::EventManager *eventManager, std::map<std::string, std::map<unsigned int, util::ResourceHandle>> resourceHandles)
//    {
//      read(stream, (sg::TreeNode&)node, eventManager, resourceHandles);
//
//      util::Quaternion<float> rotation;
//      stream >> rotation;
//
//      util::Vector<float, 3> translation;
//      stream >> translation;
//
//      float scale;
//      stream >> scale;
//
//      node.setLocalRotation(rotation);
//      node.setLocalTranslation(translation);
//      node.setLocalScale(scale);
//    }
//
//    void read(std::istream& stream, sg::AnimatedTransformNode& node, util::EventManager *eventManager, std::map<std::string, std::map<unsigned int, util::ResourceHandle>> resourceHandles)
//    {
//      read(stream, (sg::TreeNode&)node, eventManager, resourceHandles);
//
//      unsigned int animationTracksSize;
//      stream >> animationTracksSize;
//      std::vector<sg::AnimationTrack> animationTracks(animationTracksSize);
//
//      for(unsigned int i = 0; i < animationTracks.size(); i++)
//      {
//        stream >> animationTracks[i];
//      }
//
//      node.setAnimationTracks(animationTracks);
//
//      unsigned int animatedMeshIndex;
//      stream >> animatedMeshIndex;
//      node.setSkinnedMesh(sg::NodeIndex(animatedMeshIndex, sg::ANIMATEDGEONODE));
//
//      unsigned int boneIndex;
//      stream >> boneIndex;
//      node.setBoneIndex(boneIndex);
//    }
//
//    void read(std::istream& stream, sg::LODNode& node, util::EventManager *eventManager, std::map<std::string, std::map<unsigned int, util::ResourceHandle>> resourceHandles)
//    {
//      read(stream, (sg::TreeNode&)node, eventManager, resourceHandles);
//
//      util::Vector<float, 3> position;
//      stream >> position;
//      node.setPosition(position);
//
//      unsigned int lodLevel;
//      stream >> lodLevel;
//      node.setLODLevel(lodLevel);
//    }
//
//    void read(std::istream& stream, sg::BillboardNode& node, util::EventManager *eventManager, std::map<std::string, std::map<unsigned int, util::ResourceHandle>> resourceHandles)
//    {
//      read(stream, (sg::TreeNode&)node, eventManager, resourceHandles);
//
//      util::Vector<unsigned int, 2> anim;
//
//      stream >> anim;
//      node.setCurrentAnimationFrame(anim);
//      stream >> anim;
//      node.setAnimationNumber(anim);
//
//      util::Vector<float, 2> tex;
//      stream >> tex;
//      node.setTextureStart(tex);
//
//      stream >> tex;
//      node.setTextureEnd(tex);
//
//      stream >> tex;
//      node.setScale(tex);
//
//      util::Vector<float, 3> translate;
//      stream >> translate;
//      node.setTranslation(translate);
//
//      unsigned int hash;
//      stream >> hash;
//      node.setTextureHandle(resourceHandles["Texture"][hash]);
//    }
//
//    void read(std::istream& stream, sg::LightNode& node, util::EventManager *eventManager, std::map<std::string, std::map<unsigned int, util::ResourceHandle>> resourceHandles)
//    {
//      read(stream, (sg::TreeNode&)node, eventManager, resourceHandles);
//
//      util::Vector<float, 3> color;
//      stream >> color;
//
//      float data;
//      stream >> data;
//      node.setIntensity(data);
//
//      stream >> data;
//      node.setSpotLightExponent(data);
//
//      stream >> data;
//      node.setSpotLightCutoff(data);
//
//      stream >> data;
//      node.setConstAttenuation(data);
//
//      stream >> data;
//      node.setLinearAttenuation(data);
//
//      stream >> data;
//      node.setQuadricAttenuation(data);
//
//      unsigned int lightType;
//      stream >> lightType;
//      node.setLightType((LightType)lightType);
//    }
//
//    void read(std::istream& stream, sg::ShadowLightNode& node, util::EventManager *eventManager, std::map<std::string, std::map<unsigned int, util::ResourceHandle>> resourceHandles)
//    {
//      read(stream, (sg::LightNode&)node, eventManager, resourceHandles);
//
//      float near;
//      stream >> near;
//
//      float far;
//      stream >> far;
//      node.setShadowProjection(near, far);
//    }
//
//    void read(std::istream& stream, sg::ParticleNode& node, util::EventManager *eventManager, std::map<std::string, std::map<unsigned int, util::ResourceHandle>> resourceHandles)
//    {
//      read(stream, (sg::TreeNode&)node, eventManager, resourceHandles);
//
//      util::Matrix<float, 4> trfMatrix;
//      stream >> trfMatrix;
//      node.setTransformationMatrix(trfMatrix);
//    }
//
//    void read(std::istream& stream, sg::TreeNodeAllocator& allocator, std::map<std::string, std::map<unsigned int, util::ResourceHandle>> resourceHandles, util::EventManager *eventManager)
//    {
//      unsigned int nodeTypeNumber;
//      stream >> nodeTypeNumber;
//
//      unsigned int nodeBlockSize;
//      stream >> nodeBlockSize;
//
//      for(unsigned int i = 0; i < nodeTypeNumber; i++)
//      {
//        unsigned int nodeBlockNumber;
//        stream >> nodeBlockNumber;
//
//        for(unsigned int j = 0; j < nodeBlockNumber; j++)
//        {
//          for(unsigned int k = 0; k < nodeBlockSize; k++)
//          {
//            unsigned int index;
//            unsigned int type;
//            stream >> type;
//            stream >> index;
//            sg::NodeIndex nodeIndex = sg::NodeIndex(index, (sg::NodeType)type);
//            sg::TreeNode& node = allocator.insert(nodeIndex);
//            node.read(stream, eventManager, resourceHandles);
//          }
//        }
//      }
//    }
//  }
//}