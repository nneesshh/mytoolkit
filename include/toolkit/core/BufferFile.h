#pragma once
//------------------------------------------------------------------------------
/**
    @class CBufferFile
    
    (C) 2016 n.lee
*/
#include "../platform/types.h"

#include "IRandomAccessFile.h"

//------------------------------------------------------------------------------
/**
	
*/
class CBufferFile : public IRandomAccessFile
{
public:
  CBufferFile(int capacity);
  CBufferFile(unsigned char *buf, size_t len, bool ref=true);
  ~CBufferFile();

  int  MYAPI_CALL Read(void* buffer, int size);
  int  MYAPI_CALL Write(const void* buffer, int size);
  bool MYAPI_CALL Seek(int position, SeekMode mode);
  int  MYAPI_CALL Tell();

private:
  void ensureSize(size_t min_size);

public:
  u8* m_buffer;
  int m_position;
  size_t m_size;

  /// The actual size of m_buffer.  Always a power of two.
  size_t m_capacity;
  int m_nref;
};

/*EOF*/