#include "Utilities/Miscellaneous/CacheIndicesGenerator.h"

#include <cassert>
#include <random>
#include <cmath>


namespace he
{
  namespace util
  {
    CacheIndicesGenerator::CacheIndicesGenerator() : m_newVertexNumber(0)
    {

    }

    void CacheIndicesGenerator::generateCacheIndizes(const std::vector<util::vec3f>& positions, const std::vector<util::vec3f>& normals, const std::vector<util::Cache>& caches, std::vector<util::cacheIndexType>& cacheIndizes0, std::vector<util::cacheIndexType>& cacheIndizes1)
    {
      for(unsigned int i = 0; i < positions.size(); i += 3)
      {
        std::vector<std::vector<util::vec2f>> cacheIndices(3);//3 * 8 indices for a triangle. x = index | y = weight

        for(unsigned int j = 0; j < caches.size(); j++)
        {
          for(unsigned int k = 0; k < 3; k++)
          {
            float length = (positions[i + k] - caches[j].position).length();
            float weight = util::vec3f::dot(normals[i + k], caches[j].normal) / (length * length + 0.0001f);//more weight is better

            bool inserted = false;
            for(std::vector<util::vec2f>::iterator cit = cacheIndices[k].begin(); cit != cacheIndices[k].end(); cit++)
            {
              if(weight >(*cit)[1])
              {
                if(cacheIndices[k].size() < CACHEINDEXSIZE)
                {
                  cacheIndices[k].insert(cit, util::vec2f(float(j), weight));
                }
                else
                {
                  (*cit) = util::vec2f(float(j), weight);
                }
              }
            }

            if(!inserted && cacheIndices[k].size() < CACHEINDEXSIZE)//if the new weight is the lowest we need to push it to the back
            {
              cacheIndices[k].push_back(util::vec2f(float(j), weight));
            }
          }
        }

        for(unsigned int k = 0; k < 3; k++)
        {
          if(cacheIndices[k].size() < CACHEINDEXSIZE)
          {
            cacheIndices[k].resize(CACHEINDEXSIZE, util::vec2f(INT32_MAX, 0));
          }

          cacheIndizes0[i + k] = util::cacheIndexType(cacheIndices[k][0][0], cacheIndices[k][1][0], cacheIndices[k][2][0], cacheIndices[k][3][0]);
          cacheIndizes1[i + k] = util::cacheIndexType(cacheIndices[k][4][0], cacheIndices[k][5][0], cacheIndices[k][6][0], cacheIndices[k][7][0]);
        }
      }
    }

    void CacheIndicesGenerator::createVertexDataStructure(const std::vector<util::vec3f>& positions, const std::vector<util::vec3f>& normals, const std::vector<util::Cache>& caches, const std::vector<unsigned int>& indices)
    {
      for(unsigned int i = 0; i < positions.size(); i++)
      {
        m_vertexDataStructure[i] = VertexDataStructure(positions[i], normals[i], caches, i);
      }

      for(unsigned int i = 0; i < positions.size(); i++)
      {
        std::unordered_set<unsigned int> inNeighbors;

        for(unsigned int j = 0; j < indices.size(); j += 3)//create the neighbors indexing
        {
          if(indices[j + 0] == i || indices[j + 1] == i || indices[j + 2] == i)//is the vertex part of the actual triangle?
          {
            for(unsigned int k = 0; k < 3; k++)//if yes add the neighbors to the list
            {
              if(indices[j + k] != i)//insert it only if its not the own index
              {
                m_vertexDataStructure[i].neighbors.insert(indices[j + k]);
              }
            }
          }
        }
      }
    }

    void CacheIndicesGenerator::insertNewVertex(const std::vector<util::Cache>& caches, unsigned int vertexIndex0, unsigned int vertexIndex1, std::vector<unsigned int>& inoutIndices)
    {
      m_newVertexEdges.push_back(util::vec2ui(vertexIndex0, vertexIndex1));

      unsigned int vertexIndex2 = UINT32_MAX, vertexIndex3 = UINT32_MAX;

      for(std::unordered_set<unsigned int>::iterator nit0 = m_vertexDataStructure[vertexIndex0].neighbors.begin(); nit0 != m_vertexDataStructure[vertexIndex0].neighbors.end(); nit0++)//find the other two neighbors of the two triangles
      {
        std::unordered_set<unsigned int>::iterator nit1 = m_vertexDataStructure[vertexIndex1].neighbors.find(*nit0);
        if(nit1 != m_vertexDataStructure[vertexIndex1].neighbors.end())
        {
          if(vertexIndex2 == UINT32_MAX)
          {
            vertexIndex2 = *nit0;
          }
          else
          {
            vertexIndex3 = *nit0;
          }
        }
      }

      m_vertexDataStructure[vertexIndex0].neighbors.erase(m_vertexDataStructure[vertexIndex0].neighbors.find(vertexIndex1));//erase the old neighbors
      m_vertexDataStructure[vertexIndex1].neighbors.erase(m_vertexDataStructure[vertexIndex1].neighbors.find(vertexIndex0));

      util::vec3f newPosition = util::vec3f::lerp(m_vertexDataStructure[vertexIndex0].position, m_vertexDataStructure[vertexIndex1].position, 0.5f);
      util::vec3f newNormal = util::vec3f::lerp(m_vertexDataStructure[vertexIndex0].normal, m_vertexDataStructure[vertexIndex1].normal, 0.5f);

      VertexDataStructure newVertex(newPosition, newNormal, caches, m_vertexDataStructure.size());

      //add the new neighbors
      newVertex.neighbors.insert(&vertexIndex0);
      newVertex.neighbors.insert(&vertexIndex1);
      newVertex.neighbors.insert(vertexIndex2);
      newVertex.neighbors.insert(vertexIndex3);

      for(std::unordered_set<unsigned int>::iterator nit = newVertex.neighbors.begin(); nit != newVertex.neighbors.end(); nit++)
      {
        m_vertexDataStructure[*nit].neighbors.insert(&newVertex);
      }

      m_vertexDataStructure.push_back(newVertex);

      //////////Insert New Triangles//////////
      //first triangle subdivision
      inoutIndices.push_back(m_vertexDataStructure[vertexIndex0].index);
      inoutIndices.push_back(newVertex.index);
      inoutIndices.push_back(m_vertexDataStructure[vertexIndex2].index);

      inoutIndices.push_back(newVertex.index);
      inoutIndices.push_back(m_vertexDataStructure[vertexIndex1].index);
      inoutIndices.push_back(m_vertexDataStructure[vertexIndex2].index);

      if(vertexIndex3 != UINT32_MAX)
      {
        //second triangle subdivision
        inoutIndices.push_back(m_vertexDataStructure[vertexIndex0].index);
        inoutIndices.push_back(m_vertexDataStructure[vertexIndex3].index);
        inoutIndices.push_back(newVertex.index);

        inoutIndices.push_back(newVertex.index);
        inoutIndices.push_back(m_vertexDataStructure[vertexIndex3].index);
        inoutIndices.push_back(m_vertexDataStructure[vertexIndex1].index);
      }

      //////////Erase old Triangles//////////
      for(unsigned int i = 0; i < inoutIndices.size(); i += 3)
      {
        if((inoutIndices[i + 0] == m_vertexDataStructure[vertexIndex0].index || inoutIndices[i + 1] == m_vertexDataStructure[vertexIndex0].index || inoutIndices[i + 2] == m_vertexDataStructure[vertexIndex0].index) &&
          (inoutIndices[i + 0] == m_vertexDataStructure[vertexIndex1].index || inoutIndices[i + 1] == m_vertexDataStructure[vertexIndex1].index || inoutIndices[i + 2] == m_vertexDataStructure[vertexIndex1].index))
        {
          inoutIndices[i + 0] = UINT32_MAX;
          inoutIndices[i + 1] = UINT32_MAX;
          inoutIndices[i + 2] = UINT32_MAX;
        }
      }
    }

    void CacheIndicesGenerator::getCacheInterpolationEdges(VertexDataStructure& vertex, std::list<unsigned int>& verticesWithSameCacheIndices)//returns the indices of the vertices which have less than CACHEINDEXSIZE distinct cache-indices
    {
      unsigned int usedCacheSlots = 0;
      for(unsigned int j = 0; j < vertex.cacheIndices.size(); j++)//count the used cache-slots
      {
        if(vertex.cacheIndices[j][0] != INT32_MAX)
        {
          usedCacheSlots++;
        }
      }

      for(unsigned int j = 0; j < vertex.cacheIndices.size(); j++)
      {
        if(vertex.cacheIndices[j][0] == INT32_MAX)
        {
          continue;
        }

        for(std::unordered_set<unsigned int>::iterator nit = vertex.neighbors.begin(); nit != vertex.neighbors.end(); nit++)
        {
          bool hasTheCache = false;
          unsigned int validCacheIndicesNumber = 0;
          for(unsigned int k = 0; k < m_vertexDataStructure[*nit].cacheIndices.size(); k++)
          {
            if(m_vertexDataStructure[*nit].cacheIndices[k][0] != INT32_MAX)
            {
              validCacheIndicesNumber++;
            }

            if(m_vertexDataStructure[*nit].cacheIndices[k][0] != INT32_MAX && m_vertexDataStructure[*nit].cacheIndices[k][0] == vertex.cacheIndices[j][0])
            {
              hasTheCache = true;
            }
          }

          if(hasTheCache && validCacheIndicesNumber == CACHEINDEXSIZE)
          {
            if(usedCacheSlots > CACHEINDEXSIZE)//do we have more cache indices than necessary?
            {
              usedCacheSlots--;
              vertex.cacheIndices[j] = util::vec2f(INT32_MAX, 0.0f);
            }
            else//if not create a new vertex
            {
              verticesWithSameCacheIndices.push_back(*nit);
            }
          }
        }
      }

      unsigned int unusedCacheSlots = 0;
      for(unsigned int j = 0; j < vertex.cacheIndices.size(); j++)//resort the cache-indices so that valid indices are tight together
      {
        if(vertex.cacheIndices[j][0] == INT32_MAX)
        {
          unusedCacheSlots++;
        }
        else
        {
          vertex.cacheIndices[j - unusedCacheSlots] = vertex.cacheIndices[j];
        }
      }

      vertex.cacheIndices.resize(CACHEINDEXSIZE);
    }

    void CacheIndicesGenerator::eraseDoubledIndices(VertexDataStructure& vertex)//erases doubled indices from its neighbors
    {
      for(unsigned int j = 0; j < vertex.cacheIndices.size(); j++)
      {
        for(std::unordered_set<unsigned int>::iterator nit = vertex.neighbors.begin(); nit != vertex.neighbors.end(); nit++)
        {
          for(unsigned int k = 0; k < m_vertexDataStructure[*nit].cacheIndices.size(); k++)
          {
            if(m_vertexDataStructure[*nit].cacheIndices[k][0] == vertex.cacheIndices[j][0])
            {
              m_vertexDataStructure[*nit].cacheIndices[k] = util::vec2f(INT32_MAX, 0.0f);//invalid the cache-index if the neighbor vertex with more neighbors has it already in it's top eight
              break;
            }
          }
        }
      }
    }

    void CacheIndicesGenerator::generateCacheIndizes(const std::vector<util::vec3f>& positions, const std::vector<util::vec3f>& normals, std::vector<unsigned int>& indices, const std::vector<util::Cache>& caches, std::vector<util::cacheIndexType>& cacheIndizes0, std::vector<util::cacheIndexType>& cacheIndizes1)
    {
      m_vertexDataStructure.resize(positions.size());

      std::list<unsigned int> verticesWithSameCacheIndices;

      createVertexDataStructure(positions, normals, caches, indices);

      unsigned int oldVertexNumber = m_vertexDataStructure.size();

      for(unsigned int i = 0; i < m_vertexDataStructure.size(); i++)
      {
        getCacheInterpolationEdges(m_vertexDataStructure[i], verticesWithSameCacheIndices);

        for(std::list<unsigned int>::iterator it = verticesWithSameCacheIndices.begin(); it != verticesWithSameCacheIndices.end(); it++)
        {
          insertNewVertex(caches, i, *it, indices);
        }

        eraseDoubledIndices(m_vertexDataStructure[i]);
      }

      cacheIndizes0.resize(m_vertexDataStructure.size());
      cacheIndizes1.resize(m_vertexDataStructure.size());

      for(unsigned int i = 0; i < m_vertexDataStructure.size(); i++)//assign the preprocessed indices to the final format
      {
        cacheIndizes0[i] = util::cacheIndexType(m_vertexDataStructure[i].cacheIndices[0][0], m_vertexDataStructure[i].cacheIndices[1][0], m_vertexDataStructure[i].cacheIndices[2][0], m_vertexDataStructure[i].cacheIndices[3][0]);
        cacheIndizes1[i] = util::cacheIndexType(m_vertexDataStructure[i].cacheIndices[4][0], m_vertexDataStructure[i].cacheIndices[5][0], m_vertexDataStructure[i].cacheIndices[6][0], m_vertexDataStructure[i].cacheIndices[7][0]);
      }

      std::vector<unsigned int> newIndices(indices.size());

      for(unsigned int i = 0; i < indices.size(); i += 3)
      {
        if(indices[i] != UINT32_MAX)
        {
          newIndices.push_back(indices[i + 0]);
          newIndices.push_back(indices[i + 1]);
          newIndices.push_back(indices[i + 2]);
        }
      }

      indices = newIndices;

      m_newVertexNumber = m_vertexDataStructure.size() - oldVertexNumber;
    }

    const std::vector<VertexDataStructure>& CacheIndicesGenerator::getVertexDataStructure() const
    {
      return m_vertexDataStructure;
    }

    const std::list<util::vec2ui>& CacheIndicesGenerator::getNewVertexEdgeIndices() const
    {
      return m_newVertexEdges;
    }

    unsigned int CacheIndicesGenerator::getNewVertexNumber() const
    {
      return m_newVertexNumber;
    }
  }
}