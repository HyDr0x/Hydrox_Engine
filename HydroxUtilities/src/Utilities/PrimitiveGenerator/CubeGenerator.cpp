#include "Utilities/PrimitiveGenerator/CubeGenerator.h"

#include "Utilities/PrimitiveGenerator/PlaneGenerator.h"

#include <vector>
#include <string>

namespace he
{
  namespace util
  {
    void CubeGenerator::generateCube(std::vector<vec3f>& position, unsigned int levelOfDetail)
    {
      if(levelOfDetail == UINT_MAX)
      {
        generateNonIndexedPositions(position);
      }
      else
      {
        std::vector<vec3f> planePositions, transformedPositions;

        PlaneGenerator::generatePlane(planePositions, levelOfDetail);

        transformedPositions.resize(planePositions.size());

        Quaternion<float> rotQuats[6];
        rotQuats[0] = math::createRotXQuaternion(math::PI_HALF);
        rotQuats[1] = math::createRotXQuaternion(-math::PI_HALF);
        rotQuats[2] = math::createRotZQuaternion(math::PI_HALF);
        rotQuats[3] = math::createRotZQuaternion(-math::PI_HALF);
        rotQuats[4] = math::createRotXQuaternion(math::PI);
        rotQuats[5] = Quaternion<float>::identity();

        vec3f translate[6];
        translate[0] = vec3f(0.0f, 0.0f, -0.5f);
        translate[1] = vec3f(0.0f, 0.0f, 0.5f);
        translate[2] = vec3f(0.5f, 0.0f, 0.0f);
        translate[3] = vec3f(-0.5f, 0.0f, 0.0f);
        translate[4] = vec3f(0.0f, -0.5f, 0.0f);
        translate[5] = vec3f(0.0f, 0.5f, 0.0f);

        for(unsigned int i = 0; i < 6; i++)
        {
          for(unsigned int j = 0; j < planePositions.size(); j++)
          {
            transformedPositions[j] = translate[i] + rotQuats[i].apply(planePositions[j]);
          }

          position.insert(position.begin() + transformedPositions.size() * i, transformedPositions.begin(), transformedPositions.end());
        }
      }
    }

    void CubeGenerator::generateCube(std::vector<vec3f>& position, std::vector<vec3f>& normal, unsigned int levelOfDetail)
    {
      if(levelOfDetail == UINT_MAX)
      {
        generateNonIndexedPositions(position);
        generateNonIndexedNormals(normal, position);
      }
      else
      {
        std::vector<vec3f> planePositions, transformedPositions;
        std::vector<vec3f> planeNormals, transformedNormals;

        PlaneGenerator::generatePlane(planePositions, planeNormals, levelOfDetail);

        transformedPositions.resize(planePositions.size());
        transformedNormals.resize(planeNormals.size());

        Quaternion<float> rotQuats[6];
        rotQuats[0] = math::createRotXQuaternion(math::PI_HALF);
        rotQuats[1] = math::createRotXQuaternion(-math::PI_HALF);
        rotQuats[2] = math::createRotZQuaternion(math::PI_HALF);
        rotQuats[3] = math::createRotZQuaternion(-math::PI_HALF);
        rotQuats[4] = math::createRotXQuaternion(math::PI);
        rotQuats[5] = Quaternion<float>::identity();

        vec3f translate[6];
        translate[0] = vec3f(0.0f, 0.0f, -0.5f);
        translate[1] = vec3f(0.0f, 0.0f, 0.5f);
        translate[2] = vec3f(0.5f, 0.0f, 0.0f);
        translate[3] = vec3f(-0.5f, 0.0f, 0.0f);
        translate[4] = vec3f(0.0f, -0.5f, 0.0f);
        translate[5] = vec3f(0.0f, 0.5f, 0.0f);

        for(unsigned int i = 0; i < 6; i++)
        {
          for(unsigned int j = 0; j < planePositions.size(); j++)
          {
            transformedPositions[j] = translate[i] + rotQuats[i].apply(planePositions[j]);
            transformedNormals[j] = rotQuats[i].apply(planeNormals[j]);
          }
          position.insert(position.begin() + transformedPositions.size() * i, transformedPositions.begin(), transformedPositions.end());
          normal.insert(normal.begin() + transformedNormals.size() * i, transformedNormals.begin(), transformedNormals.end());
        }
      }
    }

    void CubeGenerator::generateCube(std::vector<vec3f>& position, std::vector<unsigned int>& index, unsigned int levelOfDetail)
    {
      generateIndexedPositions(position, index);

      if(levelOfDetail == UINT_MAX)
      {
        generateIndexedPositions(position, index);
      }
      else
      {
        std::vector<vec3f> planePositions, transformedPositions;
        std::vector<unsigned int> indices, offsettedIndices;

        PlaneGenerator::generatePlane(planePositions, indices, levelOfDetail);

        transformedPositions.resize(planePositions.size());
        offsettedIndices.resize(indices.size());

        Quaternion<float> rotQuats[6];
        rotQuats[0] = math::createRotXQuaternion(math::PI_HALF);
        rotQuats[1] = math::createRotXQuaternion(-math::PI_HALF);
        rotQuats[2] = math::createRotZQuaternion(math::PI_HALF);
        rotQuats[3] = math::createRotZQuaternion(-math::PI_HALF);
        rotQuats[4] = math::createRotXQuaternion(math::PI);
        rotQuats[5] = Quaternion<float>::identity();

        vec3f translate[6];
        translate[0] = vec3f(0.0f, 0.0f, -0.5f);
        translate[1] = vec3f(0.0f, 0.0f, 0.5f);
        translate[2] = vec3f(0.5f, 0.0f, 0.0f);
        translate[3] = vec3f(-0.5f, 0.0f, 0.0f);
        translate[4] = vec3f(0.0f, -0.5f, 0.0f);
        translate[5] = vec3f(0.0f, 0.5f, 0.0f);

        for(unsigned int i = 0; i < 6; i++)
        {
          for(unsigned int j = 0; j < planePositions.size(); j++)
          {
            transformedPositions[j] = translate[i] + rotQuats[i].apply(planePositions[j]);
          }

          for(unsigned int j = 0; j < indices.size(); j++)
          {
            offsettedIndices[j] = indices[j] + planePositions.size() * i;
          }

          position.insert(position.begin() + transformedPositions.size() * i, transformedPositions.begin(), transformedPositions.end());
          index.insert(index.begin() + offsettedIndices.size() * i, offsettedIndices.begin(), offsettedIndices.end());
        }
      }
    }

    void CubeGenerator::generateCube(std::vector<vec3f>& position, std::vector<unsigned int>& index, std::vector<vec3f>& normal, unsigned int levelOfDetail)
    {
      if(levelOfDetail == UINT_MAX)
      {
        generateIndexedPositions(position, index);
        generateIndexedNormals(normal, position);
      }
      else
      {
        std::vector<vec3f> planePositions, transformedPositions;
        std::vector<vec3f> planeNormals, transformedNormals;
        std::vector<unsigned int> indices, offsettedIndices;

        PlaneGenerator::generatePlane(planePositions, indices, planeNormals, levelOfDetail);

        transformedPositions.resize(planePositions.size());
        transformedNormals.resize(planeNormals.size());
        offsettedIndices.resize(indices.size());

        Quaternion<float> rotQuats[6];
        rotQuats[0] = math::createRotXQuaternion(math::PI_HALF);
        rotQuats[1] = math::createRotXQuaternion(-math::PI_HALF);
        rotQuats[2] = math::createRotZQuaternion(math::PI_HALF);
        rotQuats[3] = math::createRotZQuaternion(-math::PI_HALF);
        rotQuats[4] = math::createRotXQuaternion(math::PI);
        rotQuats[5] = Quaternion<float>::identity();

        vec3f translate[6];
        translate[0] = vec3f(0.0f, 0.0f, -0.5f);
        translate[1] = vec3f(0.0f, 0.0f, 0.5f);
        translate[2] = vec3f(0.5f, 0.0f, 0.0f);
        translate[3] = vec3f(-0.5f, 0.0f, 0.0f);
        translate[4] = vec3f(0.0f, -0.5f, 0.0f);
        translate[5] = vec3f(0.0f, 0.5f, 0.0f);

        for(unsigned int i = 0; i < 6; i++)
        {
          for(unsigned int j = 0; j < planePositions.size(); j++)
          {
            transformedPositions[j] = translate[i] + rotQuats[i].apply(planePositions[j]);
            transformedNormals[j] = rotQuats[i].apply(planeNormals[j]);
          }

          for(unsigned int j = 0; j < indices.size(); j++)
          {
            offsettedIndices[j] = indices[j] + planePositions.size() * i;
          }

          position.insert(position.begin() + transformedPositions.size() * i, transformedPositions.begin(), transformedPositions.end());
          normal.insert(normal.begin() + transformedNormals.size() * i, transformedNormals.begin(), transformedNormals.end());
          index.insert(index.begin() + offsettedIndices.size() * i, offsettedIndices.begin(), offsettedIndices.end());
        }
      }
    }

    void CubeGenerator::generateIndexedPositions(std::vector<vec3f>& position, std::vector<unsigned int>& index)
    {
      position.resize(8);

      position[0] = vec3f(-0.5f, -0.5f, -0.5f);
      position[1] = vec3f( 0.5f, -0.5f, -0.5f);
      position[2] = vec3f(-0.5f, -0.5f,  0.5f);
      position[3] = vec3f( 0.5f, -0.5f,  0.5f);

      position[4] = vec3f(-0.5f,  0.5f, -0.5f);
      position[5] = vec3f( 0.5f,  0.5f, -0.5f);
      position[6] = vec3f(-0.5f,  0.5f,  0.5f);
      position[7] = vec3f( 0.5f,  0.5f,  0.5f);

      index.resize(36);//CCW is OGL's default front facing

      index[0] = 0;//front
      index[1] = 4;
      index[2] = 1;

      index[3] = 4;
      index[4] = 5;
      index[5] = 1;

      index[6] = 5;//right
      index[7] = 3;
      index[8] = 1;

      index[9] = 5;
      index[10] = 7;
      index[11] = 3;

      index[12] = 4;//left
      index[13] = 2;
      index[14] = 6;

      index[15] = 4;
      index[16] = 0;
      index[17] = 2;

      index[18] = 7;//back
      index[19] = 6;
      index[20] = 3;

      index[21] = 6;
      index[22] = 2;
      index[23] = 3;

      index[24] = 7;//top
      index[25] = 4;
      index[26] = 6;

      index[27] = 7;
      index[28] = 5;
      index[29] = 4;

      index[30] = 3;//down
      index[31] = 2;
      index[32] = 0;

      index[33] = 3;
      index[34] = 0;
      index[35] = 1;
    }

    void CubeGenerator::generateNonIndexedPositions(std::vector<vec3f>& position)
    {
      position.resize(36);
      //front
      position[0] = vec3f(-0.5f, -0.5f, -0.5f);
      position[1] = vec3f( 0.5f,  0.5f, -0.5f);
      position[2] = vec3f( 0.5f, -0.5f, -0.5f);

      position[3] = vec3f(-0.5f, -0.5f, -0.5f);
      position[4] = vec3f(-0.5f,  0.5f, -0.5f);
      position[5] = vec3f( 0.5f,  0.5f, -0.5f);

      //back
      position[6] = vec3f(-0.5f, -0.5f, 0.5f);
      position[7] = vec3f( 0.5f, 0.5f,  0.5f);
      position[8] = vec3f(-0.5f, 0.5f,  0.5f);

      position[9] = vec3f(-0.5f, -0.5f, 0.5f); 
      position[10] = vec3f(0.5f, -0.5f, 0.5f); 
      position[11] = vec3f(0.5f, 0.5f, 0.5f);

      //right
      position[12] = vec3f(-0.5f, -0.5f, 0.5f);
      position[13] = vec3f(-0.5f, 0.5f, -0.5f);
      position[14] = vec3f(-0.5f, -0.5f, -0.5f); 

      position[15] = vec3f(-0.5f, -0.5f, 0.5f);
      position[16] = vec3f(-0.5f, 0.5f, 0.5f);
      position[17] = vec3f(-0.5f, 0.5f, -0.5f);

      //left
      position[18] = vec3f(0.5f, 0.5f, -0.5f);
      position[19] = vec3f(0.5f, -0.5f, 0.5f);
      position[20] = vec3f(0.5f, -0.5f, -0.5f);

      position[21] = vec3f( 0.5f,  0.5f, -0.5f);
      position[22] = vec3f( 0.5f,  0.5f,  0.5f);
      position[23] = vec3f( 0.5f, -0.5f,  0.5f);

      //bottom
      position[24] = vec3f(-0.5f, -0.5f, -0.5f);
      position[25] = vec3f(0.5f, -0.5f, 0.5f);
      position[26] = vec3f(-0.5f, -0.5f, 0.5f);

      position[27] = vec3f(-0.5f, -0.5f, -0.5f);
      position[28] = vec3f( 0.5f, -0.5f, -0.5f);
      position[29] = vec3f( 0.5f, -0.5f,  0.5f);

      //top
      position[30] = vec3f(-0.5f,  0.5f, -0.5f);
      position[31] = vec3f( 0.5f,  0.5f,  0.5f);
      position[32] = vec3f( 0.5f,  0.5f, -0.5f);

      position[33] = vec3f(-0.5f,  0.5f, -0.5f);
      position[34] = vec3f(-0.5f,  0.5f,  0.5f);
      position[35] = vec3f( 0.5f,  0.5f,  0.5f);
    }

    void CubeGenerator::generateIndexedNormals(std::vector<vec3f>& normal, const std::vector<vec3f>& positions)
    {
      normal.resize(8);
      for(unsigned int i = 0; i < positions.size(); i++)
      {
        normal[i] = positions[i].normalize();
      }
    }

    void CubeGenerator::generateNonIndexedNormals(std::vector<vec3f>& normal, const std::vector<vec3f>& positions)
    {
      normal.resize(36);

      for(unsigned int i = 0; i < positions.size(); i += 3)
      {
        vec3f a = positions[i + 1] - positions[i];
        vec3f b = positions[i + 2] - positions[i];
        normal[i] = normal[i + 1] = normal[i + 2] = math::cross(a, b).normalize();
      }
    }

    void CubeGenerator::generateIndexedTextures(std::vector<vec3f>& texture)
    {
    }

    void CubeGenerator::generateNonIndexedTextures(std::vector<vec3f>& texture)
    {
    }
  }
}