#include "filestream.h"

#include "../physfs/physfs.h"

FileStream::FileStream(const std::string& name, PHYSFS_File *fileHandle, bool writeable) :
    _name(name),
    _fileHandle(fileHandle),
    _pos(0),
    _writeable(writeable),
    _caching(false) {
}

FileStream::FileStream(const std::string& name, const std::string& buffer) :
    _name(name),
    _fileHandle(nullptr),
    _pos(0),
    _writeable(false),
    _caching(true) {
    _data.resize(buffer.length());
    memcpy(&_data[0], &buffer[0], buffer.length());
}

FileStream::~FileStream() {
	close();
}

void FileStream::cache() {
    _caching = true;

    if(!_writeable) {
        if(!_fileHandle)
            return;

        // cache entire file into data buffer
        _pos = PHYSFS_tell(_fileHandle);
        PHYSFS_seek(_fileHandle, 0);
        int size = PHYSFS_fileLength(_fileHandle);
        _data.resize(size);
        if(PHYSFS_read(_fileHandle, _data.data(), size, 1) == -1)
            throwError("unable to read file data", true);
        PHYSFS_close(_fileHandle);
        _fileHandle = nullptr;
    }
}

void FileStream::close() {
    if(_fileHandle && PHYSFS_isInit()) {
        if(!PHYSFS_close(_fileHandle))
            throwError("close failed", true);
        _fileHandle = nullptr;
    }

    _data.clear();
    _pos = 0;
}

void FileStream::flush() {
    if(!_writeable)
        throwError("filestream is not writeable");

    if(_fileHandle) {
        if(_caching) {
            if(!PHYSFS_seek(_fileHandle, 0))
                throwError("flush seek failed", true);
            uint len = _data.size();
            if(PHYSFS_write(_fileHandle, _data.data(), 1, len) != len)
                throwError("flush write failed", true);
        }

        if(PHYSFS_flush(_fileHandle) == 0)
            throwError("flush failed", true);
    }
}

int FileStream::read(void *buffer, uint32 size, uint32 nmemb) {
    if(!_caching) {
        int res = PHYSFS_read(_fileHandle, buffer, size, nmemb);
        if(res == -1)
            throwError("read failed", true);
        return res;
    } else {
        int writePos = 0;
        uint8 *outBuffer = (uint8*)buffer;
        for(uint i=0;i<nmemb;++i) {
            if(_pos+size > _data.size())
                return i;

            for(uint j=0;j<size;++j)
                outBuffer[writePos++] = _data[_pos++];
        }
        return nmemb;
    }
}

void FileStream::write(const void *buffer, uint32 count) {
    if(!_caching) {
        if(PHYSFS_write(_fileHandle, buffer, 1, count) != count)
            throwError("write failed", true);
    } else {
        _data.grow(_pos + count);
        memcpy(&_data[_pos], buffer, count);
        _pos += count;
    }
}

void FileStream::seek(uint32 pos) {
    if(!_caching) {
        if(!PHYSFS_seek(_fileHandle, pos))
            throwError("seek failed", true);
    } else {
        if(pos > _data.size())
            throwError("seek failed");
        _pos = pos;
    }
}

void FileStream::skip(uint len) {
    seek(tell() + len);
}

uint FileStream::size() {
    if(!_caching)
        return PHYSFS_fileLength(_fileHandle);
    else
        return _data.size();
}

uint FileStream::tell() {
    if(!_caching)
        return PHYSFS_tell(_fileHandle);
    else
        return _pos;
}

bool FileStream::eof() {
    if(!_caching)
        return PHYSFS_eof(_fileHandle);
    else
        return _pos >= _data.size();
}

uint8 FileStream::getU8() {
    uint8 v = 0;
    if(!_caching) {
        if(PHYSFS_read(_fileHandle, &v, 1, 1) != 1)
            throwError("read failed", true);
    } else {
        if(_pos+1 > _data.size())
            throwError("read failed");

        v = _data[_pos];
        _pos += 1;
    }
    return v;
}

uint16 FileStream::getU16() {
    uint16 v = 0;
    if(!_caching) {
        if(PHYSFS_readULE16(_fileHandle, &v) == 0)
            throwError("read failed", true);
    } else {
        if(_pos+2 > _data.size())
            throwError("read failed");

        v = stdext::readULE16(&_data[_pos]);
        _pos += 2;
    }
    return v;
}

uint32 FileStream::getU32() {
    uint32 v = 0;
    if(!_caching) {
        if(PHYSFS_readULE32(_fileHandle, &v) == 0)
            throwError("read failed", true);
    } else {
        if(_pos+4 > _data.size())
            throwError("read failed");

        v = stdext::readULE32(&_data[_pos]);
        _pos += 4;
    }
    return v;
}

uint64 FileStream::getU64() {
    uint64 v = 0;
    if(!_caching) {
        if(PHYSFS_readULE64(_fileHandle, (PHYSFS_uint64*)&v) == 0)
            throwError("read failed", true);
    } else {
        if(_pos+8 > _data.size())
            throwError("read failed");
        v = stdext::readULE64(&_data[_pos]);
        _pos += 8;
    }
    return v;
}

int8 FileStream::get8() {
    int8 v = 0;
    if(!_caching) {
        if(PHYSFS_read(_fileHandle, &v, 1, 1) != 1)
            throwError("read failed", true);
    } else {
        if(_pos+1 > _data.size())
            throwError("read failed");

        v = _data[_pos];
        _pos += 1;
    }
    return v;
}

int16 FileStream::get16() {
    int16 v = 0;
    if(!_caching) {
        if(PHYSFS_readSLE16(_fileHandle, &v) == 0)
            throwError("read failed", true);
    } else {
        if(_pos+2 > _data.size())
            throwError("read failed");

        v = stdext::readSLE16(&_data[_pos]);
        _pos += 2;
    }
    return v;
}

int32 FileStream::get32() {
    int32 v = 0;
    if(!_caching) {
        if(PHYSFS_readSLE32(_fileHandle, &v) == 0)
            throwError("read failed", true);
    } else {
        if(_pos+4 > _data.size())
            throwError("read failed");

        v = stdext::readSLE32(&_data[_pos]);
        _pos += 4;
    }
    return v;
}

int64 FileStream::get64() {
    int64 v = 0;
    if(!_caching) {
        if(PHYSFS_readSLE64(_fileHandle, (PHYSFS_sint64*)&v) == 0)
            throwError("read failed", true);
    } else {
        if(_pos+8 > _data.size())
            throwError("read failed");
        v = stdext::readSLE64(&_data[_pos]);
        _pos += 8;
    }
    return v;
}

std::string FileStream::getString() {
    std::string str;
    uint16 len = getU16();
    if(len > 0 && len < 8192) {
        char buffer[8192];
        if(_fileHandle) {
            if(PHYSFS_read(_fileHandle, buffer, 1, len) == 0)
                throwError("read failed", true);
            else
                str = std::string(buffer, len);
        } else {
            if(_pos+len > _data.size()) {
                throwError("read failed");
                return 0;
            }

            str = std::string((char*)&_data[_pos], len);
            _pos += len;
        }
    } else if(len != 0)
        throwError("read failed because string is too big");
    return str;
}

void FileStream::addU8(uint8 v) {
    if(!_caching) {
        if(PHYSFS_write(_fileHandle, &v, 1, 1) != 1)
            throwError("write failed", true);
    } else {
        _data.add(v);
        _pos++;
    }
}

void FileStream::addU16(uint16 v) {
    if(!_caching) {
        if(PHYSFS_writeULE16(_fileHandle, v) == 0)
            throwError("write failed", true);
    } else {
        _data.grow(_pos + 2);
        stdext::writeULE16(&_data[_pos], v);
        _pos += 2;
    }
}

void FileStream::addU32(uint32 v) {
    if(!_caching) {
        if(PHYSFS_writeULE32(_fileHandle, v) == 0)
            throwError("write failed", true);
    } else {
        _data.grow(_pos + 4);
        stdext::writeULE32(&_data[_pos], v);
        _pos += 4;
    }
}

void FileStream::addU64(uint64 v) {
    if(!_caching) {
        if(PHYSFS_writeULE64(_fileHandle, v) == 0)
            throwError("write failed", true);
    } else {
        _data.grow(_pos + 8);
        stdext::writeULE64(&_data[_pos], v);
        _pos += 8;
    }
}

void FileStream::add8(int8 v) {
    if(!_caching) {
        if(PHYSFS_write(_fileHandle, &v, 1, 1) != 1)
            throwError("write failed", true);
    } else {
        _data.add(v);
        _pos++;
    }
}

void FileStream::add16(int16 v) {
    if(!_caching) {
        if(PHYSFS_writeSLE16(_fileHandle, v) == 0)
            throwError("write failed", true);
    } else {
        _data.grow(_pos + 2);
        stdext::writeSLE16(&_data[_pos], v);
        _pos += 2;
    }
}

void FileStream::add32(int32 v) {
    if(!_caching) {
        if(PHYSFS_writeSLE32(_fileHandle, v) == 0)
            throwError("write failed", true);
    } else {
        _data.grow(_pos + 4);
        stdext::writeSLE32(&_data[_pos], v);
        _pos += 4;
    }
}

void FileStream::add64(int64 v) {
    if(!_caching) {
        if(PHYSFS_writeSLE64(_fileHandle, v) == 0)
            throwError("write failed", true);
    } else {
        _data.grow(_pos + 8);
        stdext::writeSLE64(&_data[_pos], v);
        _pos += 8;
    }
}

void FileStream::addString(const std::string& v) {
    addU16(v.length());
    write(v.c_str(), v.length());
}

void FileStream::throwError(const std::string& message, bool physfsError) {
    std::string completeMessage = stdext::format("in file '%s': %s", _name, message);
    if(physfsError)
        completeMessage += std::string(": ") + PHYSFS_getLastError();
    stdext::throw_exception(completeMessage);
}