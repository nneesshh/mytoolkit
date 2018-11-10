#ifndef FILESTREAM_H
#define FILESTREAM_H

#include "declarations.h"

struct PHYSFS_File;

// @bindclass
class FileStream : public stdext::shared_object {
public:
	FileStream(const std::string& name, PHYSFS_File *fileHandle, bool writeable);
	FileStream(const std::string& name, const std::string& buffer);
	~FileStream();

    void cache();
    void close();
    void flush();
    void write(const void *buffer, uint count);
    int read(void *buffer, uint size, uint nmemb = 1);
    void seek(uint pos);
    void skip(uint len);
    uint size();
    uint tell();
    bool eof();
    std::string name() { return _name; }

    uint8 getU8();
    uint16 getU16();
    uint32 getU32();
    uint64 getU64();
    int8 get8();
    int16 get16();
    int32 get32();
    int64 get64();
    std::string getString();

	void addU8(uint8 v);
    void addU16(uint16 v);
    void addU32(uint32 v);
    void addU64(uint64 v);
    void add8(int8 v);
    void add16(int16 v);
    void add32(int32 v);
    void add64(int64 v);
    void addString(const std::string& v);
    void addPos(uint16 x, uint16 y, uint8 z) { addU16(x); addU16(y); addU8(z); }
	void addPoint(uint8 x, uint8 y) { addU8(x); addU8(y); }

	FileStreamPtr asFileStream() { return static_self_cast<FileStream>(); }

private:
    void checkWrite();
    void throwError(const std::string& message, bool physfsError = false);

    std::string _name;
    PHYSFS_File * _fileHandle;
    uint _pos;
    bool _writeable;
    bool _caching;

    DataBuffer<uint8_t> _data;
};

#endif