#ifndef FILECHECK_H_
#define FILECHECK_H_

#include <string>
#include <vector>

#define _AFXDLL

#include <afxwin.h>

#include "Utilities/DLLExport.h"

namespace he
{
  namespace util
  {
    class GRAPHICAPI Filecheck
    {
    public:

      Filecheck();
      ~Filecheck();

      void initialize(std::string directoryPath);

      std::string getDirectoryPath() const;
      void setDirectoryPath(std::string directoryPath);

      bool isRunning() const;
      void stopRunning();

      bool isFileChanged() const;

      std::string getChangedFilepath() const;
      void setChangedFilepath(std::string changedFilepath);

      void resetFilecheckerStatus();

    private:

      bool m_isRunning;
      bool m_isFileChanged;
      CWinThread* m_thread;
      std::string m_directoryPath;
      std::string m_changedFilepath;
    };
  }
}

#endif