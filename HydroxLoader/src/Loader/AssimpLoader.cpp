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
    AssimpLoader::AssimpLoader(util::SingletonManager *singletonManager) : 
      m_singletonManager(singletonManager), 
      m_animationTimeUnit(Seconds)
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

    void AssimpLoader::setCacheGeneratorParamter(float errorRate, float maxDistance, float maxAngle)
    {
      m_errorRate = errorRate;
      m_maxDistance = maxDistance;
      m_maxAngle = maxAngle;
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
      std::cout << "Assimp Loads Scene" << std::endl;
      sg::Scene *scene = nullptr;
      Assimp::Importer importer;

      unsigned int deleteAssimpOptions = ~(aiProcess_GenUVCoords | aiProcess_CalcTangentSpace);
      const aiScene *assimpScene = importer.ReadFile(path + filename, aiProcessPreset_TargetRealtime_MaxQuality & deleteAssimpOptions);

      if(!assimpScene)
      {
        std::cerr << importer.GetErrorString() << std::endl;

        scene = loadDefaultSceneGraph();
      }
      else
      {
        m_fileInformation.filename = filename;
        m_fileInformation.meshNumber = assimpScene->mNumMeshes;
        m_fileInformation.materialNumber = assimpScene->mNumMaterials;
        m_fileInformation.animationNumber = assimpScene->mNumAnimations;
        m_fileInformation.cacheNumber = 0;
        m_fileInformation.faceNumber = 0;
        m_fileInformation.vertexNumber = 0;

        std::cout << "Mesh Conversion" << std::endl;
        loadMeshesFromAssimp(assimpScene, yAxisFlipped);

        std::cout << "Materials Conversion" << std::endl;
        loadMaterialsFromAssimp(path, assimpScene);

        std::cout << "Skeleton Conversion" << std::endl;
        loadAnimatedSkeleton(assimpScene);

        std::cout << "SceneGraph Conversion" << std::endl;
        sg::NodeIndex rootNode = loadSceneGraphFromAssimp(filename, assimpScene);

        attachBonesToSkinnedMesh();

        scene = new sg::Scene(m_allocator, rootNode);

        std::cout << "Finished!" << std::endl;

        m_animationTracks.clear();
        m_inverseBindPoseTable.clear();
        m_boneNameTable.clear();
        m_skinnedMeshTable.clear();
        m_boneTable.clear();
      }

      importer.FreeScene();

      return scene;
    }

    void AssimpLoader::loadMeshesFromAssimp(const aiScene *scene, bool yAxisFlipped)
    {
      m_meshes.resize(scene->mNumMeshes);
      m_materialIndex.resize(scene->mNumMeshes);
      m_inverseBindPoseTable.resize(scene->mNumMeshes);
      m_boneNameTable.resize(scene->mNumMeshes);

      util::ResourceHandle materialHandle;

      std::cout << "Total Mesh Number: " << scene->mNumMeshes << std::endl;

      for(unsigned int i = 0; i != scene->mNumMeshes; i++)
      {
        m_meshes[i] = loadVertices(scene->mMeshes[i], i, yAxisFlipped);

        m_fileInformation.cacheNumber += m_modelManager->getObject(m_meshes[i])->getCacheCount();
        m_fileInformation.faceNumber += m_modelManager->getObject(m_meshes[i])->getPrimitiveCount();
        m_fileInformation.vertexNumber += m_modelManager->getObject(m_meshes[i])->getVertexCount();

        m_materialIndex[i] = scene->mMeshes[i]->mMaterialIndex;

        std::cout << "(" << i + 1 << "/" << scene->mNumMeshes << ")" << std::endl;
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

      std::vector<VertexElements> vertexElements;

      if(primitiveType == GL_TRIANGLES)
      {
        vertexElements.push_back(db::Mesh::MODEL_CACHEINDICES0);
        vertexElements.push_back(db::Mesh::MODEL_CACHEINDICES1);
      }

      if(mesh->HasPositions()) vertexElements.push_back(db::Mesh::MODEL_POSITION);
      for(unsigned int j = 0; j < mesh->GetNumUVChannels(); j++)
      {
        if(mesh->HasTextureCoords(j) && mesh->mNumUVComponents[j] != 0)
        {
          vertexElements.push_back(VertexElements(db::Mesh::MODEL_TEXTURE0 + j));
        }
      }

      vertexElements.push_back(db::Mesh::MODEL_NORMAL);//normals have to be there, if not they have to be generated
      if(mesh->HasTangentsAndBitangents()) vertexElements.push_back(db::Mesh::MODEL_BINORMAL);
      if(mesh->HasBones())
      {
        vertexElements.push_back(db::Mesh::MODEL_BONE_WEIGHTS);
        vertexElements.push_back(db::Mesh::MODEL_BONE_INDICES);
      }

      if(mesh->HasVertexColors(0)) vertexElements.push_back(db::Mesh::MODEL_COLOR);

      if(mesh->HasFaces())
      {
        indices.resize(mesh->mNumFaces * indicesPerFace);

        for(unsigned int j = 0; j < mesh->mNumFaces; j++)
        {
          if(mesh->mFaces[j].mNumIndices != indicesPerFace)
          {
            std::cerr << "Invalid number of indices per face!" << std::endl;
            std::cerr << "Expected number: " << indicesPerFace << std::endl;
            std::cerr << "Actual number: " << mesh->mFaces[j].mNumIndices << std::endl;

            if(mesh->mFaces[j].mNumIndices > 3)
            {
              std::cerr << "No quads or polygons allowed!" << std::endl;
            }
          }

          for(unsigned int k = 0; k < indicesPerFace; k++)
          {
            indices[j * indicesPerFace + k] = static_cast<db::Mesh::indexType>(mesh->mFaces[j].mIndices[k]);
          }
        }
      }

      db::Mesh newMesh(GL_TRIANGLES, mesh->mNumVertices, vertexElements, indices);

      if(mesh->HasPositions())
      {
        positions.resize(mesh->mNumVertices);

        for(unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
          positions[i] = util::vec3f(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        }

        newMesh.copyDataIntoGeometryBuffer(db::Mesh::MODEL_POSITION, 0, positions.size(), reinterpret_cast<const GLubyte*>(&positions[0]));

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

          newMesh.copyDataIntoGeometryBuffer(db::Mesh::MODEL_TEXTURE0 + j, 0, textureCoords[j].size(), reinterpret_cast<const GLubyte*>(&textureCoords[j][0]));
        }
      }

      if(mesh->HasNormals())
      {
        normals.resize(mesh->mNumVertices);
        for(unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
          normals[i] = util::vec3f(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        }

        newMesh.copyDataIntoGeometryBuffer(db::Mesh::MODEL_NORMAL, 0, normals.size(), reinterpret_cast<const GLubyte*>(&normals[0]));
      }
      else
      {
        newMesh.generateNormals();
      }

      if(mesh->HasTangentsAndBitangents())
      {
        binormals.resize(mesh->mNumVertices);
        for(unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
          binormals[i] = util::vec3f(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
        }

        newMesh.copyDataIntoGeometryBuffer(db::Mesh::MODEL_BINORMAL, 0, binormals.size(), reinterpret_cast<const GLubyte*>(&binormals[0]));
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

        newMesh.copyDataIntoGeometryBuffer(db::Mesh::MODEL_BONE_WEIGHTS, 0, boneWeights.size(), reinterpret_cast<const GLubyte*>(&boneWeights[0]));
        newMesh.copyDataIntoGeometryBuffer(db::Mesh::MODEL_BONE_INDICES, 0, boneIndices.size(), reinterpret_cast<const GLubyte*>(&boneIndices[0]));
      }

      if(mesh->HasVertexColors(0))
      {
        vertexColors.resize(mesh->mNumVertices);

        for(unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
          vertexColors[i] = util::vec4f(mesh->mColors[0][i].r, mesh->mColors[0][i].g, mesh->mColors[0][i].b, mesh->mColors[0][i].a);
        }

        newMesh.copyDataIntoGeometryBuffer(db::Mesh::MODEL_COLOR, 0, vertexColors.size(), reinterpret_cast<const GLubyte*>(&vertexColors[0]));
      }

      newMesh.generateBoundingVolume();
      newMesh.generateCaches(m_errorRate, m_maxDistance, m_maxAngle);

      return m_modelManager->addObject(newMesh);
    }

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
        geoNodeIndex = m_allocator.insert(sg::AnimatedGeoNode(m_inverseBindPoseTable[meshIndex], m_eventManager, m_meshes[meshIndex], m_materials[m_materialIndex[meshIndex]], meshName, parentNode, nextSibling));
        m_skinnedMeshTable[geoNodeIndex] = m_boneNameTable[meshIndex];
      }
      else
      {
        geoNodeIndex = m_allocator.insert(sg::GeoNode(m_eventManager, m_meshes[meshIndex], m_materials[m_materialIndex[meshIndex]], meshName, parentNode, nextSibling));
      }

      return geoNodeIndex;
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

    sg::Scene* AssimpLoader::loadDefaultSceneGraph()
    {
      sg::NodeIndex sceneRootNode = m_allocator.insert(sg::TransformNode(util::Matrix<float, 4>::identity(), std::string("defaultCube")));

      std::vector<util::vec3f> positions;
      std::vector<util::vec3f> normals;
      std::vector<db::Mesh::indexType> indices;
      util::CubeGenerator::generateCube(positions, indices, normals);

      std::vector<VertexElements> vertexElements;
      vertexElements.push_back(db::Mesh::MODEL_POSITION);
      vertexElements.push_back(db::Mesh::MODEL_NORMAL);

      db::Mesh mesh(GL_TRIANGLES, positions.size(), vertexElements, indices);
      mesh.copyDataIntoGeometryBuffer(db::Mesh::MODEL_POSITION, 0, positions.size(), reinterpret_cast<const GLubyte*>(&positions[0]));
      mesh.copyDataIntoGeometryBuffer(db::Mesh::MODEL_NORMAL, 0, normals.size(), reinterpret_cast<const GLubyte*>(&normals[0]));
      mesh.generateCaches(m_errorRate, m_maxDistance, m_maxAngle);

      sg::NodeIndex geoNodeIndex = m_allocator.insert(sg::GeoNode(m_eventManager, m_modelManager->addObject(mesh), m_defaultMaterial, std::string("defaultCubeMesh"), sceneRootNode));
      ((sg::GroupNode&)m_allocator[sceneRootNode]).setFirstChild(geoNodeIndex);

      return new sg::Scene(m_allocator, sceneRootNode);
    }

    void AssimpLoader::loadMaterialsFromAssimp(std::string path, const aiScene *scene)
    {
      m_materials.resize(scene->mNumMaterials);
      std::vector<std::vector<util::ResourceHandle>> textures(db::Material::TEXTURETYPENUM);
      ILDevilLoader texLoader(m_singletonManager);
      aiString texPath;

      for(unsigned int i = 0; i < m_materials.size(); i++)
      {
        std::vector<uint64_t> hashes;

        textures[db::Material::DIFFUSETEX].resize(scene->mMaterials[i]->GetTextureCount(aiTextureType_DIFFUSE));
        textures[db::Material::NORMALTEX].resize(scene->mMaterials[i]->GetTextureCount(aiTextureType_NORMALS));
        textures[db::Material::DISPLACEMENTTEX].resize(scene->mMaterials[i]->GetTextureCount(aiTextureType_DISPLACEMENT));
        textures[db::Material::SPECULARTEX].resize(scene->mMaterials[i]->GetTextureCount(aiTextureType_SPECULAR));

        for(unsigned int k = 0; k != textures[db::Material::DIFFUSETEX].size(); k++)
        {
          scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, k, &texPath);
          textures[db::Material::DIFFUSETEX][k] = texLoader.loadResource(path + std::string(texPath.data));
          db::Texture2D *texture = m_singletonManager->getService<db::TextureManager>()->getObject(textures[db::Material::DIFFUSETEX][k]);
          hashes.push_back(texture->getHash());
        }

        for(unsigned int k = 0; k != textures[db::Material::NORMALTEX].size(); k++)
        {
          scene->mMaterials[i]->GetTexture(aiTextureType_NORMALS, 0, &texPath);
          textures[db::Material::NORMALTEX][k] = texLoader.loadResource(path + std::string(texPath.data));
          db::Texture2D *texture = m_singletonManager->getService<db::TextureManager>()->getObject(textures[db::Material::NORMALTEX][k]);
          hashes.push_back(texture->getHash());
        }

        for(unsigned int k = 0; k != textures[db::Material::DISPLACEMENTTEX].size(); k++)
        {
          scene->mMaterials[i]->GetTexture(aiTextureType_DISPLACEMENT, 0, &texPath);
          textures[db::Material::DISPLACEMENTTEX][k] = texLoader.loadResource(path + std::string(texPath.data));
          db::Texture2D *texture = m_singletonManager->getService<db::TextureManager>()->getObject(textures[db::Material::DISPLACEMENTTEX][k]);
          hashes.push_back(texture->getHash());
        }

        for(unsigned int k = 0; k != textures[db::Material::SPECULARTEX].size(); k++)
        {
          scene->mMaterials[i]->GetTexture(aiTextureType_SPECULAR, 0, &texPath);
          textures[db::Material::SPECULARTEX][k] = texLoader.loadResource(path + std::string(texPath.data));
          db::Texture2D *texture = m_singletonManager->getService<db::TextureManager>()->getObject(textures[db::Material::SPECULARTEX][k]);
          hashes.push_back(texture->getHash());
        }

        db::Material::MaterialData matData;
        matData.diffuseStrength = 0.5f;
        matData.specularStrength = 0.5f;
        matData.ambientStrength = 0.1f;
        matData.specularExponent = 1.0f;

        m_materials[i] = m_materialManager->addObject(db::Material(matData, textures, hashes, false));
      }
    }

    void AssimpLoader::printStatusInformations()
    {
      std::cout << std::endl;
      std::cout << "File Informations:" << std::endl;
      std::cout << "Filename: " << m_fileInformation.filename << std::endl;
      std::cout << "Mesh Number: " << m_fileInformation.meshNumber << std::endl;
      std::cout << "Material Number: " << m_fileInformation.materialNumber << std::endl;
      std::cout << "Animation Number: " << m_fileInformation.animationNumber << std::endl;
      std::cout << "Face Number: " << m_fileInformation.faceNumber << std::endl;
      std::cout << "Vertex Number: " << m_fileInformation.vertexNumber << std::endl;
      std::cout << "Cache Number: " << m_fileInformation.cacheNumber << std::endl;
      std::cout << std::endl;
    }
  }
}