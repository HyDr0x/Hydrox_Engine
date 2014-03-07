#ifndef CACHEMANAGER_H_
#define CACHEMANAGER_H_

#include <map>
#include <vector>
#include <list>

#include <Utilities/Observer/Observer.hpp>
#include <Utilities/Miscellaneous/SingletonBehaviour.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>

#include "Renderer/Resources/ManagedResource.h"

#include "Renderer/Resources/Mesh.h"
#include "Renderer/Resources/Material.h"
#include "Renderer/Resources/RenderShader.h"
#include "Renderer/Resources/ComputeShader.h"
#include "Renderer/Resources/Texture.h"

namespace he
{
	namespace renderer
	{
    template<class CLASS> class ResourceManager : public util::SingletonBehaviour, public util::Observer<util::ResourceHandle*>
    {
    public:

      ResourceManager()
      {
      }

	    ~ResourceManager()
      {
        std::list<std::vector<unsigned int>>::iterator referenceIterator = m_referenceCounterList.begin();
        std::vector<unsigned int>& currentBlockReferences = *referenceIterator;
        for(unsigned int i = 0; i < m_objectsCache.size(); i++)
        {
          if(i % m_BLOCKSIZE == 0)
          {
            currentBlockReferences = *referenceIterator;
            referenceIterator++;
          }

          if(currentBlockReferences[i % m_BLOCKSIZE] > 0)
          {
            std::cout << "WARNING, Resource not being cleaned correctly!" << std::endl;
          }

          m_objectsCache[i].free();
        }
      }

      CLASS* getObject(util::ResourceHandle handle)
      {
        return dynamic_cast<CLASS*>(&m_objectsCache[handle.getID()]);
      }

      bool isAlreadyCached(CLASS& object)
      {
        return m_objectHash.count(object.getHash());
      }

      util::ResourceHandle addObject(CLASS& object)
      {
        uint64_t hash = object.getHash();

        if(!m_objectHash.count(hash))
        {
          if(m_list.empty())
          {
            unsigned int oldSize = m_objectsCache.size();
            unsigned int size = static_cast<unsigned int>(m_objectsCache.size() + m_BLOCKSIZE);
            m_objectsCache.resize(size);
            m_referenceCounterList.push_back(std::vector<unsigned int>(size, 0));
            for(unsigned int i = oldSize; i < size; i++)
            {
              m_list.push_back(i);
            }
          }

          unsigned int id = *m_list.begin();
          m_list.pop_front();

          unsigned int currentReferenceBlock = id / m_BLOCKSIZE;
          std::list<std::vector<unsigned int>>::iterator referenceIterator = m_referenceCounterList.begin();
          for(unsigned int i = 0; i < currentReferenceBlock; i++)
          {
            referenceIterator++;
          }

          std::vector<unsigned int>& referenceCounter = *referenceIterator;
          util::ResourceHandle handle(id, &referenceCounter[id % m_BLOCKSIZE]);
          handle.add(this);

          m_objectsCache[handle.getID()] = object;
          m_objectHash[hash] = handle.getID();

          return handle;
        }
        else
        {
          unsigned int id = m_objectHash[hash];

          unsigned int currentReferenceBlock = id / m_BLOCKSIZE;
          std::list<std::vector<unsigned int>>::iterator referenceIterator = m_referenceCounterList.begin();
          for(unsigned int i = 0; i < currentReferenceBlock; i++)
          {
            referenceIterator++;
          }

          std::vector<unsigned int>& referenceCounter = *referenceIterator;

          util::ResourceHandle handle(id, &referenceCounter[id % m_BLOCKSIZE]);
          handle.add(this);

          return handle;
        }
      }

      void updateObserver(util::ResourceHandle* param)
      {
        uint64_t hash = m_objectsCache[param->getID()].getHash();
        m_objectHash.erase(m_objectsCache[param->getID()].getHash());
        m_objectsCache[param->getID()].free();

        m_list.push_front(param->getID());
      }

    private:

	    ResourceManager(const ResourceManager&){}

      static const unsigned int m_BLOCKSIZE = 64;

	    std::list<unsigned int> m_list;

	    std::vector<CLASS> m_objectsCache;
      std::list<std::vector<unsigned int>> m_referenceCounterList;
      std::map<uint64_t, unsigned int> m_objectHash;
    };

    template class GRAPHICAPI ResourceManager<Mesh>;
    template class GRAPHICAPI ResourceManager<Material>;
    template class GRAPHICAPI ResourceManager<RenderShader>;
    template class GRAPHICAPI ResourceManager<ComputeShader>;
    template class GRAPHICAPI ResourceManager<Texture>;

    typedef ResourceManager<Mesh> ModelManager;
    typedef ResourceManager<Material> MaterialManager;
    typedef ResourceManager<RenderShader> RenderShaderManager;
    typedef ResourceManager<ComputeShader> ComputeShaderManager;
    typedef ResourceManager<Texture> TextureManager;
	}
}

#endif
