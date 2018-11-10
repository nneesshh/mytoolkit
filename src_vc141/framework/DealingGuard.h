#pragma once
//------------------------------------------------------------------------------
/**
    @class CDealingGuard: client side 
    
    (C) 2016 n.lee
*/

//////////////////////////////////////////////////////////////////////////
struct dealing_guard_t {
	int			_targetid;
	int			_status;

	dealing_guard_t() {
		Reset();
	}
	~dealing_guard_t() { }

	void		Reset() {
		_targetid = 0;
		_status = 0;
	}
};

/*EOF*/