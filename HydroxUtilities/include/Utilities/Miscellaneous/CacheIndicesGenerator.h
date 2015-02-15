#ifndef CACHEINDICESGENERATOR_H_
#define CACHEINDICESGENERATOR_H_

#include <vector>
#include <list>
#include <unordered_set>
#include <cstdint>

#include "Utilities/Math/Math.hpp"
#include "Utilities/Miscellaneous/CacheGenerator.h"

#include "Utilities/DLLExport.h"

namespace he
{
  namespace util
  {
    struct VertexDataStructure
    {
      VertexDataStructure(){}

      VertexDataStructure(util::vec3f inPosition, util::vec3f inNormal, const std::vector<util::Cache>& caches, unsigned int vertexIndex, unsigned int cacheIndexSize, float maxDistance)
      {
        position = inPosition;
        normal = inNormal;
        cacheIndices.resize(3 * cacheIndexSize, util::vec2f(INT32_MAX, 0.0f));
        index = vertexIndex;

        for(unsigned int j = 0; j < caches.size(); j++)
        {
          float length = (position - caches[j].position).length();

          //if(length <= maxDistance)
          {
            float weight = util::vec3f::dot(normal, caches[j].normal) / (length * length + 0.0001f);//more weight is better

            if(weight > cacheIndices.back()[1])
            {
              sortCacheIndices(cacheIndices, util::vec2f(float(j), weight));
            }
          }
        }
      }

      std::unordered_set<unsigned int> neighbors;//indices of the neighbor vertices
      std::vector<util::vec2f> cacheIndices;//x = index | y = weight
      util::vec3f position;
      util::vec3f normal;
      unsigned int index;//the index of the vertex in the vertex buffer
      //float actualCacheWeight;//weight of the actual processed cache in the assign function

    private:

      void sortCacheIndices(std::vector<util::vec2f>& indices, util::vec2f newValue)
      {
        std::vector<util::vec2f> sortedIndices(indices.size());

        float highestWeight = 0.0f;
        float bestIndex = FLT_MAX;

        for(unsigned int i = 0; i < indices.size(); i++)
        {
          if(newValue[1] > indices[i][1])
          {
            bestIndex = i;
            break;
          }

          sortedIndices[i] = indices[i];
        }

        if(bestIndex == FLT_MAX)
        {
          return;
        }

        sortedIndices[bestIndex] = newValue;

        for(unsigned int i = bestIndex; i < indices.size() - 1; i++)
        {
          sortedIndices[i + 1] = indices[i];
        }

        indices = sortedIndices;
      }
    };

    class GRAPHICAPI CacheIndicesGenerator
    {
    public:

      CacheIndicesGenerator(float maxDistance);

      void generateCacheIndizes(const std::vector<util::vec3f>& positions, const std::vector<util::vec3f>& normals, const std::vector<util::Cache>& caches, std::vector<util::cacheIndexType>& cacheIndizes0, std::vector<util::cacheIndexType>& cacheIndizes1);
      void generateCacheIndizes(const std::vector<util::vec3f>& positions, const std::vector<util::vec3f>& normals, std::vector<unsigned int>& indices, const std::vector<util::Cache>& caches, std::vector<util::cacheIndexType>& cacheIndizes0, std::vector<util::cacheIndexType>& cacheIndizes1);

      const std::vector<VertexDataStructure>& getVertexDataStructure() const;
      const std::list<util::vec2ui>& getNewVertexEdgeIndices() const;
      unsigned int getNewVertexNumber() const;

    private:

      void createVertexDataStructure(const std::vector<util::vec3f>& positions, const std::vector<util::vec3f>& normals, const std::vector<util::Cache>& caches, const std::vector<unsigned int>& indices);
      void insertNewVertex(const std::vector<util::Cache>& caches, unsigned int vertexIndex0, unsigned int vertexIndex1, std::vector<unsigned int>& inoutIndices);
      void getCacheInterpolationEdges(VertexDataStructure& vertex, std::unordered_set<unsigned int>& verticesWithSameCacheIndices);
      void eraseDoubledIndices(VertexDataStructure& vertex);

      static const unsigned int CACHEINDEXSIZE = 8;

      std::vector<VertexDataStructure> m_vertexDataStructure;

      std::list<util::vec2ui> m_newVertexEdges;//x = vertex0 | y = vertex1 between those vertices is a new one being created
      std::list<unsigned int> m_nextVertexIndex;//the list which represents the order in which the vertices should be visited for the cache-index-creation algorithm

      unsigned int m_newVertexNumber;

      float m_maxDistance;
    };
  }
}

#endif