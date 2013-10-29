#ifndef SERVICEMANAGER_H_
#define SERVICEMANAGER_H_

#include <map>
#include <assert.h>
#include <string>

#include "Hydrox/Services/Io_service.h"

class ServiceManager
{
public:
	ServiceManager(){}
	~ServiceManager(){}

	template<class T> void addService(T* service)
	{
		m_serviceMap[std::string(typeid(service).name())] = service;
	}

	template<class T> T* getService()
	{
		return dynamic_cast<T*>(m_serviceMap[std::string(typeid(T*).name())]);
	}

private:

	ServiceManager(const ServiceManager&){}
	ServiceManager& operator=(const ServiceManager&){}

	std::map<std::string, Io_service*> m_serviceMap;
};

#endif