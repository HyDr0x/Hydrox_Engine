#include "Hydrox/Loader/AssimpLoader.h"

#include <sstream>

#include "Hydrox/Utility/Tree/GroupNode.h"
#include "Hydrox/Utility/Tree/GeoNode.h"
#include "Hydrox/Utility/Tree/LODNode.h"
#include "Hydrox/Utility/Tree/TransformNode.h"

#include "Hydrox/Loader/ILDevilLoader.h"

#include "Hydrox/Graphics/Scene.h"
#include "Hydrox/Graphics/Texture.h"

AssimpLoader::AssimpLoader(ModelManager *modelManager, MaterialManager *materialManager, TextureManager *textureManager, ShaderManager *shaderManager) : m_modelManager(modelManager),
                                                                                                                                                         m_materialManager(materialManager),
                                                                                                                                                         m_textureManager(textureManager),
                                                                                                                                                         m_shaderManager(shaderManager)
{
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

Scene* AssimpLoader::load(std::string filename, std::string materialFileName, bool yAxisFlipped)
{
  Assimp::Importer importer;
	const aiScene *assimpScene = importer.ReadFile(m_modelManager->getPath() + filename, aiProcessPreset_TargetRealtime_MaxQuality ^ aiProcess_GenUVCoords);

	if(!assimpScene)
  {
		printf("%s", importer.GetErrorString());
  }

	assert(assimpScene);

  std::vector<ResourceHandle> meshes;//contains the mesh id's of the scene

  loadMeshesFromAssimp(meshes, materialFileName, assimpScene, yAxisFlipped);

  loadAnimatedSkeleton(assimpScene);

  GroupNode *rootNode = loadSceneGraphFromAssimp(filename, assimpScene->mRootNode, meshes);

  Scene *scene = new Scene(rootNode, Vec<float, 3>::identity());

	importer.FreeScene();

	return scene;
}

void AssimpLoader::loadMeshesFromAssimp(std::vector<ResourceHandle>& out_meshes, std::string materialFileName, const aiScene *scene, bool yAxisFlipped)
{
  std::vector<ResourceHandle> materialBuffer;//holds the information of the old ID in the scene graph and the new ID of the material

  materialBuffer.resize(scene->mNumMaterials, ~0);
  out_meshes.resize(scene->mNumMeshes);

  ResourceHandle tmpMaterialIndex;
  
	for(unsigned int i = 0; i != scene->mNumMeshes; i++)
	{
		if(materialBuffer[scene->mMeshes[i]->mMaterialIndex] == ~0)//if material wasn't loaded yet, do it!
		{
      tmpMaterialIndex = loadMaterialsFromAssimp(materialFileName, scene, scene->mMeshes[i]->mMaterialIndex);
			materialBuffer[scene->mMeshes[i]->mMaterialIndex] = tmpMaterialIndex;//remember where the model was which has the material data already converted and loaded
		}
		else
    {
      tmpMaterialIndex = materialBuffer[scene->mMeshes[i]->mMaterialIndex];//if material is already loaded, just copy it
    }

    out_meshes[i] = loadVertices(scene->mMeshes[i], tmpMaterialIndex, yAxisFlipped);
	}
}

ResourceHandle AssimpLoader::loadVertices(const aiMesh *mesh, ResourceHandle materialIndex, bool yAxisFlipped)
{
  GLuint vertexDeclarationFlags;

  vertexDeclarationFlags |= mesh->HasPositions()              ? Mesh::MODEL_POSITION     : 0;
  vertexDeclarationFlags |= mesh->HasTextureCoords(0)         ? Mesh::MODEL_TEXTURE      : 0;
  vertexDeclarationFlags |= mesh->HasNormals()                ? Mesh::MODEL_NORMAL       : 0;
  vertexDeclarationFlags |= mesh->HasTangentsAndBitangents()  ? Mesh::MODEL_BINORMAL     : 0;
  vertexDeclarationFlags |= mesh->HasBones()                  ? Mesh::MODEL_BONE_WEIGHTS : 0;
  vertexDeclarationFlags |= mesh->HasBones()                  ? Mesh::MODEL_BONE_INDICES : 0;

  std::vector<Vec<float, 3>> positions;
  std::vector<Vec<float, 2>> textureCoords;
  std::vector<Vec<float, 3>> normals;
  std::vector<Vec<float, 3>> binormals;
  std::vector<Vec<unsigned int, 4>> boneIndices;
  std::vector<Vec<float, 4>> boneWeights;
  std::vector<Mesh::indexType> indices;

	if(mesh->HasPositions())
	{
    positions.resize(mesh->mNumVertices);

    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
      positions[i] = Vec<float, 3>(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
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

      textureCoords[i] = Vec<float, 2>(mesh->mTextureCoords[0][i][0], tmpYCoordinates);//if the model is from DX the y-axis must be flipped
		}
	}

	if(mesh->HasNormals())
	{
    normals.resize(mesh->mNumVertices);
    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
      normals[i] = Vec<float, 3>(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
    }
	}

	if(mesh->HasTangentsAndBitangents())
	{
    binormals.resize(mesh->mNumVertices);
    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
      binormals[i] = Vec<float, 3>(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
    }
	}

	if(mesh->HasBones())//TODO SKINNED ANIMATION
	{
    boneWeights.resize(mesh->mNumVertices, Vec<float, 4>::identity());
    boneIndices.resize(mesh->mNumVertices, Vec<unsigned int, 4>(~0, ~0, ~0, ~0));
    for(unsigned int i = 0; i < mesh->mNumBones; i++)
		{
      mesh->mBones[i]->mName;
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

ResourceHandle AssimpLoader::loadMaterialsFromAssimp(std::string materialFileName, const aiScene *model, unsigned int id)
{
  std::vector< std::vector<ResourceHandle> > textures;
  textures.resize(4);
  textures[0].resize(model->mMaterials[id]->GetTextureCount(aiTextureType_DIFFUSE));
  textures[1].resize(model->mMaterials[id]->GetTextureCount(aiTextureType_NORMALS));
  textures[2].resize(model->mMaterials[id]->GetTextureCount(aiTextureType_DISPLACEMENT));
  textures[3].resize(model->mMaterials[id]->GetTextureCount(aiTextureType_SPECULAR));
  
  aiString texPath;

  ILDevilLoader texLoader(m_textureManager);

	for(unsigned int k = 0; k != textures[0].size(); k++)
	{
		model->mMaterials[id]->GetTexture(aiTextureType_DIFFUSE, k, &texPath);
		textures[0][k] = texLoader.load(texPath.data, GL_TEXTURE_2D);
	}

	for(unsigned int k = 0; k != textures[1].size(); k++)
	{
		model->mMaterials[id]->GetTexture(aiTextureType_NORMALS, 0, &texPath);
		textures[1][k] = texLoader.load(texPath.data, GL_TEXTURE_2D);
	}

	for(unsigned int k = 0; k != textures[2].size(); k++)
	{
		model->mMaterials[id]->GetTexture(aiTextureType_DISPLACEMENT, 0, &texPath);
		textures[2][k] = texLoader.load(texPath.data, GL_TEXTURE_2D);
	}

	for(unsigned int k = 0;k != textures[3].size(); k++)
	{
		model->mMaterials[id]->GetTexture(aiTextureType_SPECULAR, 0, &texPath);
		textures[3][k] = texLoader.load(texPath.data, GL_TEXTURE_2D);
	}

  std::string shaderPath = m_shaderManager->getPath();

  std::string simpleVert = "Shader/simpleShader.vert";
  std::string simpleFrag = "Shader/simpleShader.frag";

  ResourceHandle tmpMaterial = m_materialManager->addObject(Material(Material::MaterialData(1.0f, 1.0f, 1.0f, 1.0f, false), textures, m_shaderManager->addObject(Shader((shaderPath + simpleVert).c_str(), (shaderPath + simpleFrag).c_str(), nullptr, nullptr, nullptr, nullptr))));

  for(unsigned int i = 0; i != textures.size(); i++)
  {
    textures[i].clear();
  }

  textures.clear();

  return tmpMaterial;
}

GroupNode* AssimpLoader::loadSceneGraphFromAssimp(std::string filename, const aiNode *rootNode, std::vector<ResourceHandle> meshes)
{
  TransformNode *sceneRootNode = new TransformNode(Mat<float, 4>::identity(), filename);
  sceneRootNode->setFirstChild(createSceneNodes(rootNode, meshes, sceneRootNode, nullptr));

  return sceneRootNode;
}

TreeNode* AssimpLoader::createSceneNodes(const aiNode *node, std::vector<ResourceHandle> meshes, GroupNode *parentNode, TreeNode *nextSibling)
{
  Mat<float, 4> tmpTransformationMatrix;
  memcpy(&(tmpTransformationMatrix[0][0]), &(node->mTransformation[0][0]), sizeof(Mat<float, 4>));
  tmpTransformationMatrix = tmpTransformationMatrix.transpose();

  TransformNode *transformNode = new TransformNode(tmpTransformationMatrix, std::string(node->mName.C_Str()), parentNode, nextSibling, nullptr);

  parentNode = transformNode;
  nextSibling = nullptr;

  std::stringstream stream;
  for(unsigned int i = 0; i < node->mNumMeshes; i++)//all meshes are children of the current transformation node
  {
    stream << i;
    GeoNode *geoNode = new GeoNode(meshes[node->mMeshes[i]], true, std::string(node->mName.C_Str()) + std::string("_Mesh") + stream.str(), parentNode, nextSibling);
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

    m_animationTable[animationName].resize(scene->mAnimations[i]->mNumChannels);

    for(unsigned int j = 0; j < scene->mAnimations[i]->mNumChannels; j++)
    {
      m_animationTable[animationName][j] = new heAnimation(animationName, scene->mAnimations[i]->mChannels[j]);
    }
  }
}

void AssimpLoader::findAnimatedSkeleton()
{

}