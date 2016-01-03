#ifndef CACHEMANAGER_H_
#define CACHEMANAGER_H_

#include <map>
#include <vector>
#include <list>
#include <algorithm>

#include <Utilities/Observer/Observer.hpp>
#include <Utilities/Miscellaneous/SingletonBehaviour.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>

#include "DataBase/ManagedResource.h"

#include "DataBase/Mesh.h"
#include "DataBase/Material.h"
#include "DataBase/ParticleEmitter.h"
#include "DataBase/Texture2D.h"
#include "DataBase/Texture3D.h"

namespace he
{
  namespace db
  {
    template<class CLASS> class ResourceManager : public util::SingletonBehaviour, public util::Observer<util::ResourceHandle&>
    {
    public:

      ResourceManager()
      {
      }

      ~ResourceManager()
      {
        for(unsigned int i = 0; i < m_objectsCache.size(); i++)
        {
          if(std::find(m_list.begin(), m_list.end(), i) == m_list.end())
          {
            m_objectsCache[i].free();
          }
        }
      }

      CLASS* getObject(util::ResourceHandle handle)
      {
        return &m_objectsCache[handle.getID()];
      }

      bool isCached(CLASS& object)
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
            unsigned int oldSize = static_cast<unsigned int>(m_objectsCache.size());
            unsigned int size = static_cast<unsigned int>(m_objectsCache.size() + m_BLOCKSIZE);
            m_objectsCache.resize(size);
            for(unsigned int i = oldSize; i < size; i++)
            {
              m_list.push_back(i);
            }
          }

          unsigned int id = *m_list.begin();
          m_list.pop_front();

          util::ResourceHandle handle(id, hash);
          handle.add(this);

          m_objectsCache[handle.getID()] = object;
          m_objectHash[hash] = handle.getID();

          return handle;
        }
        else
        {
          unsigned int id = m_objectHash[hash];

          util::ResourceHandle handle(id, hash);
          handle.add(this);

          return handle;
        }
      }

      void update(util::ResourceHandle& param) override
      {
        unsigned int erasedNumber = m_objectHash.erase(m_objectsCache[param.getID()].getHash());

        if(erasedNumber != 0)//delete it only once
        {
          m_objectsCache[param.getID()].free();

          m_list.push_front(param.getID());
        }
      }

    private:

      ResourceManager(const ResourceManager&);
      ResourceManager& operator=(const ResourceManager&);

      static const unsigned int m_BLOCKSIZE = 64;

      std::list<unsigned int> m_list;

      std::vector<CLASS> m_objectsCache;
      std::map<uint64_t, unsigned int> m_objectHash;
    };

    template class GRAPHICAPI ResourceManager<Mesh>;
    template class GRAPHICAPI ResourceManager<Material>;
    template class GRAPHICAPI ResourceManager<ParticleEmitter>;
    template class GRAPHICAPI ResourceManager<Texture2D>;
    template class GRAPHICAPI ResourceManager<Texture3D>;

    typedef ResourceManager<Mesh> ModelManager;
    typedef ResourceManager<Material> MaterialManager;
    typedef ResourceManager<ParticleEmitter> ParticleEmitterManager;
    typedef ResourceManager<Texture2D> TextureManager;
    typedef ResourceManager<Texture3D> TextureArrayManager;
  }
}

#endif
