#pragma once
//------------------------------------------------------------------------------
/**
	@class CBitFlag

	(C) 2016 n.lee
*/

#include "bit_flag.h"

//////////////////////////////////////////////////////////////////////////
struct CBitFlag
{
	struct bit_flag_t _bitflag;

	CBitFlag()
	{
		Reset();
	}

	CBitFlag(uint64_t flag)
	{
		Reset(flag);
	}

	~CBitFlag() { }

public:
	void						SetFlag(uint64_t flag)
	{
		bit_flag_set(&_bitflag, flag);
	}

	void						ClearFlag(uint64_t flag)
	{
		bit_flag_clear(&_bitflag, flag);
	}

	bool						TestFlag(uint64_t flag)
	{
		return (0 != bit_flag_test(&_bitflag, flag));
	}

	void						Reset(uint64_t flag = 0)
	{
		bit_flag_reset(&_bitflag, flag);
	}

	uint64_t				FlagValue()
	{
		return _bitflag._flag;
	}

};

/*EOF*/