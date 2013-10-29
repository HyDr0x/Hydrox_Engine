#include "Hydrox/Utility/Miscellaneous/Buffer.h"


Buffer::Buffer(unsigned int size) : m_changed(false),
                                    m_corrupted(false)
{
  m_data.resize(size);
}

Buffer::Buffer(const Buffer& obj)
{
  m_changed = obj.m_changed;
  m_corrupted = obj.m_corrupted;
  m_data = obj.m_data;
  m_offsets = obj.m_offsets;
  m_freeDataOffsets = obj.m_freeDataOffsets;
  m_freeDataSizes = obj.m_freeDataSizes;
}

Buffer::~Buffer()
{
  m_data.clear();
}

void Buffer::AddData(unsigned int size, unsigned char *data)
{
  for(unsigned int i = 0; i != size; i++)
  {
    m_data.push_back(data[i]);
  }
  m_offsets.push_back(*m_offsets.end() + size);
}

void Buffer::ChangeData(unsigned int offset, unsigned int size, unsigned char *data)
{
  for(unsigned int i = 0; i != size; i++)
  {
    m_data[offset + i] = data[i];
  }
}

void Buffer::DeleteData(unsigned int offset)//TODO reallocate after certain memory fragmenting
{
  bool tmpCorrupted = true;
  for(unsigned int i = 0; i != m_offsets.size(); i++)
  {
    if(offset == m_offsets[i])
    {
      tmpCorrupted = false;
    }
  }

  m_corrupted = tmpCorrupted;
  m_freeDataOffsets.push_back(offset);
}

bool Buffer::IsChanged() const
{
  return m_changed;
}

bool Buffer::IsCorrupted() const
{
  return m_corrupted;
}

