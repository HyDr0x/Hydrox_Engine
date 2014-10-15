#include "Loader/AssimpLoader.h"

#include <sstream>

#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/PrimitiveGenerator/CubeGenerator.h>

#include <SceneGraph/TreeNodes/GroupNode.h>
#include <SceneGraph/TreeNodes/AnimatedGeoNode.h>
#include <SceneGraph/TreeNodes/GeoNode.h>
#include <SceneGraph/TreeNodes/LODNode.h>
#include <SceneGraph/TreeNodes/TransformNode.h>
#include <SceneGraph/Scene/Scene.h>

#include <DataBase/Texture2D.h>
#include <DataBase/Material.h>
#include <DataBase/Mesh.h>

#include "Loader/ILDevilLoader.h"
#include "Loader/RenderShaderLoader.h"
#include "Loader/MaterialLoader.h"

namespace he
{
  namespace loader
  {
    AssimpLoader::AssimpLoader(util::SingletonManager *singletonManager) : m_singletonManager(singletonManager), m_animationTimeUnit(Seconds)
    {
      m_eventManager = singletonManager->getService<util::EventManager>();
      m_modelManager = singletonManager->getService<db::ModelManager>();
      m_materialManager = singletonManager->getService<db::MaterialManager>();
      m_textureManager = singletonManager->getService<db::TextureManager>();
      m_renderShaderManager = singletonManager->getService<db::RenderShaderManager>();

      MaterialLoader materialLoader(m_singletonManager);
      m_defaultMaterial = materialLoader.getDefaultResource();
      setAnimationTimeUnit(m_animationTimeUnit);
    }

    AssimpLoader::AssimpLoader(const AssimpLoader& other)
    {
      m_modelManager = other.m_modelManager;
      m_materialManager = other.m_materialManager;
      m_textureManager = other.m_textureManager;
      m_renderShaderManager = other.m_renderShaderManager;
      m_defaultMaterial = other.m_defaultMaterial;
    }

    AssimpLoader::~AssimpLoader()
    {
    }

    AssimpLoader& AssimpLoader::operator=(const AssimpLoader& other)
    {
      m_modelManager = other.m_modelManager;
      m_materialManager = other.m_materialManager;
      m_textureManager = other.m_textureManager;
      m_renderShaderManager = other.m_renderShaderManager;
      m_defaultMaterial = other.m_defaultMaterial;

      return *this;
    }

    void AssimpLoader::setAnimationTimeUnit(AnimationTimeUnit animationTimeUnit)
    {
      m_animationTimeUnit = animationTimeUnit;
      switch(m_animationTimeUnit)
      {
      case Microseconds:
        m_animationTimeUnitConvert = 0.000001f;
        break;
      case Milliseconds:
        m_animationTimeUnitConvert = 0.001f;
        break;
      case Minutes:
        m_animationTimeUnitConvert = 60.0f;
        break;
      case Seconds:
      default:
        m_animationTimeUnitConvert = 1.0f;
      }
    }

    AssimpLoader::AnimationTimeUnit AssimpLoader::getAnimationTimeUnit() const
    {
      return m_animationTimeUnit;
    }

    sg::Scene* AssimpLoader::load(std::string path, std::string filename, bool yAxisFlipped)
    {
      sg::Scene *scene = nullptr;
      Assimp::Importer importer;
      const aiScene *assimpScene = importer.ReadFile(path + filename, aiProcessPreset_TargetRealtime_MaxQuality ^ aiProcess_GenUVCoords);

      if(!assimpScene)
      {
        std::cerr << importer.GetErrorString() << std::endl;

        scene = loadDefaultSceneGraph();
      }
      else
      {  
        loadMeshesFromAssimp(assimpScene, yAxisFlipped);

        loadAnimatedSkeleton(assimpScene);

        sg::NodeIndex rootNode = loadSceneGraphFromAssimp(filename, assimpScene);

        attachBonesToSkinnedMesh();

        scene = new sg::Scene(m_allocator, rootNode);

        m_animationTracks.clear();
        m_inverseBindPoseTable.clear();
        m_boneNameTable.clear();
        m_skinnedMeshTable.clear();
        m_boneTable.clear();
      }

      importer.FreeScene();

      return scene;
    }

    sg::Scene* AssimpLoader::loadDefaultSceneGraph()
    {
      sg::NodeIndex sceneRootNode = m_allocator.insert(sg::TransformNode(util::Matrix<float, 4>::identity(), std::string("defaultCube")));

      std::vector<util::vec3f> positions;
      std::vector<db::Mesh::indexType> indices;
      util::CubeGenerator::generateCube(positions, indices);
    
      MaterialLoader materialLoader(m_singletonManager);

      sg::NodeIndex geoNodeIndex = m_allocator.insert(sg::GeoNode(m_eventManager, m_modelManager->addObject(db::Mesh(GL_TRIANGLES, positions, indices)), materialLoader.getDefaultResource(), std::string("defaultCubeMesh"), sceneRootNode));
      ((sg::GroupNode&)m_allocator[sceneRootNode]).setFirstChild(geoNodeIndex);

      return new sg::Scene(m_allocator, sceneRootNode);
    }

    void AssimpLoader::loadMeshesFromAssimp(const aiScene *scene, bool yAxisFlipped)
    {
      m_meshes.resize(scene->mNumMeshes);
      m_inverseBindPoseTable.resize(scene->mNumMeshes);
      m_boneNameTable.resize(scene->mNumMeshes);

      util::ResourceHandle materialHandle;

      for(unsigned int i = 0; i != scene->mNumMeshes; i++)
      {
        m_meshes[i] = loadVertices(scene->mMeshes[i], i, yAxisFlipped);
      }
    }

    util::ResourceHandle AssimpLoader::loadVertices(const aiMesh *mesh, unsigned int meshIndex, bool yAxisFlipped)
    {
      std::vector<util::vec3f> positions;   
      std::vector<std::vector<util::vec2f>> textureCoords(db::Material::TEXTURETYPENUM);
      std::vector<util::vec3f> normals;
      std::vector<util::vec3f> binormals;
      std::vector<util::vec4f> boneIndices;
      std::vector<util::vec4f> boneWeights;
      std::vector<util::vec4f> vertexColors;
      std::vector<db::Mesh::indexType> indices;

      GLuint primitiveType;
      unsigned int indicesPerFace;

      switch(mesh->mPrimitiveTypes)
      {
      case aiPrimitiveType_POINT:
        primitiveType = GL_POINTS;
        indicesPerFace = 1;
        break;
      case aiPrimitiveType_LINE:
        primitiveType = GL_LINES;
        indicesPerFace = 2;
        break;
      case aiPrimitiveType_TRIANGLE:
      default:
        primitiveType = GL_TRIANGLES;
        indicesPerFace = 3;
        break;
      }

      if(mesh->HasPositions())
      {
        positions.resize(mesh->mNumVertices);

        for(unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
          positions[i] = util::vec3f(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        }
      }

      for(unsigned int j = 0; j < mesh->GetNumUVChannels(); j++)
      {
        if(mesh->HasTextureCoords(j) && mesh->mNumUVComponents[j] != 0)
        {
          textureCoords[j].resize(mesh->mNumVertices);

          GLfloat tmpYCoordinates;
          for(unsigned int i = 0; i < mesh->mNumVertices; i++)
          {
            //assert(mesh->mTextureCoords[j][i][0] >= 0.0 && mesh->mTextureCoords[j][i][1] >= 0.0 && mesh->mTextureCoords[j][i][0] <= 1.0 && mesh->mTextureCoords[j][i][1] <= 1.0);

            if( yAxisFlipped )
            {
              tmpYCoordinates = -mesh->mTextureCoords[j][i][1];
            }
            else
            {
              tmpYCoordinates = mesh->mTextureCoords[j][i][1];
            }

            textureCoords[j][i] = util::vec2f(mesh->mTextureCoords[j][i][0], tmpYCoordinates);//if the model is from DX the y-axis must be flipped
          }
        }
      }

      if(mesh->HasNormals())
      {
        normals.resize(mesh->mNumVertices);
        for(unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
          normals[i] = util::vec3f(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        }
      }

      if(mesh->HasTangentsAndBitangents())
      {
        binormals.resize(mesh->mNumVertices);
        for(unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
          binormals[i] = util::vec3f(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
        }
      }

      if(mesh->HasBones())
      {
        boneWeights.resize(mesh->mNumVertices, util::vec4f::identity());
        boneIndices.resize(mesh->mNumVertices, util::vec4f(~0, ~0, ~0, ~0));

        util::Matrix<float, 4> tmpBoneMatrix;
        m_inverseBindPoseTable[meshIndex].resize(mesh->mNumBones);
        m_boneNameTable[meshIndex].resize(mesh->mNumBones);

        for(unsigned int i = 0; i < mesh->mNumBones; i++)
        {
          std::copy(&mesh->mBones[i]->mOffsetMatrix[0][0], &mesh->mBones[i]->mOffsetMatrix[0][0] + 16, &tmpBoneMatrix[0][0]);
          tmpBoneMatrix = tmpBoneMatrix.transpose();

          m_inverseBindPoseTable[meshIndex][i] = tmpBoneMatrix;
          m_boneNameTable[meshIndex][i] = std::string(mesh->mBones[i]->mName.C_Str());

          for(unsigned int j = 0; j < mesh->mBones[i]->mNumWeights; j++)
          {
            for(unsigned int k = 0; k < 4; k++)
            {
              unsigned int vertexID = mesh->mBones[i]->mWeights[j].mVertexId;
              if(boneIndices[vertexID][k] == ~0)
              {
                boneWeights[vertexID][k] = mesh->mBones[i]->mWeights[j].mWeight;
                boneIndices[vertexID][k] = static_cast<float>(i);
                break;
              }
            }
          }
        }
      }

      if(mesh->HasVertexColors(0))
      {
        vertexColors.resize(mesh->mNumVertices);

        for(unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
          vertexColors[i] = util::vec4f(mesh->mColors[0][i].r, mesh->mColors[0][i].g, mesh->mColors[0][i].b, mesh->mColors[0][i].a);
        }
      }

      if(mesh->HasFaces())
      {
        indices.resize(mesh->mNumFaces * indicesPerFace);

        for(unsigned int j = 0; j < mesh->mNumFaces; j++)
        {
          for(unsigned int k = 0; k < mesh->mFaces[j].mNumIndices; k++)
          {
            assert(mesh->mFaces[j].mNumIndices < 4 && "NO QUADS OR POLYGON PRIMITIVES ALLOWED!");
            indices[j * indicesPerFace + k] = static_cast<db::Mesh::indexType>(mesh->mFaces[j].mIndices[k]);
          }
        }
      }

      return m_modelManager->addObject(db::Mesh(
        primitiveType,
        positions,
        indices,
        textureCoords,
        normals,
        binormals,
        boneWeights,
        boneIndices,
        vertexColors));
    }

    sg::NodeIndex AssimpLoader::loadSceneGraphFromAssimp(std::string filename, const aiScene *scene)
    {
      sg::NodeIndex sceneRootNodeIndex = m_allocator.insert(sg::TransformNode(util::Matrix<float, 4>::identity(), filename));
      ((sg::GroupNode&)m_allocator[sceneRootNodeIndex]).setFirstChild(createTransformNodes(scene->mRootNode, sceneRootNodeIndex, ~0));

      return sceneRootNodeIndex;
    }

    sg::NodeIndex AssimpLoader::createTransformNodes(const aiNode *node, sg::NodeIndex parentNode, sg::NodeIndex nextSibling)
    {
      sg::NodeIndex transformNodeIndex = ~0;

      std::map<std::string, std::vector<sg::AnimationTrack>>::iterator mit = m_animationTracks.find(std::string(node->mName.C_Str()));

      if(mit != m_animationTracks.end())
      {
        transformNodeIndex = m_allocator.insert(sg::AnimatedTransformNode(mit->second, std::string(node->mName.C_Str()), parentNode, nextSibling, ~0));
        sg::AnimatedTransformNode& transformNode = (sg::AnimatedTransformNode&)m_allocator[transformNodeIndex];

        m_boneTable[transformNode.getNodeName()] = transformNodeIndex;
        transformNode.addDirtyFlag(sg::GroupNode::ANIM_DIRTY);//animations need to be updated
      }
      else
      {
        util::Matrix<float, 4> tmpTransformationMatrix;
        int size = sizeof(node->mTransformation) / sizeof(node->mTransformation[0][0]);
        std::copy(&node->mTransformation[0][0], &node->mTransformation[0][0] + size, &tmpTransformationMatrix[0][0]);
        tmpTransformationMatrix = tmpTransformationMatrix.transpose();

        transformNodeIndex = m_allocator.insert(sg::TransformNode(tmpTransformationMatrix, std::string(node->mName.C_Str()), parentNode, nextSibling, ~0));
        ((sg::GroupNode&)m_allocator[transformNodeIndex]).addDirtyFlag(sg::GroupNode::TRF_DIRTY);//transformations need to be updated
      }

      nextSibling = ~0;

      if(node->mNumMeshes)
      {
        for(unsigned int i = 0; i < node->mNumMeshes; i++)//all meshes are children of the current transformation node
        {
          std::stringstream stream;
          stream << i;

          nextSibling = createGeoNodes(std::string(node->mName.C_Str()) + std::string("_Mesh") + stream.str(), node->mMeshes[i], transformNodeIndex, nextSibling);

          stream.str("");
        }

        ((sg::GroupNode&)m_allocator[transformNodeIndex]).setFirstChild(nextSibling);//the last sibling index must be the first childindex
      }

      if(node->mNumChildren)
      {
        for(unsigned int i = 0; i < node->mNumChildren; i++)//all further child nodes of the current transformation node
        {
          nextSibling = createTransformNodes(node->mChildren[i], transformNodeIndex, nextSibling);
        }

        ((sg::GroupNode&)m_allocator[transformNodeIndex]).setFirstChild(nextSibling);//the last sibling index must be the first childindex
      }

      return transformNodeIndex;
    }

    sg::NodeIndex AssimpLoader::createGeoNodes(std::string meshName, unsigned int meshIndex, sg::NodeIndex parentNode, sg::NodeIndex nextSibling)
    {
      sg::NodeIndex geoNodeIndex = ~0;

      if(!m_inverseBindPoseTable[meshIndex].empty())
      {
        geoNodeIndex = m_allocator.insert(sg::AnimatedGeoNode(m_inverseBindPoseTable[meshIndex], m_eventManager, m_meshes[meshIndex], m_defaultMaterial, meshName, parentNode, nextSibling));
        m_skinnedMeshTable[geoNodeIndex] = m_boneNameTable[meshIndex];
      }
      else
      {
        geoNodeIndex = m_allocator.insert(sg::GeoNode(m_eventManager, m_meshes[meshIndex], m_defaultMaterial, meshName, parentNode, nextSibling));
      }

      return geoNodeIndex;
    }

    //sg::TreeNode* AssimpLoader::createSceneNodes(const aiScene *scene, const aiNode *node, const std::vector<util::ResourceHandle>& meshes, sg::GroupNode *parentNode, sg::TreeNode *nextSibling)
    //{
    //  sg::TransformNode *transformNode = nullptr;

    //  std::map<std::string, std::vector<sg::AnimationTrack>>::iterator mit = m_animationTracks.find(std::string(node->mName.C_Str()));

    //  if(mit != m_animationTracks.end())
    //  {
    //    transformNode = new sg::AnimatedTransformNode(mit->second, std::string(node->mName.C_Str()), parentNode, nextSibling, nullptr);
    //    m_boneTable[transformNode->getNodeName()] = dynamic_cast<sg::AnimatedTransformNode*>(transformNode);
    //    transformNode->addDirtyFlag(sg::GroupNode::ANIM_DIRTY);//animations need to be updated
    //  }
    //  else
    //  {
    //    util::Matrix<float, 4> tmpTransformationMatrix;
    //    std::copy(&node->mTransformation[0][0], &node->mTransformation[0][0] + 16, &tmpTransformationMatrix[0][0]);
    //    tmpTransformationMatrix = tmpTransformationMatrix.transpose();

    //    transformNode = new sg::TransformNode(tmpTransformationMatrix, std::string(node->mName.C_Str()), parentNode, nextSibling, nullptr);
    //    transformNode->addDirtyFlag(sg::GroupNode::TRF_DIRTY);//transformations need to be updated
    //  }

    //  parentNode = transformNode;
    //  nextSibling = nullptr;

    //  std::stringstream stream;
    //  sg::GeoNode *geoNode = nullptr;
    //  util::Matrix<float, 4> tmpBoneMatrix;

    //  for(unsigned int i = 0; i < node->mNumMeshes; i++)//all meshes are children of the current transformation node
    //  {
    //    unsigned int meshIndex = node->mMeshes[i];

    //    stream << i;
    //    if(!m_inverseBindPoseTable[meshIndex].empty())
    //    {
    //      geoNode = new sg::AnimatedGeoNode(m_inverseBindPoseTable[meshIndex], m_eventManager, meshes[meshIndex], m_defaultMaterial, std::string(node->mName.C_Str()) + std::string("_Mesh") + stream.str(), parentNode, nextSibling);
    //      m_skinnedMeshTable[dynamic_cast<sg::AnimatedGeoNode*>(geoNode)] = m_boneNameTable[meshIndex];
    //    }
    //    else
    //    {
    //      geoNode = new sg::GeoNode(m_eventManager, meshes[meshIndex], m_defaultMaterial, std::string(node->mName.C_Str()) + std::string("_Mesh") + stream.str(), parentNode, nextSibling);
    //    }
    //  
    //    stream.str("");
    //    nextSibling = geoNode;
    //  }

    //  for(unsigned int i = 0; i < node->mNumChildren; i++)//all further child nodes of the current transformation node
    //  {
    //    nextSibling = createSceneNodes(scene, node->mChildren[i], meshes, parentNode, nextSibling);
    //  }

    //  if(node->mNumMeshes != 0 || node->mNumChildren != 0)
    //  {
    //    parentNode->setFirstChild(nextSibling);//the last sibling index must be the first childindex
    //  }

    //  return transformNode;
    //}

    void AssimpLoader::loadAnimatedSkeleton(const aiScene *scene)
    {
      for(unsigned int i = 0; i < scene->mNumAnimations; i++)
      {
        std::string animationName = scene->mAnimations[i]->mName.C_Str();

        for(unsigned int j = 0; j < scene->mAnimations[i]->mNumChannels; j++)
        {
          aiNodeAnim *channel = scene->mAnimations[i]->mChannels[j];

          std::string nodeName = std::string(channel->mNodeName.C_Str());
          if(m_animationTracks[nodeName].empty())
          {
            m_animationTracks[nodeName].resize(scene->mNumAnimations);
          }

          sg::AnimationTrack& animationTrack = m_animationTracks[nodeName][i];

          animationTrack.m_positions.resize(channel->mNumPositionKeys);
          animationTrack.m_positionsTime.resize(channel->mNumPositionKeys);

          animationTrack.m_rotations.resize(channel->mNumRotationKeys);
          animationTrack.m_rotationsTime.resize(channel->mNumRotationKeys);

          animationTrack.m_scales.resize(channel->mNumScalingKeys);
          animationTrack.m_scalesTime.resize(channel->mNumScalingKeys);

          animationTrack.m_animationName = animationName;
          animationTrack.m_duration = scene->mAnimations[i]->mDuration;
          animationTrack.m_animationTicksPerSecond = scene->mAnimations[i]->mTicksPerSecond == 0.0 ? m_animationTimeUnitConvert : scene->mAnimations[i]->mTicksPerSecond * m_animationTimeUnitConvert;

          for(unsigned int k = 0; k < channel->mNumPositionKeys; k++)
          {
            aiVector3D position = channel->mPositionKeys[k].mValue;
            animationTrack.m_positions[k] = util::vec3f(position.x, position.y, position.z);
            animationTrack.m_positionsTime[k] = channel->mPositionKeys[k].mTime;
          }

          for(unsigned int k = 0; k < channel->mNumRotationKeys; k++)
          {
            aiQuaternion rotation = channel->mRotationKeys[k].mValue;
            animationTrack.m_rotations[k] = util::Quaternion<float>(rotation.w, rotation.x, rotation.y, rotation.z).invert();
            animationTrack.m_rotationsTime[k] = channel->mRotationKeys[k].mTime;
          }

          for(unsigned int k = 0; k < channel->mNumScalingKeys; k++)
          {
            aiVector3D scale = channel->mScalingKeys[k].mValue;
            assert(abs(scale.x - scale.y) < 0.001f && abs(scale.x - scale.z) < 0.001f && abs(scale.y - scale.z) < 0.001f);

            animationTrack.m_scales[k] = scale.x;
            animationTrack.m_scalesTime[k] = channel->mScalingKeys[k].mTime;
          }
        }
      }
    }

    void AssimpLoader::attachBonesToSkinnedMesh()
    {
      for(std::map<sg::NodeIndex, std::vector<std::string>, sg::NodeIndex::Less>::iterator mit = m_skinnedMeshTable.begin(); mit != m_skinnedMeshTable.end(); mit++)
      {
        sg::NodeIndex geoNode = mit->first;
        for(unsigned int i = 0; i < mit->second.size(); i++)
        {
          sg::AnimatedTransformNode& animNode = (sg::AnimatedTransformNode&)m_allocator[m_boneTable.find(mit->second[i])->second];
          animNode.setSkinnedMesh(geoNode);
          animNode.setBoneIndex(i);
        }
      }
    }
  }
}