#ifndef ACONTAINER_H_
#define ACONTAINER_H_

#include <vector>

#include <Utilities/Miscellaneous/MurMurHash3.h>

#include "XBar/DLLExport.h"

namespace he
{
	namespace xBar
	{
    class GRAPHICAPI AContainer
    {
    public:

      AContainer() : m_hash(0)
      {
      }

      uint64_t getHash()
      {
        return m_hash;
      }

    protected:

      virtual void createHash() = 0;

      uint64_t m_hash;
    };
  }
}

#endif