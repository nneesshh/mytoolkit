#pragma once

#include <stdio.h>
#include <string>

#include "../platform/types.h"

#include "IRandomAccessFile.h"

class CRaFile : public IRandomAccessFile {

public:
	enum OPEN_MODE {
		modeCreate = 0x1,
		modeRead = 0x2,
		modeReadWrite = 0x4,
		modeWrite = 0x8,
		modeNoTruncate = 0x10
	};

	CRaFile(const char *filename, int mode, bool bin)
		: m_iswritable(false)
		, m_file(nullptr) {
			Open(filename, mode, bin);
	}

	CRaFile(FILE* file)
		: m_iswritable(false)
		, m_file(nullptr) {
			m_file = file;
	}

	CRaFile()
		: m_iswritable(false)
		, m_file(nullptr) {

	}

	~CRaFile() {
		if (nullptr != m_file)
			fclose(m_file);
	}

	int MYAPI_CALL Read(void* buffer, int size) {
		return fread(buffer, 1, size, m_file);
	}

	int MYAPI_CALL Write(const void* buffer, int size) {
		if (!m_file)
			return -1;
		return fwrite(buffer, 1, size, m_file);
	}

	bool MYAPI_CALL Seek(int position, SeekMode mode) {
		int m;
		switch (mode) {
		case BEGIN:   m = SEEK_SET; break;
		case CURRENT: m = SEEK_CUR; break;
		case END:     m = SEEK_END; break;
		default: return false;
		}

		return (fseek(m_file, position, m) == 0);
	}

	int MYAPI_CALL Tell() {
		return ftell(m_file);
	}

	int MYAPI_CALL Length() {
		int32_t len = 0, cur = Pos();
		Seek(0, END);
		len = Pos();
		Seek(cur, BEGIN);
		return len;
	}

	int MYAPI_CALL Pos() {
		return (nullptr == m_file) ? -1 : Tell();
	}

	int MYAPI_CALL Len() {
		return (nullptr == m_file) ? 0 : Length();
	}

	int MYAPI_CALL IsEOF() {
		return (nullptr == m_file) ? true : ((Pos() >= Len()) ? true : false);
		//int nEof = feof(m_file);
		//return (feof(m_file) != 0);
	}

public:
	FILE				*m_file;

	bool				m_iswritable;
	std::string			m_pathname;

public:
	bool MYAPI_CALL	Open( const char *filename, int mode, bool bin );

	void MYAPI_CALL Close() {
		if (!m_file)
			return;

		if ( m_iswritable )
			Flush();

		fclose(m_file);
	}

	void MYAPI_CALL Flush()	{
		if (!m_file)
			return;

		fflush(m_file);
	}

public:
	const char *	PathName() { return m_pathname.c_str(); }

	inline std::string		ReadLine();
	inline std::string		ReadLString();
	inline char				ReadTChar();
	inline uint8_t			ReadByte();
	inline int16_t			ReadInt16();
	inline uint16_t			ReadInt16U();
	inline int32_t			ReadInt32();
	inline uint32_t			ReadInt32U();
	inline float			ReadFloat();

	inline int32_t			WriteLine(const char *s);
	inline int32_t			WriteLString(const char *s);
	inline int32_t			WriteTChar(char c);
	inline int32_t			WriteByte(uint8_t c);
	inline int32_t			WriteInt16U(uint16_t s);
	inline int32_t			WriteInt16(int16_t s);
	inline int32_t			WriteInt32U(uint32_t nInt);
	inline int32_t			WriteInt32(int32_t nInt);
	inline int32_t			WriteFloat(float f);
};

int32_t
CRaFile::WriteLine(const char *s) {
	int32_t	len = strlen(s);
	Write((uint8_t*)s, strlen(s));
	WriteByte((uint8_t)'\n');
	return len+1;
}

int32_t
CRaFile::WriteLString(const char *s) {
	int32_t leading = strlen(s);
	int32_t	len = WriteInt16(leading);
	len += Write((uint8_t*)s, leading);
	return len;
}

int32_t
CRaFile::WriteInt16U(uint16_t s) {
	return Write(&s,sizeof(uint16_t));
}

int32_t
CRaFile::WriteInt32U(uint32_t s) {
	return Write(&s,sizeof(uint32_t));
}

int32_t
CRaFile::WriteInt16(int16_t s) {
	return Write(&s,sizeof(int16_t));
}

int32_t
CRaFile::WriteInt32(int32_t nInt) {
	return Write(&nInt,sizeof(int32_t));
}

int32_t
CRaFile::WriteFloat(float f) {
	return Write(&f,sizeof(float));
}

int32_t
CRaFile::WriteTChar(char c) {
	return Write(&c,sizeof(c));
}

int32_t
CRaFile::WriteByte(uint8_t c) {
	return Write(&c,sizeof(uint8_t));
}

std::string
CRaFile::ReadLine() {
	char chLine[1024];
	if (nullptr == fgets(chLine, sizeof(chLine), m_file)) {
		chLine[0] = '\0';
	}
	else
	{
		// remove tail '\r' and '\n'
		int nIndex = strlen(chLine);
		while(--nIndex>0)
		{
			if ('\r'!=chLine[nIndex] && '\n'!=chLine[nIndex])
			{
				break;
			}
			chLine[nIndex] = '\0';
		}

		//
		if (nIndex <= 0)
		{
			chLine[0] = '\0';
		}
	}
	return chLine;

	// "SAMPLE CODE"
	//  char chLine[1024];
	// 	FILE *fp = fopen(sFile, "r");
	// 	if ( fp )
	// 	{
	// 		while ( nullptr != fgets(chLine, sizeof(chLine), fp) )
	// 		{
	//			// remove tail '\r' and '\n'
	//			int nIndex = strlen(chLine);
	//			while(--nIndex>0)
	//			{
	//				if ('\r'!=chLine[nIndex] && '\n'!=chLine[nIndex])
	//				{
	//					break;
	//				}
	//				chLine[nIndex] = '\0';
	//			}	
	//
	//			//
	//			if (nIndex > 0)
	//			{
	// 				AddWord(chLine);
	// 			}
	//		}
	//	}		
	// 	fclose(fp);

	// 	int32_t head = Pos()
	// 		, len = -1
	// 		, tail = 0;
	// 
	// 	while ( !IsEOF() ) {
	// 		char c = ReadTChar();
	// 
	// 		if ( '\r' == c ) {
	// 			if ( '\n' == ReadTChar() ) {
	// 				tail = 2;
	// 				len = Pos() - head - tail;
	// 				break;
	// 			}
	// 		}
	// 
	// 		if ( '\n'== c ) {
	// 			tail	=	1;
	// 			len = Pos() - head - tail;
	// 			break;
	// 		}
	// 	}
	// 
	// 	if ( IsEOF() && len == -1 ) {
	// 		len = Pos() - head;
	// 	}
	// 
	// 	Seek(head, BEGIN);
	// 
	// 	char *pData = new char[len+1];
	// 	Read(pData,len);
	// 	pData[len] = 0;
	// 	std::string	s(pData);
	// 	delete [] pData;
	// 
	// 	if ( tail ) {
	// 		Seek(tail, CURRENT);
	// 	}
	//
	//	return	s;
}

std::string
CRaFile::ReadLString() {
	int32_t leading = ReadInt16();
	char *pBuffer = new char[leading+1];
	Read(pBuffer, leading);
	pBuffer[leading] = 0;
	std::string	s(pBuffer);
	delete [] pBuffer;
	return	s;
}

int16_t
CRaFile::ReadInt16() {
	int16_t s = 0;
	Read(&s,sizeof(int16_t));
	return s;
}

uint16_t
CRaFile::ReadInt16U() {
	uint16_t s = 0;
	Read(&s,sizeof(uint16_t));
	return s;
}

int32_t
CRaFile::ReadInt32() {
	int32_t nInt = 0;
	Read(&nInt,sizeof(int32_t));
	return nInt;
}

uint32_t
CRaFile::ReadInt32U() {
	uint32_t nInt = 0;
	Read(&nInt,sizeof(uint32_t));
	return nInt;
}

float
CRaFile::ReadFloat() {
	float f = 0;
	Read(&f,sizeof(float));
	return f;
}

uint8_t
CRaFile::ReadByte() {
	uint8_t b = 0;
	Read(&b,sizeof(uint8_t));
	return b;
}

char
CRaFile::ReadTChar() {
	char c = 0;
	int ret = Read(&c,sizeof(c));
	return c;
}

/*EOF*/