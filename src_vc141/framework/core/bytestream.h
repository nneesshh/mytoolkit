#ifndef BYTESTREAM_H
#define BYTESTREAM_H

#include "cocos2d.h"
#include "declarations.h"
#include "../stdext/databuffer.h"

// @bindclass
class ByteStream : public stdext::shared_object {
public:
	ByteStream(const Uint8ArrayPtr& data);
    ~ByteStream();

	void seek(uint pos) {
		if (pos > _data->size())
			stdext::throw_exception("seek failed");
		
		_offset = pos;
	}
	void skip(uint len) { seek(tell() + len); }
	uint size() { return _data->size(); }
	uint tell() { return _offset; }

    uint8 getU8();
    uint16 getU16();
    uint32 getU32();
    uint64 getU64();
    int8 get8();
    int16 get16();
    int32 get32();
    int64 get64();
	std::string getString(uint16 len = 0);
	uint8 getPointX() { return getU8(); }
	uint8 getPointY() { return getU8(); }

	ByteStreamPtr asByteStream() { return static_self_cast<ByteStream>(); }

private:
	const Uint8ArrayPtr _data;
	uint _offset;
};

#endif
