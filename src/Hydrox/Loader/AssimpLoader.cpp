#include "Hydrox/Loader/AssimpLoader.h"

#include <sstream>

#include "Hydrox/Utility/Tree/GroupNode.h"
#include "Hydrox/Utility/Tree/AnimatedGeoNode.h"
#include "Hydrox/Utility/Tree/GeoNode.h"
#include "Hydrox/Utility/Tree/LODNode.h"
#include "Hydrox/Utility/Tree/TransformNode.h"

#include "Hydrox/Loader/ILDevilLoader.h"

#include "Hydrox/Graphics/Scene.h"
#include "Hydrox/Graphics/Texture.h"

namespace he
{
  AssimpLoader::AssimpLoader(ModelManager *modelManager, MaterialManager *materialManager, TextureManager *textureManager, ShaderManager *shaderManager) : m_modelManager(modelManager),
                                                                                                                                                           m_materialManager(materialManager),
                                                                                                                                                           m_textureManager(textureManager),
                                                                                                                                                           m_shaderManager(shaderManager),
                                                                                                                                                           m_animationTimeUnit(AnimationTimeUnit::Seconds)
  {
    setAnimationTimeUnit(m_animationTimeUnit);
  }

  AssimpLoader::AssimpLoader(const AssimpLoader& o)
  {
    m_modelPath = o.m_modelPath;

    m_modelManager = o.m_modelManager;
    m_materialManager = o.m_materialManager;
    m_textureManager = o.m_textureManager;
    m_shaderManager = o.m_shaderManager;
  }

  AssimpLoader& AssimpLoader::operator=(const AssimpLoader& o)
  {
    m_modelPath = o.m_modelPath;

    m_modelManager = o.m_modelManager;
    m_materialManager = o.m_materialManager;
    m_textureManager = o.m_textureManager;
    m_shaderManager = o.m_shaderManager;

    return *this;
  }

  AssimpLoader::~AssimpLoader()
  {
  }

  void AssimpLoader::setAnimationTimeUnit(AnimationTimeUnit animationTimeUnit)
  {
    m_animationTimeUnit = animationTimeUnit;
    switch(m_animationTimeUnit)
    {
    case AnimationTimeUnit::Microseconds:
      m_animationTimeUnitConvert = 0.000001f;
      break;
    case AnimationTimeUnit::Milliseconds:
      m_animationTimeUnitConvert = 0.001f;
      break;
    case AnimationTimeUnit::Minutes:
      m_animationTimeUnitConvert = 60.0f;
      break;
    case AnimationTimeUnit::Seconds:
    default:
      m_animationTimeUnitConvert = 1.0f;
    }
  }

  AssimpLoader::AnimationTimeUnit AssimpLoader::getAnimationTimeUnit()
  {
    return m_animationTimeUnit;
  }

  Scene* AssimpLoader::load(std::string filename, std::string materialFileName, bool yAxisFlipped)
  {
    Assimp::Importer importer;
	  const aiScene *assimpScene = importer.ReadFile(m_modelManager->getPath() + filename, aiProcessPreset_TargetRealtime_MaxQuality ^ aiProcess_GenUVCoords);

	  if(!assimpScene)
    {
		  printf("%s\n", importer.GetErrorString());
    }

	  assert(assimpScene);

    std::vector<ResourceHandle> meshes;//contains the mesh id's of the scene
    std::vector<ResourceHandle> materials;//contains the material id's of the scene
    
    loadMaterialsFromAssimp(materials, materialFileName, assimpScene);

    loadMeshesFromAssimp(meshes, materials, assimpScene, yAxisFlipped);

    loadAnimatedSkeleton(assimpScene);

    GroupNode *rootNode = loadSceneGraphFromAssimp(filename, assimpScene->mRootNode, meshes);

    attachBonesToSkinnedMesh();

    Scene *scene = new Scene(rootNode, Vector<float, 3>::identity());

	  importer.FreeScene();

    m_animationTracks.clear();
    m_inverseBindPoseTable.clear();
    m_boneNameTable.clear();
    m_skinnedMeshTable.clear();
    m_boneTable.clear();

	  return scene;
  }

  void AssimpLoader::loadMeshesFromAssimp(std::vector<ResourceHandle>& out_meshes, std::vector<ResourceHandle>& in_materials, const aiScene *scene, bool yAxisFlipped)
  {
    out_meshes.resize(scene->mNumMeshes);
    m_inverseBindPoseTable.resize(scene->mNumMeshes);
    m_boneNameTable.resize(scene->mNumMeshes);

    ResourceHandle materialHandle;

	  for(unsigned int i = 0; i != scene->mNumMeshes; i++)
	  {
      materialHandle = in_materials[scene->mMeshes[i]->mMaterialIndex];

      out_meshes[i] = loadVertices(scene->mMeshes[i], materialHandle, i, yAxisFlipped);
	  }
  }

  ResourceHandle AssimpLoader::loadVertices(const aiMesh *mesh, ResourceHandle materialIndex, unsigned int meshIndex, bool yAxisFlipped)
  {
    GLuint vertexDeclarationFlags;

    vertexDeclarationFlags |= mesh->HasPositions()              ? Mesh::MODEL_POSITION     : 0;
    vertexDeclarationFlags |= mesh->HasTextureCoords(0)         ? Mesh::MODEL_TEXTURE      : 0;
    vertexDeclarationFlags |= mesh->HasNormals()                ? Mesh::MODEL_NORMAL       : 0;
    vertexDeclarationFlags |= mesh->HasTangentsAndBitangents()  ? Mesh::MODEL_BINORMAL     : 0;
    vertexDeclarationFlags |= mesh->HasBones()                  ? Mesh::MODEL_BONE_WEIGHTS : 0;
    vertexDeclarationFlags |= mesh->HasBones()                  ? Mesh::MODEL_BONE_INDICES : 0;

    std::vector<Vector<float, 3>> positions;
    std::vector<Vector<float, 2>> textureCoords;
    std::vector<Vector<float, 3>> normals;
    std::vector<Vector<float, 3>> binormals;
    std::vector<Vector<float, 4>> boneIndices;
    std::vector<Vector<float, 4>> boneWeights;
    std::vector<Mesh::indexType> indices;

	  if(mesh->HasPositions())
	  {
      positions.resize(mesh->mNumVertices);

      for(unsigned int i = 0; i < mesh->mNumVertices; i++)
      {
        positions[i] = Vector<float, 3>(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
      }
	  }

	  if(mesh->HasTextureCoords(0) && mesh->mNumUVComponents[0]!=0)
	  {
		  assert(!mesh->mNumUVComponents[1] && !mesh->mNumUVComponents[2] && !mesh->mNumUVComponents[3]);

      textureCoords.resize(mesh->mNumVertices);

		  GLfloat tmpYCoordinates;
		  for(unsigned int i = 0; i < mesh->mNumVertices; i++)
		  {
        //assert(mesh->mTextureCoords[0][i][0] >= 0.0 && mesh->mTextureCoords[0][i][1] >= 0.0 && mesh->mTextureCoords[0][i][0] <= 1.0 && mesh->mTextureCoords[0][i][1] <= 1.0);

			  if( yAxisFlipped )
        {
          tmpYCoordinates = -mesh->mTextureCoords[0][i][1];
        }
			  else
        {
          tmpYCoordinates = mesh->mTextureCoords[0][i][1];
        }

        textureCoords[i] = Vector<float, 2>(mesh->mTextureCoords[0][i][0], tmpYCoordinates);//if the model is from DX the y-axis must be flipped
		  }
	  }

	  if(mesh->HasNormals())
	  {
      normals.resize(mesh->mNumVertices);
      for(unsigned int i = 0; i < mesh->mNumVertices; i++)
		  {
        normals[i] = Vector<float, 3>(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
      }
	  }

	  if(mesh->HasTangentsAndBitangents())
	  {
      binormals.resize(mesh->mNumVertices);
      for(unsigned int i = 0; i < mesh->mNumVertices; i++)
		  {
        binormals[i] = Vector<float, 3>(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
      }
	  }

	  if(mesh->HasBones())
	  {
      boneWeights.resize(mesh->mNumVertices, Vector<float, 4>::identity());
      boneIndices.resize(mesh->mNumVertices, Vector<float, 4>(~0, ~0, ~0, ~0));

      Matrix<float, 4> tmpBoneMatrix;
      m_inverseBindPoseTable[meshIndex].resize(mesh->mNumBones);
      m_boneNameTable[meshIndex].resize(mesh->mNumBones);

      for(unsigned int i = 0; i < mesh->mNumBones; i++)
		  {
        memcpy(&(tmpBoneMatrix[0][0]), &(mesh->mBones[i]->mOffsetMatrix[0][0]), sizeof(Matrix<float, 4>));
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
              boneIndices[vertexID][k] = i;
              break;
            }
          }
        }
      }
	  }

	  if(mesh->HasFaces())
	  {
		  indices.resize(mesh->mNumFaces * 3);

		  for(unsigned int j = 0; j < mesh->mNumFaces; j++)
		  {
        for(unsigned int k = 0; k < mesh->mFaces[j].mNumIndices; k++)
        {
          assert(mesh->mFaces[j].mNumIndices == 3);
          indices[j * 3 + k] = static_cast<Mesh::indexType>(mesh->mFaces[j].mIndices[k]);
        }
		  }
	  }

    return m_modelManager->addObject(Mesh(vertexDeclarationFlags, materialIndex, 
      positions,
      textureCoords,
      normals,
      binormals,
      boneIndices,
      boneWeights,
      indices));
  }

  void AssimpLoader::loadMaterialsFromAssimp(std::vector<ResourceHandle>& out_materials, std::string materialFileName, const aiScene *scene)
  {
    out_materials.resize(scene->mNumMaterials);
    std::vector< std::vector<ResourceHandle> > textures;
    ILDevilLoader texLoader(m_textureManager);
    aiString texPath;

    for(unsigned int j = 0; j < out_materials.size(); j++)
    {
      textures.resize(4);
      textures[0].resize(scene->mMaterials[j]->GetTextureCount(aiTextureType_DIFFUSE));
      textures[1].resize(scene->mMaterials[j]->GetTextureCount(aiTextureType_NORMALS));
      textures[2].resize(scene->mMaterials[j]->GetTextureCount(aiTextureType_DISPLACEMENT));
      textures[3].resize(scene->mMaterials[j]->GetTextureCount(aiTextureType_SPECULAR));
  
	    for(unsigned int k = 0; k != textures[0].size(); k++)
	    {
		    scene->mMaterials[j]->GetTexture(aiTextureType_DIFFUSE, k, &texPath);
		    textures[0][k] = texLoader.load(texPath.data, GL_TEXTURE_2D);
	    }

	    for(unsigned int k = 0; k != textures[1].size(); k++)
	    {
		    scene->mMaterials[j]->GetTexture(aiTextureType_NORMALS, 0, &texPath);
		    textures[1][k] = texLoader.load(texPath.data, GL_TEXTURE_2D);
	    }

	    for(unsigned int k = 0; k != textures[2].size(); k++)
	    {
		    scene->mMaterials[j]->GetTexture(aiTextureType_DISPLACEMENT, 0, &texPath);
		    textures[2][k] = texLoader.load(texPath.data, GL_TEXTURE_2D);
	    }

	    for(unsigned int k = 0;k != textures[3].size(); k++)
	    {
		    scene->mMaterials[j]->GetTexture(aiTextureType_SPECULAR, 0, &texPath);
		    textures[3][k] = texLoader.load(texPath.data, GL_TEXTURE_2D);
	    }

      std::string shaderPath = m_shaderManager->getPath();

      if(j == 1)
      {
        std::string simpleVert = "Shader/simpleSkinningShader.vert";
        std::string simpleFrag = "Shader/simpleSkinningShader.frag";

        out_materials[j] = m_materialManager->addObject(Material(Material::MaterialData(1.0f, 1.0f, 1.0f, 1.0f, false), textures, m_shaderManager->addObject(Shader((shaderPath + simpleVert).c_str(), (shaderPath + simpleFrag).c_str(), nullptr, nullptr, nullptr, nullptr))));
      }
      else
      {
        std::string simpleVert = "Shader/simpleShader.vert";
        std::string simpleFrag = "Shader/simpleShader.frag";

        out_materials[j] = m_materialManager->addObject(Material(Material::MaterialData(1.0f, 1.0f, 1.0f, 1.0f, false), textures, m_shaderManager->addObject(Shader((shaderPath + simpleVert).c_str(), (shaderPath + simpleFrag).c_str(), nullptr, nullptr, nullptr, nullptr))));
      }

      for(unsigned int i = 0; i != textures.size(); i++)
      {
        textures[i].clear();
      }

      textures.clear();
    }
  }

  GroupNode* AssimpLoader::loadSceneGraphFromAssimp(std::string filename, const aiNode *rootNode, std::vector<ResourceHandle> meshes)
  {
    TransformNode *sceneRootNode = new TransformNode(Matrix<float, 4>::identity(), filename);
    sceneRootNode->setFirstChild(createSceneNodes(rootNode, meshes, sceneRootNode, nullptr));

    return sceneRootNode;
  }

  TreeNode* AssimpLoader::createSceneNodes(const aiNode *node, std::vector<ResourceHandle> meshes, GroupNode *parentNode, TreeNode *nextSibling)
  {
    TransformNode *transformNode = nullptr;

    std::map<std::string, std::vector<AnimationTrack>>::iterator mit = m_animationTracks.find(std::string(node->mName.C_Str()));

    if(mit != m_animationTracks.end())
    {
      transformNode = new AnimatedTransformNode(mit->second, std::string(node->mName.C_Str()), parentNode, nextSibling, nullptr);
      m_boneTable[transformNode->getNodeName()] = dynamic_cast<AnimatedTransformNode*>(transformNode);
    }
    else
    {
      Matrix<float, 4> tmpTransformationMatrix;
      memcpy(&(tmpTransformationMatrix[0][0]), &(node->mTransformation[0][0]), sizeof(Matrix<float, 4>));
      tmpTransformationMatrix = tmpTransformationMatrix.transpose();

      transformNode = new TransformNode(tmpTransformationMatrix, std::string(node->mName.C_Str()), parentNode, nextSibling, nullptr);
    }

    transformNode->addDirtyFlag(GroupNode::TRF_DIRTY);//animations need to be updated

    parentNode = transformNode;
    nextSibling = nullptr;

    std::stringstream stream;
    GeoNode *geoNode = nullptr;
    Matrix<float, 4> tmpBoneMatrix;

    for(unsigned int i = 0; i < node->mNumMeshes; i++)//all meshes are children of the current transformation node
    {
      unsigned int meshIndex = node->mMeshes[i];

      stream << i;
      if(!m_inverseBindPoseTable[meshIndex].empty())
      {
        geoNode = new AnimatedGeoNode(m_inverseBindPoseTable[meshIndex], meshes[meshIndex], true, std::string(node->mName.C_Str()) + std::string("_Mesh") + stream.str(), parentNode, nextSibling);
        m_skinnedMeshTable[dynamic_cast<AnimatedGeoNode*>(geoNode)] = m_boneNameTable[meshIndex];
      }
      else
      {
        geoNode = new GeoNode(meshes[node->mMeshes[i]], true, std::string(node->mName.C_Str()) + std::string("_Mesh") + stream.str(), parentNode, nextSibling);
      }
      
      stream.clear();
      nextSibling = geoNode;
    }

    for(unsigned int i = 0; i < node->mNumChildren; i++)//all further child nodes of the current transformation node
    {
      nextSibling = createSceneNodes(node->mChildren[i], meshes, parentNode, nextSibling);
    }

    if(node->mNumMeshes != 0 || node->mNumChildren != 0)
    {
      parentNode->setFirstChild(nextSibling);//the last sibling index must be the first childindex
    }

    return transformNode;
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

        AnimationTrack& animationTrack = m_animationTracks[nodeName][i];

        animationTrack.m_positions.resize(channel->mNumPositionKeys);
        animationTrack.m_positionsTime.resize(channel->mNumPositionKeys);

        animationTrack.m_rotations.resize(channel->mNumRotationKeys);
        animationTrack.m_rotationsTime.resize(channel->mNumRotationKeys);

        animationTrack.m_scales.resize(channel->mNumScalingKeys);
        animationTrack.m_scalesTime.resize(channel->mNumScalingKeys);

        animationTrack.m_animationName = animationName;
        animationTrack.m_duration = scene->mAnimations[i]->mDuration;
        animationTrack.m_animationTicksPerSecond = scene->mAnimations[i]->mTicksPerSecond == 0.0 ? m_animationTimeUnitConvert : scene->mAnimations[i]->mTicksPerSecond / m_animationTimeUnitConvert;

        for(unsigned int k = 0; k < channel->mNumPositionKeys; k++)
        {
          aiVector3D position = channel->mPositionKeys[k].mValue;
          animationTrack.m_positions[k] = Vector<float, 3>(position.x, position.y, position.z);
          animationTrack.m_positionsTime[k] = channel->mPositionKeys[k].mTime;
        }

        for(unsigned int k = 0; k < channel->mNumRotationKeys; k++)
        {
          aiQuaternion rotation = channel->mRotationKeys[k].mValue;
          animationTrack.m_rotations[k] = Quaternion<float>(rotation.w, rotation.x, rotation.y, rotation.z).invert();
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
    for(std::map<AnimatedGeoNode*, std::vector<std::string>>::iterator mit = m_skinnedMeshTable.begin(); mit != m_skinnedMeshTable.end(); mit++)
    {
      AnimatedGeoNode *geoNode = mit->first;
      for(unsigned int i = 0; i < mit->second.size(); i++)
      {
        std::map<std::string, AnimatedTransformNode*>::iterator animNode = m_boneTable.find(mit->second[i]);
        animNode->second->setSkinnedMesh(geoNode);
        animNode->second->setBoneIndex(i);
      }
    }
  }
}