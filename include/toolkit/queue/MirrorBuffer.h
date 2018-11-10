#pragma once
//------------------------------------------------------------------------------
/**
    @class CMirrorBuffer
    
    (C) 2016 n.lee
*/
#include <string>

//------------------------------------------------------------------------------
/**
	Mirrored circular buffer
*/
class CMirrorBuffer {
public:
	CMirrorBuffer(size_t size);
	~CMirrorBuffer();

	/** append l bytes from p to buffer */
	bool						Write(const char *p, size_t l);

	/** copy l bytes from buffer to string */
	bool						Read(size_t l, std::string& out);

	/** skip l bytes from buffer */
	bool						Skip(size_t l);

	/** total data length */
	size_t						GetLength() {
		return _datalen;
	}

	/** pointer to circular buffer beginning */
	const char *				GetStart() {
		return _xbuf + _first;
	}

	/** return free space in buffer, number of bytes until buffer overrun */
	size_t						Space()	{
		return _size - _datalen;
	}

	/** return total number of bytes written to this buffer, ever */
	unsigned long				ByteCounter(bool clear = false) {
		if (clear) {
			unsigned long x = _bytes;
			_bytes = 0;
			return x;
		}
		return _bytes;
	}

private:
	CMirrorBuffer(const CMirrorBuffer&) = delete;
	CMirrorBuffer& operator=(const CMirrorBuffer&) = delete;

	char			*_xbuf;
	size_t			_size;
	size_t			_datalen; // if _last is not wrapped between mirror, _datalen = _last - first, if _last is wrapped, _datalen = [ _size - (_first - _last) or _size * 2 - (_first - _last)].
	size_t			_first;
	size_t			_last;
	unsigned long	_bytes;
};

/*EOF*/