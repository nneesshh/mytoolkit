#include "clear_at_exit.h"

#ifdef _WIN32
#  ifdef _DEBUG
#    include <crtdbg.h.>
#  endif
#endif

#include "base/command_line.h"
#include "base/path_service.h"
#include "base/threading/thread_restrictions.h"
#include "base/threading/thread_local_storage.h"

#ifdef _WIN32
#include "base/win/scoped_handle_verifier.h"
#endif

namespace base {

void clear_at_exit() {
  //
  PathService::ClearAtExit();
  CommandLine::Reset();

#ifdef _WIN32
  assert(_CrtCheckMemory());
  win::internal::ClearHandleVerifierAtExit();
  internal::PlatformThreadLocalStorage::OnThreadExit();
  assert(_CrtCheckMemory());
#endif
}

}