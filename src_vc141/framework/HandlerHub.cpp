//------------------------------------------------------------------------------
//  HandlerHub.cpp
//  (C) 2016 n.lee
//------------------------------------------------------------------------------
#include "HandlerHub.h"

//------------------------------------------------------------------------------
/**

*/
CHandlerHub::CHandlerHub()
	: _registeredHandlerMax(0) {
	
}

//------------------------------------------------------------------------------
/**

*/
CHandlerHub::~CHandlerHub() {
	IHandler *pHandler;
	int i;
	for (i = 0; i < _registeredHandlerMax; ++i) {
		pHandler = _arrHandler[i];
		if (pHandler) {
			delete pHandler;
		}
	}
}

//------------------------------------------------------------------------------
/**

*/
void
CHandlerHub::OnInit(time_t tmNow) {
	RegisterHandlers();

	//
	IHandler *pHandler;
	int i;
	for (i = 0; i < _registeredHandlerMax; ++i) {
		pHandler = _arrHandler[i];
		if (pHandler) {
			pHandler->OnInit(tmNow);
		}
	}
}

//------------------------------------------------------------------------------
/**

*/
void
CHandlerHub::OnDelete() {
	IHandler *pHandler;
	int i;
	for (i = 0; i < _registeredHandlerMax; ++i) {
		pHandler = _arrHandler[i];
		if (pHandler) {
			pHandler->OnDelete();
		}
	}
}

//------------------------------------------------------------------------------
/**

*/
void
CHandlerHub::OnUpdate(float fElapsed) {
	IHandler *pHandler;
	int i;
	for (i = 0; i < _registeredHandlerMax; ++i) {
		pHandler = _arrHandler[i];
		if (pHandler) {
			pHandler->OnUpdate(fElapsed);
		}
	}
}

/* -- EOF -- */