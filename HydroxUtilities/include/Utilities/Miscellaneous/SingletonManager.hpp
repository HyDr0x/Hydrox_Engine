#ifndef SINGLETONMANAGER_H_
#define SINGLETONMANAGER_H_

#include <map>
#include <assert.h>
#include <string>

#include "Utilities/Miscellaneous/SingletonBehaviour.hpp"

namespace he
{
	namespace util
	{
    class SingletonManager
    {
    public:
	    SingletonManager(){}
	    ~SingletonManager(){}

	    template<class T> void addService(T* service)
	    {
		    m_serviceMap[std::string(typeid(service).name())] = service;
	    }

	    template<class T> T* getService()
	    {
		    return dynamic_cast<T*>(m_serviceMap[std::string(typeid(T*).name())]);
	    }

    private:

	    SingletonManager(const SingletonManager&){}
	    SingletonManager& operator=(const SingletonManager&){}

	    std::map<std::string, SingletonBehaviour*> m_serviceMap;
    };
  }
}

#endif