#include "Utilities/Miscellaneous/Filecheck.h"

#include <cassert>
#include <vector>

namespace he
{
  namespace util
  {
    unsigned int getFilesLastWriteTime(CString root, std::vector<std::pair<FILETIME, std::string>>& accessTimes)
    {
      CString path = root;
      path.Append(CString(_T("/*.*")));

      WIN32_FIND_DATA fileData;
      HANDLE sh = FindFirstFile(path, &fileData);

      if(INVALID_HANDLE_VALUE == sh)
      {
        return 0;
      }

      do
      {
        CString fileName = CString(fileData.cFileName);
        
        if(fileData.dwFileAttributes != -1 && fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY &&//is it a directory?
          !(fileName.Compare(CString(_T("."))) == 0 || fileName.Compare(CString(_T(".."))) == 0))//and not root?
        {
          CString newPath = root;
          if(newPath.Right(1).Compare("/") != 0)
          {
            newPath.Append("/");
          }
          newPath.Append(fileName);

          accessTimes.push_back(std::pair<FILETIME, std::string>(fileData.ftLastWriteTime, newPath));

          getFilesLastWriteTime(newPath, accessTimes);
        }
        else if(!(fileName.Compare(CString(_T("."))) == 0 || fileName.Compare(CString(_T(".."))) == 0) &&//no root?
          !(fileData.dwFileAttributes != -1 && fileData.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY) &&//no temporary file?
          !(fileData.dwFileAttributes != -1 && fileData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) &&//no system file?
          !(fileData.dwFileAttributes != -1 && fileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))//no hidden file?
        {
          CString newPath = root;
          newPath.Append("/");
          newPath.Append(fileName);

          accessTimes.push_back(std::pair<FILETIME, std::string>(fileData.ftLastWriteTime, newPath));//must be a file!
        }

      } while(FindNextFile(sh, &fileData));

      FindClose(sh);

      return accessTimes.size();
    }

    UINT NotifyDirThread(LPVOID parameter)
    {
      HANDLE hDir = NULL;
      Filecheck* filechecker = (Filecheck*)parameter;

      std::vector<std::pair<FILETIME, std::string>> newAccessTimes, oldAccessTimes;

      assert(filechecker != nullptr);

      hDir = FindFirstChangeNotification(filechecker->getDirectoryPath().c_str(), TRUE, FILE_NOTIFY_CHANGE_LAST_WRITE);

      assert(hDir != INVALID_HANDLE_VALUE);

      unsigned int oldFileNumber = getFilesLastWriteTime(filechecker->getDirectoryPath().c_str(), oldAccessTimes);//initialization

      while(filechecker->isRunning())
      {
        while(WaitForSingleObject(hDir, WAIT_TIMEOUT) != WAIT_OBJECT_0)
        {
          if(!filechecker->isRunning())
          {
            return 0;
          }
        }

        newAccessTimes.clear();
        unsigned int newFileNumber = getFilesLastWriteTime(filechecker->getDirectoryPath().c_str(), newAccessTimes);

        if(newFileNumber == oldFileNumber)
        {
          for(unsigned int i = 0; i < oldAccessTimes.size(); i++)
          {
            if(oldAccessTimes[i].first.dwHighDateTime != newAccessTimes[i].first.dwHighDateTime || oldAccessTimes[i].first.dwLowDateTime != newAccessTimes[i].first.dwLowDateTime)
            {
              filechecker->setChangedFilepath(newAccessTimes[i].second);
              break;
            }
          }
        }
        
        oldAccessTimes = newAccessTimes;

        assert(FindNextChangeNotification(hDir) != 0);
      }

      return FindCloseChangeNotification(hDir);
    }

    Filecheck::Filecheck()
    {
    }

    Filecheck::~Filecheck()
    {
      stopRunning();

      WaitForSingleObject(m_thread->m_hThread, INFINITE);
      m_thread = NULL;
    }

    void Filecheck::initialize(std::string directoryPath)
    {
      m_directoryPath = directoryPath;
      m_isFileChanged = false;

      m_thread = AfxBeginThread(NotifyDirThread, this);

      assert(m_thread != nullptr);

      m_isRunning = true;
    }

    std::string Filecheck::getDirectoryPath() const
    {
      return m_directoryPath;
    }

    void Filecheck::setDirectoryPath(std::string directoryPath)
    {
      m_directoryPath = directoryPath;
    }

    bool Filecheck::isRunning() const
    {
      return m_isRunning;
    }

    void Filecheck::stopRunning()
    {
      m_isRunning = false;
    }

    bool Filecheck::isFileChanged() const
    {
      return m_isFileChanged;
    }

    std::string Filecheck::getChangedFilepath() const
    {
      return m_changedFilepath;
    }

    void Filecheck::setChangedFilepath(std::string changedFilepath)
    {
      m_isFileChanged = true;
      m_changedFilepath = changedFilepath;
    }

    void Filecheck::resetFilecheckerStatus()
    {
      m_isFileChanged = false;
      m_changedFilepath.clear();
    }
  }
}
