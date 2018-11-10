#pragma once
//------------------------------------------------------------------------------
/**
@class IConfig

(C) 2016 n.lee
*/

//------------------------------------------------------------------------------
/**
@brief IConfig
*/
class IConfig {
public:
	virtual ~IConfig() { }

	virtual bool				IsReady() = 0;
};

/*EOF*/