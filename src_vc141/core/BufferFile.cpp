//------------------------------------------------------------------------------
//  BufferFile.cpp
//  (C) 2016 n.lee
//------------------------------------------------------------------------------
#include "bufferfile.h"
#include <memory>

#define STD_MIN(a, b) (a < b ? a : b)

IRandomAccessFile * AuCreateBufferFile(int capacity)
{
	if (capacity < 0)
	{
		return 0;
	}

	return new CBufferFile(capacity);
}


static int getNextPowerOfTwo(int value)
{
	int n=1;
	while (n<value)
	{
		n*=2;
	}
	return n;
}

CBufferFile::CBufferFile(int capacity)
{
	m_capacity = getNextPowerOfTwo(capacity);
	m_buffer = new u8[m_capacity];
	m_size = 0;
	m_position = 0;
	m_nref = 0;
}

CBufferFile::CBufferFile(unsigned char *buf, size_t len, bool ref)
{
	m_buffer = buf;
	m_capacity = len;
	m_size = len;
	m_position = 0;
	m_nref = (int)ref;
}

CBufferFile::~CBufferFile()
{
	if (0>=m_nref)
	{
		delete[] m_buffer;
	}
}

int MYAPI_CALL CBufferFile::Read(void* buffer, int size)
{
	int real_read = STD_MIN(int(m_size - m_position), size);
	memcpy(buffer, m_buffer + m_position, real_read);
	m_position += real_read;
	return real_read;
}

int MYAPI_CALL CBufferFile::Write(const void* buffer, int size)
{
	ensureSize(m_position + size);
	memcpy(m_buffer + m_position, buffer, size);
	m_position += size;
	return size;
}

bool MYAPI_CALL CBufferFile::Seek(int position, SeekMode mode)
{
	size_t real_pos;
	switch (mode)
	{
	case BEGIN:   real_pos = position;              break;
	case CURRENT: real_pos = m_position + position; break;
	case END:     real_pos = m_size + position;     break;
	default:      return false;
	}

	if (real_pos < 0 || real_pos > m_size)
	{
		m_position = 0;
		return false;
	}
	else
	{
		m_position = (int)real_pos;
		return true;
	}
}

int MYAPI_CALL CBufferFile::Tell()
{
	return m_position;
}

void CBufferFile::ensureSize(size_t min_size)
{
	bool realloc_needed = false;
	while (m_capacity < min_size)
	{
		m_capacity *= 2;
		realloc_needed = true;
	}

	if (realloc_needed)
	{
		u8* new_buffer = new u8[m_capacity];
		memcpy(new_buffer, m_buffer, m_size);
		delete[] m_buffer;
		m_buffer = new_buffer;
	}

	m_size = min_size;
}

/** -- EOF -- **/
