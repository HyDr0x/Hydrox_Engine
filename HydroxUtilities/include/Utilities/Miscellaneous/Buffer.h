#ifndef BUFFER_H_
#define BUFFER_H_

#include <vector>

#include "Utilities/DLLExport.h"

namespace he
{
	namespace util
	{
    class GRAPHICAPI Buffer
    {
    public:

      Buffer(unsigned int size);
      Buffer(const Buffer& obj);
      ~Buffer();

      void AddData(unsigned int size, unsigned char *data);
      void ChangeData(unsigned int offset, unsigned int size, unsigned char *data);
      void DeleteData(unsigned int offset);

      bool IsChanged() const;
      bool IsCorrupted() const;

    private:

      Buffer(){}

      std::vector<unsigned char> m_data;//the actual data from the buffer
      std::vector<unsigned int> m_offsets;//offsets to the next element
      std::vector<unsigned int> m_freeDataOffsets;//offsets where free space is available
      std::vector<unsigned int> m_freeDataSizes;//sizes of the free space which is available

      bool m_changed;
      bool m_corrupted;//if the buffer data is out of integrity e.g. you delete something between two offsets
    };
	}
}

#endif
