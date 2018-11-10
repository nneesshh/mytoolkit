#include "bytestream.h"

ByteStream::ByteStream(const Uint8ArrayPtr& data) :
    _data(data),
    _offset(0) {
}

ByteStream::~ByteStream() {

}

uint8 ByteStream::getU8() {
    uint8 v = 0;
    if(_offset+1 > _data->size())
        stdext::throw_exception("read failed");

    v = _data->at(_offset);
    _offset += 1;
    return v;
}

uint16 ByteStream::getU16() {
    uint16 v = 0;
    if(_offset+2 > _data->size())
        stdext::throw_exception("read failed");

    v = stdext::readULE16(&_data->at(_offset));
    _offset += 2;
    return v;
}

uint32 ByteStream::getU32() {
    uint32 v = 0;
    if(_offset+4 > _data->size())
        stdext::throw_exception("read failed");

    v = stdext::readULE32(&_data->at(_offset));
    _offset += 4;
    return v;
}

uint64 ByteStream::getU64() {
    uint64 v = 0;
    if(_offset+8 > _data->size())
        stdext::throw_exception("read failed");
    v = stdext::readULE64(&_data->at(_offset));
    _offset += 8;
    return v;
}

int8 ByteStream::get8() {
    int8 v = 0;
    if(_offset+1 > _data->size())
        stdext::throw_exception("read failed");

    v = _data->at(_offset);
    _offset += 1;
    return v;
}

int16 ByteStream::get16() {
    int16 v = 0;
    if(_offset+2 > _data->size())
        stdext::throw_exception("read failed");

    v = stdext::readSLE16(&_data->at(_offset));
    _offset += 2;
    return v;
}

int32 ByteStream::get32() {
    int32 v = 0;
    
    if(_offset+4 > _data->size())
        stdext::throw_exception("read failed");

    v = stdext::readSLE32(&_data->at(_offset));
    _offset += 4;
    return v;
}

int64 ByteStream::get64() {
    int64 v = 0;
    if(_offset+8 > _data->size())
        stdext::throw_exception("read failed");
    v = stdext::readSLE64(&_data->at(_offset));
    _offset += 8;
    return v;
}

std::string ByteStream::getString(uint16 len) {
    std::string str;
    if (0==len)
        len = getU16();
    if(len > 0 && len < 8192) {
        if(_offset+len > _data->size()) {
            stdext::throw_exception("read failed");
            return 0;
        }

        str = std::string((char*)&_data->at(_offset), len);
        _offset += len;
    } else if(len != 0)
        stdext::throw_exception("read failed because string is too big");
    return str;
}