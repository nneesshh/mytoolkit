//------------------------------------------------------------------------------
//  MirrorBuffer.cpp
//  (C) 2016 n.lee
//------------------------------------------------------------------------------
#include "MirrorBuffer.h"

// Round up to the next power of two
static uint32_t
next_pot(uint32_t x) {
	--x;
	x |= x >> 1; // handle 2 bit numbers
	x |= x >> 2; // handle 4 bit numbers
	x |= x >> 4; // handle 8 bit numbers
	x |= x >> 8; // handle 16 bit numbers
	x |= x >> 16; // handle 32 bit numbers
	return ++x;
}

//------------------------------------------------------------------------------
/**

*/
CMirrorBuffer::CMirrorBuffer(size_t size)
	: _xbuf(new char[2 * size])
	, _size(size)
	, _datalen(0)
	, _first(0)
	, _last(0)
	, _bytes(0) {

}

//------------------------------------------------------------------------------
/**

*/
CMirrorBuffer::~CMirrorBuffer() {
	delete[] _xbuf;
}

//------------------------------------------------------------------------------
/**

*/
bool
CMirrorBuffer::Write(const char *s, size_t l) {
	if (l > 0) {
		if (_datalen + l > _size) {
			char *tmp = _xbuf;
			size_t sz = _size;

			// realloc
			_size = next_pot(_datalen + 1);
			if (_size <= 64 * 1024 * 1024) {
				_xbuf = new char[2 * _size]; // at max 2 * 64 Megabytes
				memcpy(_xbuf, tmp, sz);
				memcpy(_xbuf + _size, tmp, sz);
				delete[] tmp;
			}
			else {
				// overflow
				return false;
			}
		}
		_bytes += (unsigned long)l;

		// check block crossed mirror border
		if (_last + l > _size) {
			// size left until mirror border crossing
			size_t l1 = _size - _last;
			
			// always copy full block to buffer(_xbuf) + tail pointer(_last)
			// because we have doubled the buffer size as mirror for performance reasons
			memcpy(_xbuf + _last, s, l);
			memcpy(_xbuf, s + l1, l - l1);
			_last = l - l1;
		}
		else {
			memcpy(_xbuf + _last, s, l);	// append tail
			memcpy(_xbuf + _size + _last, s, l);	// append double buffer tail
			_last += l;
		}

		_datalen += l;
	}
	return true;
}

//------------------------------------------------------------------------------
/**

*/
bool
CMirrorBuffer::Read(size_t l, std::string& out) {
	const char *start = GetStart();
	if (Skip(l)) {
		out += std::string(start, l);
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
/**

*/
bool
CMirrorBuffer::Skip(size_t l) {
	if (l > 0) {
		if (l > _datalen) {
			return false; // not enough chars
		}

		// check block crossed mirror border
		if (_first + l > _size) {
			size_t l1 = _size - _first;
			_first = l - l1;
		}
		else {
			_first += l;
		}

		_datalen -= l;

		if (!_datalen) {
			_first = _last = 0;
		}
	}
	return true;
}

/** -- EOF -- **/
