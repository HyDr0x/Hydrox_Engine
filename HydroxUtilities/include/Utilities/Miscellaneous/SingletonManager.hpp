#ifndef SINGLETONMANAGER_H_
#define SINGLETONMANAGER_H_

#include <map>
#include <assert.h>
#include <string>

#include "Utilities/Miscellaneous/SingletonBehaviour.hpp"
#include "Utilities/Pointer/SharedPointer.h"

namespace he
{
  namespace util
  {
    class SingletonManager
    {
    public:

      SingletonManager(){}

      template<class T> void addService(SharedPointer<T> service)
      {
        m_serviceMap[std::string(typeid(service).name())] = service.dynamic_pointer_cast<SingletonBehaviour>();
      }

      template<class T> SharedPointer<T> getService()
      {
        return m_serviceMap[std::string(typeid(SharedPointer<T>).name())].dynamic_pointer_cast<T>();
      }

    private:

      SingletonManager(const SingletonManager&);
      SingletonManager& operator=(const SingletonManager&);

      std::map<std::string, SharedPointer<SingletonBehaviour>> m_serviceMap;
    };
  }
}

#endif