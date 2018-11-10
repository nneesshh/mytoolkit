// Copyright (c) 2013 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ATOM_COMMON_CRASH_REPORTER_CRASH_REPORTER_IN_PROCESS_WIN_H_
#define ATOM_COMMON_CRASH_REPORTER_CRASH_REPORTER_IN_PROCESS_WIN_H_

#include <memory>
#include <string>
#include <vector>

#include "crash_reporter.h"
#include "base/compiler_specific.h"
#include "client/windows/handler/exception_handler.h"

namespace base {
	template <typename T> struct DefaultSingletonTraits;
}

namespace crash_reporter {

class CrashReporterInProcessWin : public CrashReporter {
public:
	static CrashReporterInProcessWin* GetInstance();

	void InitBreakpad(const std::string& daemon_executable_name,
                      const std::string& product_name,
                      const std::string& company_name,
                      const std::string& submit_url,
                      const base::FilePath& crashes_dir,
                      bool upload_to_server,
                      bool skip_system_crash_handler) override;

	void SetUploadParameters() override;

	// Crashes the process after generating a dump for the provided exception.
	int CrashForException(EXCEPTION_POINTERS* info);

private:
	friend struct base::DefaultSingletonTraits<CrashReporterInProcessWin>;

	CrashReporterInProcessWin();
	virtual ~CrashReporterInProcessWin();

	static bool FilterCallback(void* context,
		EXCEPTION_POINTERS* exinfo,
		MDRawAssertionInfo* assertion);

	static bool MinidumpCallback(const wchar_t* dump_path,
		const wchar_t* minidump_id,
		void* context,
		EXCEPTION_POINTERS* exinfo,
		MDRawAssertionInfo* assertion,
		bool succeeded);

	// Custom information to be passed to crash handler.
	std::vector<google_breakpad::CustomInfoEntry> custom_info_entries_;
	google_breakpad::CustomClientInfo custom_info_;

	bool skip_system_crash_handler_;
	bool code_range_registered_;
	std::unique_ptr<google_breakpad::ExceptionHandler> breakpad_;

	DISALLOW_COPY_AND_ASSIGN(CrashReporterInProcessWin);
};

}  // namespace crash_reporter

#endif  // ATOM_COMMON_CRASH_REPORTER_CRASH_REPORTER_IN_PROCESS_WIN_H_
