#pragma once
//------------------------------------------------------------------------------
/**
    @class CUsingCapnp
    (C) 2016 n.lee
*/
#include "thirdparty/g/capnp/kj/windows-sanity.h"
#include "thirdparty/g/capnp/kj/debug.h"
#include "thirdparty/g/capnp/kj/async.h"
#include "thirdparty/g/capnp/kj/async-io.h"

#ifdef _WIN32
#include "thirdparty/g/capnp/kj/async-win32.h"
#else
#include "thirdparty/g/capnp/kj/async-unix.h"
#endif

/*EOF*/