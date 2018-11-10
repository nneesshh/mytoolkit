// Copyright (c) 2013 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "crash_reporter_in_process_win.h"

#include <winternl.h>
#include <string>

#include "base/files/file_util.h"
#include "base/logging.h"
#include "base/memory/singleton.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "base/process/launch.h"
#include "base/build/result_codes.h"

#pragma intrinsic(_AddressOfReturnAddress)
#pragma intrinsic(_ReturnAddress)

#ifdef _WIN64
// See http://msdn.microsoft.com/en-us/library/ddssxxy8.aspx
typedef struct _UNWIND_INFO {
	unsigned char Version : 3;
	unsigned char Flags : 5;
	unsigned char SizeOfProlog;
	unsigned char CountOfCodes;
	unsigned char FrameRegister : 4;
	unsigned char FrameOffset : 4;
	ULONG ExceptionHandler;
} UNWIND_INFO, *PUNWIND_INFO;
#endif

namespace crash_reporter {

namespace {

	// Matches breakpad/src/client/windows/common/ipc_protocol.h.
	const int kNameMaxLength = 64;
	const int kValueMaxLength = 64;

	typedef NTSTATUS(WINAPI* NtTerminateProcessPtr)(HANDLE ProcessHandle,
		NTSTATUS ExitStatus);
	char* g_real_terminate_process_stub = NULL;

	void TerminateProcessWithoutDump() {
		// Patched stub exists based on conditions (See InitCrashReporter).
		// As a side note this function also gets called from
		// WindowProcExceptionFilter.
		if (g_real_terminate_process_stub == NULL) {
			::TerminateProcess(::GetCurrentProcess(), content::RESULT_CODE_KILLED);
		}
		else {
			NtTerminateProcessPtr real_terminate_proc =
				reinterpret_cast<NtTerminateProcessPtr>(
					static_cast<char*>(g_real_terminate_process_stub));
			real_terminate_proc(::GetCurrentProcess(), content::RESULT_CODE_KILLED);
		}
	}

#ifdef _WIN64
	int CrashForExceptionInNonABICompliantCodeRange(
		PEXCEPTION_RECORD ExceptionRecord,
		ULONG64 EstablisherFrame,
		PCONTEXT ContextRecord,
		PDISPATCHER_CONTEXT DispatcherContext) {
		EXCEPTION_POINTERS info = { ExceptionRecord, ContextRecord };
		if (!CrashReporter::GetInstance())
			return EXCEPTION_CONTINUE_SEARCH;
		return static_cast<CrashReporterInProcessWin*>(CrashReporter::GetInstance())->
			CrashForException(&info);
	}

	struct ExceptionHandlerRecord {
		RUNTIME_FUNCTION runtime_function;
		UNWIND_INFO unwind_info;
		unsigned char thunk[12];
	};

	bool RegisterNonABICompliantCodeRange(void* start, size_t size_in_bytes) {
		ExceptionHandlerRecord* record =
			reinterpret_cast<ExceptionHandlerRecord*>(start);

		// We assume that the first page of the code range is executable and
		// committed and reserved for breakpad. What could possibly go wrong?

		// All addresses are 32bit relative offsets to start.
		record->runtime_function.BeginAddress = 0;
		record->runtime_function.EndAddress =
			base::checked_cast<DWORD>(size_in_bytes);
		record->runtime_function.UnwindData =
			offsetof(ExceptionHandlerRecord, unwind_info);

		// Create unwind info that only specifies an exception handler.
		record->unwind_info.Version = 1;
		record->unwind_info.Flags = UNW_FLAG_EHANDLER;
		record->unwind_info.SizeOfProlog = 0;
		record->unwind_info.CountOfCodes = 0;
		record->unwind_info.FrameRegister = 0;
		record->unwind_info.FrameOffset = 0;
		record->unwind_info.ExceptionHandler =
			offsetof(ExceptionHandlerRecord, thunk);

		// Hardcoded thunk.
		// mov imm64, rax
		record->thunk[0] = 0x48;
		record->thunk[1] = 0xb8;
		void* handler = &CrashForExceptionInNonABICompliantCodeRange;
		memcpy(&record->thunk[2], &handler, 8);

		// jmp rax
		record->thunk[10] = 0xff;
		record->thunk[11] = 0xe0;

		// Protect reserved page against modifications.
		DWORD old_protect;
		return VirtualProtect(start, sizeof(ExceptionHandlerRecord),
			PAGE_EXECUTE_READ, &old_protect) &&
			RtlAddFunctionTable(&record->runtime_function, 1,
				reinterpret_cast<DWORD64>(start));
	}

	void UnregisterNonABICompliantCodeRange(void* start) {
		ExceptionHandlerRecord* record =
			reinterpret_cast<ExceptionHandlerRecord*>(start);

		RtlDeleteFunctionTable(&record->runtime_function);
	}
#endif  // _WIN64

}  // namespace

CrashReporterInProcessWin::CrashReporterInProcessWin()
	: skip_system_crash_handler_(false),
	code_range_registered_(false) {
}

CrashReporterInProcessWin::~CrashReporterInProcessWin() {
}

void CrashReporterInProcessWin::InitBreakpad(const std::string& daemon_executable_name,
                                             const std::string& product_name,
                                             const std::string& company_name,
                                             const std::string& submit_url,
                                             const base::FilePath& crashes_dir,
                                             bool upload_to_server,
                                             bool skip_system_crash_handler) {
	skip_system_crash_handler_ = skip_system_crash_handler;

	// ExceptionHandler() attaches our handler and ~ExceptionHandler() detaches
	// it, so we must explicitly reset *before* we instantiate our new handler
	// to allow any previous handler to detach in the correct order.
	breakpad_.reset();

	breakpad_.reset(new google_breakpad::ExceptionHandler(
		crashes_dir.value(),
		FilterCallback,
		MinidumpCallback,
		this,
		google_breakpad::ExceptionHandler::HANDLER_ALL));

}

void CrashReporterInProcessWin::SetUploadParameters() {
	upload_parameters_["platform"] = "win32";
}

int CrashReporterInProcessWin::CrashForException(EXCEPTION_POINTERS* info) {
	if (breakpad_) {
		breakpad_->WriteMinidumpForException(info);
		TerminateProcessWithoutDump();
	}
	return EXCEPTION_CONTINUE_SEARCH;
}

// static
bool CrashReporterInProcessWin::FilterCallback(void* context,
	EXCEPTION_POINTERS* exinfo,
	MDRawAssertionInfo* assertion) {
	return true;
}

// static
bool CrashReporterInProcessWin::MinidumpCallback(const wchar_t* dump_path,
	const wchar_t* minidump_id,
	void* context,
	EXCEPTION_POINTERS* exinfo,
	MDRawAssertionInfo* assertion,
	bool succeeded) {
	CrashReporterInProcessWin* self = static_cast<CrashReporterInProcessWin*>(context);
	if (succeeded && !self->skip_system_crash_handler_) {
		//
		CrashReporterInProcessWin *reporter = (CrashReporterInProcessWin *)context;
		if (reporter
			&&reporter->restart_cmdline_.length() > 0) {
			//
			base::LaunchOptions launch_options;
			launch_options.wait = false;
			launch_options.start_hidden = true;
			base::LaunchProcess(reporter->restart_cmdline_, launch_options);
		}
		return true;
	}
	else
		return false;
}

// static
CrashReporterInProcessWin* CrashReporterInProcessWin::GetInstance() {
	return base::Singleton<CrashReporterInProcessWin>::get();
}

// impl fro CrashReporter
// CrashReporter* CrashReporter::GetInstance() {
// 	return CrashReporterInProcessWin::GetInstance();
// }

}  // namespace crash_reporter
