#include "Utilities/Miscellaneous/CacheIndicesGenerator.h"

#include <cassert>
#include <random>
#include <cmath>
#include <algorithm>

namespace he
{
  namespace util
  {
    CacheIndicesGenerator::CacheIndicesGenerator(float maxDistance) : m_newVertexNumber(0), m_maxDistance(maxDistance)
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
                  cacheIndices[k].insert(cit, util::vec2f(float(j + 1), weight));
                }
                else
                {
                  (*cit) = util::vec2f(float(j + 1), weight);
                }
              }
            }

            if(!inserted && cacheIndices[k].size() < CACHEINDEXSIZE)//if the new weight is the lowest we need to push it to the back
            {
              cacheIndices[k].push_back(util::vec2f(float(j + 1), weight));
            }
          }
        }

        for(unsigned int k = 0; k < 3; k++)
        {
          if(cacheIndices[k].size() < CACHEINDEXSIZE)
          {
            cacheIndices[k].resize(CACHEINDEXSIZE, util::vec2f(0, 0));
          }

          cacheIndizes0[i + k] = util::cacheIndexType(cacheIndices[k][0][0], cacheIndices[k][1][0], cacheIndices[k][2][0], cacheIndices[k][3][0]);
          cacheIndizes1[i + k] = util::cacheIndexType(cacheIndices[k][4][0], cacheIndices[k][5][0], cacheIndices[k][6][0], cacheIndices[k][7][0]);
        }
      }
    }

    void CacheIndicesGenerator::createVertexDataStructure(const std::vector<util::vec3f>& positions, const std::vector<util::vec3f>& normals, const std::vector<util::Cache>& caches, const std::vector<unsigned int>& indices)
    {
      for(unsigned int i = 0; i < positions.size(); i++)//create the vertices
      {
        m_vertexDataStructure[i] = VertexDataStructure(positions[i], normals[i], caches, i, CACHEINDEXSIZE, m_maxDistance);
      }

      for(unsigned int i = 0; i < positions.size(); i++)//create the neighbor indices
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

      std::vector<bool> alreadyInserted(m_vertexDataStructure.size());//a vector whichs saves indices of already inserted vertices
      std::list<unsigned int> actualParentList, nextParentList;//one saves the indices of all parents for the next level, the other has the indices of all parents of the actual level

      while(m_nextVertexIndex.size() < m_vertexDataStructure.size())
      {
        if(actualParentList.empty())
        {
          for(unsigned int i = 0; i < alreadyInserted.size(); i++)//look if a not visited from the mesh unconnected vertex exist
          {
            if(!alreadyInserted[i])
            {
              m_nextVertexIndex.push_back(i);
              alreadyInserted[i] = true;
              actualParentList.push_back(i);
              break;
            }
          }
        }

        for(std::list<unsigned int>::iterator pit = actualParentList.begin(); pit != actualParentList.end(); pit++)//find the other two neighbors of the two triangles
        {
          for(std::unordered_set<unsigned int>::iterator nit = m_vertexDataStructure[*pit].neighbors.begin(); nit != m_vertexDataStructure[*pit].neighbors.end(); nit++)//find the unconnected next sibling
          {
            if(std::find(nextParentList.begin(), nextParentList.end(), *nit) == nextParentList.end() && !alreadyInserted[*nit])
            {
              m_nextVertexIndex.push_back(*nit);
              nextParentList.push_back(*nit);
              alreadyInserted[*nit] = true;
            }
          }
        }

        actualParentList.clear();
        std::swap(nextParentList, actualParentList);
      }
    }

    void CacheIndicesGenerator::insertNewVertex(const std::vector<util::Cache>& caches, unsigned int vertexIndex0, unsigned int vertexIndex1, std::vector<unsigned int>& inoutIndices)
    {
      m_newVertexEdges.push_back(util::vec2ui(vertexIndex0, vertexIndex1));

      std::vector<unsigned int> neighborVertexIndices;

      for(std::unordered_set<unsigned int>::iterator nit0 = m_vertexDataStructure[vertexIndex0].neighbors.begin(); nit0 != m_vertexDataStructure[vertexIndex0].neighbors.end(); nit0++)//find the other two neighbors of the two triangles
      {
        std::unordered_set<unsigned int>::iterator nit1 = m_vertexDataStructure[vertexIndex1].neighbors.find(*nit0);
        if(nit1 != m_vertexDataStructure[vertexIndex1].neighbors.end())
        {
          neighborVertexIndices.push_back(*nit0);
        }
      }

      m_vertexDataStructure[vertexIndex0].neighbors.erase(m_vertexDataStructure[vertexIndex0].neighbors.find(vertexIndex1));//erase the old neighbors
      m_vertexDataStructure[vertexIndex1].neighbors.erase(m_vertexDataStructure[vertexIndex1].neighbors.find(vertexIndex0));

      util::vec3f newPosition = util::vec3f::lerp(m_vertexDataStructure[vertexIndex0].position, m_vertexDataStructure[vertexIndex1].position, 0.5f);
      util::vec3f newNormal = util::vec3f::lerp(m_vertexDataStructure[vertexIndex0].normal, m_vertexDataStructure[vertexIndex1].normal, 0.5f).normalize();

      VertexDataStructure newVertex(newPosition, newNormal, caches, m_vertexDataStructure.size(), CACHEINDEXSIZE, m_maxDistance);

      //add the new neighbors
      newVertex.neighbors.insert(vertexIndex0);
      newVertex.neighbors.insert(vertexIndex1);
      for(unsigned int i = 0; i < neighborVertexIndices.size(); i++)
      {
        newVertex.neighbors.insert(neighborVertexIndices[i]);
      }

      for(std::unordered_set<unsigned int>::iterator nit = newVertex.neighbors.begin(); nit != newVertex.neighbors.end(); nit++)
      {
        m_vertexDataStructure[*nit].neighbors.insert(newVertex.index);
      }

      m_vertexDataStructure.push_back(newVertex);
      m_nextVertexIndex.push_back(newVertex.index);

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

      //////////Insert New Triangles//////////
      //first triangle subdivision
      for(unsigned int i = 0; i < neighborVertexIndices.size(); i++)
      {
        util::vec3f e0 = m_vertexDataStructure[neighborVertexIndices[i]].position - m_vertexDataStructure[vertexIndex0].position;
        util::vec3f e1 = newVertex.position - m_vertexDataStructure[vertexIndex0].position;

        //util::vec3f e0 = m_vertexDataStructure[vertexIndex2].position - newVertex.position;
        //util::vec3f e1 = m_vertexDataStructure[vertexIndex0].position  - newVertex.position;

        if(util::vec3f::dot(util::math::cross(e0, e1), m_vertexDataStructure[vertexIndex0].normal) > 0.0f)
        {
          inoutIndices.push_back(newVertex.index);
          inoutIndices.push_back(m_vertexDataStructure[vertexIndex0].index);
          inoutIndices.push_back(m_vertexDataStructure[neighborVertexIndices[i]].index);

          inoutIndices.push_back(newVertex.index);
          inoutIndices.push_back(m_vertexDataStructure[neighborVertexIndices[i]].index);
          inoutIndices.push_back(m_vertexDataStructure[vertexIndex1].index);
        }
        else
        {
          inoutIndices.push_back(m_vertexDataStructure[neighborVertexIndices[i]].index);
          inoutIndices.push_back(m_vertexDataStructure[vertexIndex0].index);
          inoutIndices.push_back(newVertex.index);

          inoutIndices.push_back(m_vertexDataStructure[vertexIndex1].index);
          inoutIndices.push_back(m_vertexDataStructure[neighborVertexIndices[i]].index);
          inoutIndices.push_back(newVertex.index);
        }
      }

      //if(vertexIndex3 != UINT32_MAX)
      //{
      //  util::vec3f e0 = m_vertexDataStructure[vertexIndex3].position - newVertex.position;
      //  util::vec3f e1 = m_vertexDataStructure[vertexIndex0].position - newVertex.position;

      //  //second triangle subdivision
      //  if(util::vec3f::dot(util::math::cross(e0, e1), m_vertexDataStructure[vertexIndex0].normal) > 0.0f)
      //  {
      //    inoutIndices.push_back(m_vertexDataStructure[vertexIndex3].index);
      //    inoutIndices.push_back(m_vertexDataStructure[vertexIndex0].index);
      //    inoutIndices.push_back(newVertex.index);

      //    inoutIndices.push_back(m_vertexDataStructure[vertexIndex1].index);
      //    inoutIndices.push_back(m_vertexDataStructure[vertexIndex3].index);
      //    inoutIndices.push_back(newVertex.index);
      //  }
      //  else
      //  {
      //    inoutIndices.push_back(newVertex.index);
      //    inoutIndices.push_back(m_vertexDataStructure[vertexIndex0].index);
      //    inoutIndices.push_back(m_vertexDataStructure[vertexIndex3].index);

      //    inoutIndices.push_back(newVertex.index);
      //    inoutIndices.push_back(m_vertexDataStructure[vertexIndex3].index);
      //    inoutIndices.push_back(m_vertexDataStructure[vertexIndex1].index);
      //  }
      //}
    }

    void CacheIndicesGenerator::getCacheInterpolationEdges(VertexDataStructure& vertex, std::unordered_set<unsigned int>& verticesWithSameCacheIndices)//returns the indices of the vertices which have less than CACHEINDEXSIZE distinct cache-indices
    {
      unsigned int usedCacheSlots = 0;
      for(unsigned int j = 0; j < vertex.cacheIndices.size(); j++)//count the used cache-slots
      {
        if(vertex.cacheIndices[j][0] != INT32_MAX)
        {
          usedCacheSlots++;
        }
      }

      for(std::unordered_set<unsigned int>::iterator nit = vertex.neighbors.begin(); nit != vertex.neighbors.end(); nit++)
      {
        std::list<unsigned int> equalCacheIndices;
        unsigned int validCacheIndicesNumber = 0;

        for(unsigned int k = 0; k < m_vertexDataStructure[*nit].cacheIndices.size(); k++)
        {
          if(m_vertexDataStructure[*nit].cacheIndices[k][0] != INT32_MAX)
          {
            validCacheIndicesNumber++;
          }

          for(unsigned int j = 0; j < vertex.cacheIndices.size(); j++)
          {
            if(vertex.cacheIndices[j][0] == INT32_MAX)
            {
              continue;
            }

            if(m_vertexDataStructure[*nit].cacheIndices[k][0] != INT32_MAX && m_vertexDataStructure[*nit].cacheIndices[k][0] == vertex.cacheIndices[j][0] && equalCacheIndices.size() < CACHEINDEXSIZE)
            {
              equalCacheIndices.push_back(vertex.cacheIndices[j][0]);//collect equal cache-indices, but not more than we will invalidate
            }
          }
        }

        if(validCacheIndicesNumber - equalCacheIndices.size() < CACHEINDEXSIZE)//if more than CACHEINDEXSIZE indices are equal (we will only kill CACHEINDEXSIZE indices, not more) and there are not enough indices ...
        {
          if(usedCacheSlots - equalCacheIndices.size() >= CACHEINDEXSIZE)//... look if we have more cache indices than necessary
          {
            unsigned int countDown = equalCacheIndices.size() - std::max<int>(validCacheIndicesNumber - int(CACHEINDEXSIZE), 0);//guarantees that not more indices are being deleted than necessary
            for(std::list<unsigned int>::iterator it = equalCacheIndices.begin(); it != equalCacheIndices.end(); it++, countDown--)//if yes eliminate those equal cache-indices
            {
              if(countDown == 0) break;
              for(unsigned int j = 0; j < vertex.cacheIndices.size(); j++)
              {
                if(vertex.cacheIndices[j][0] == *it)
                {
                  usedCacheSlots--;
                  vertex.cacheIndices[j] = util::vec2f(INT32_MAX, 0.0f);
                  break;
                }
              }
            }
          }
          else//if not create a new vertex
          {
            //verticesWithSameCacheIndices.insert(*nit);
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

      for(unsigned int j = vertex.cacheIndices.size() - unusedCacheSlots; j < vertex.cacheIndices.size(); j++)//resort the cache-indices so that valid indices are tight together
      {
        vertex.cacheIndices[j] = util::vec2f(INT32_MAX, 0.0f);
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

      createVertexDataStructure(positions, normals, caches, indices);

      unsigned int oldVertexNumber = m_vertexDataStructure.size();

      unsigned int vertexCounter = 0;
      for(std::list<unsigned int>::iterator vit = m_nextVertexIndex.begin(); vit != m_nextVertexIndex.end(); vit++, vertexCounter++)
      {
        std::unordered_set<unsigned int> verticesWithSameCacheIndices;

        getCacheInterpolationEdges(m_vertexDataStructure[*vit], verticesWithSameCacheIndices);

        if(vertexCounter < oldVertexNumber)
        {
          for(std::unordered_set<unsigned int>::iterator it = verticesWithSameCacheIndices.begin(); it != verticesWithSameCacheIndices.end(); it++)
          {
            insertNewVertex(caches, *vit, *it, indices);
          }
        }

        eraseDoubledIndices(m_vertexDataStructure[*vit]);
      }

      cacheIndizes0.resize(m_vertexDataStructure.size());
      cacheIndizes1.resize(m_vertexDataStructure.size());

      for(unsigned int i = 0; i < m_vertexDataStructure.size(); i++)//assign the preprocessed indices to the final format
      {
        for(unsigned int j = 0; j < CACHEINDEXSIZE; j++)//convert the indices into the shader accessable format (invalid: INT32_MAX --> 0, valid: x --> x + 1)
        {
          m_vertexDataStructure[i].cacheIndices[j][0] = m_vertexDataStructure[i].cacheIndices[j][0] == INT32_MAX ? 0 : m_vertexDataStructure[i].cacheIndices[j][0] + 1;
        }

        cacheIndizes0[i] = util::cacheIndexType(m_vertexDataStructure[i].cacheIndices[0][0], m_vertexDataStructure[i].cacheIndices[1][0], m_vertexDataStructure[i].cacheIndices[2][0], m_vertexDataStructure[i].cacheIndices[3][0]);
        cacheIndizes1[i] = util::cacheIndexType(m_vertexDataStructure[i].cacheIndices[4][0], m_vertexDataStructure[i].cacheIndices[5][0], m_vertexDataStructure[i].cacheIndices[6][0], m_vertexDataStructure[i].cacheIndices[7][0]);
      }

      std::vector<unsigned int> newIndices;

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