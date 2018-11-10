#pragma once
//------------------------------------------------------------------------------
/**
@class IHandler

(C) 2016 n.lee
*/

//------------------------------------------------------------------------------
/**
@brief IHandler
*/
class IHandler {
public:
	virtual ~IHandler() { }

	virtual void			OnInit(time_t tmNow) = 0;
	virtual void			OnDelete() = 0;

	///
	virtual void 			OnUpdate(float fElapsed) = 0;

	virtual void			SetHid(int nHid) {
		_hid = nHid;
	}

	virtual int				GetHid() {
		return _hid;
	}

public:
	int _hid;
};

/*EOF*/