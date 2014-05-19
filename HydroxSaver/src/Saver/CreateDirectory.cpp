#ifndef CREATEDIRECTORY_H_
#define CREATEDIRECTORY_H_

#include <string>

#ifdef _WIN32

#include <Windows.h>

bool createDirectory(std::string pathName)
{
  return CreateDirectory(pathName.c_str(), nullptr);
}
#else
#include <dirent.h> 
#include <sys/stat.h> 

bool createDirectory(std::string pathName)
{
  return mkdir(pathName.c_str(), 777);
}
#endif

#endif