// Copyright (c) 2013 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "crash_reporter.h"

#include "base/path_service.h"
#include "base/files/file_util.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/utf_string_conversions.h"
#include "base/strings/string_split.h"
#include "base/command_line.h"

namespace crash_reporter {

CrashReporter::CrashReporter() {
  auto cmd = base::CommandLine::ForCurrentProcess();
}

CrashReporter::~CrashReporter() {
}

void CrashReporter::Start(const std::string& daemon_executable_name, 
                          const std::string& product_name,
                          const std::string& company_name,
                          const std::string& submit_url,
                          const base::FilePath& crashes_dir,
                          bool upload_to_server,
                          bool skip_system_crash_handler,
                          const StringMap& extra_parameters) {
  SetUploadParameters(extra_parameters);

  InitBreakpad(daemon_executable_name, product_name, company_name, submit_url,
               crashes_dir, upload_to_server, skip_system_crash_handler);
}

void CrashReporter::SetUploadParameters(const StringMap& parameters) {
  upload_parameters_ = parameters;
  upload_parameters_["process_type"] = "renderer";

  // Setting platform dependent parameters.
  SetUploadParameters();
}

void CrashReporter::SetUploadToServer(const bool upload_to_server) {
}

bool CrashReporter::GetUploadToServer() {
  return true;
}

std::vector<CrashReporter::UploadReportResult>
CrashReporter::GetUploadedReports(const base::FilePath& crashes_dir) {
  std::string file_content;
  std::vector<CrashReporter::UploadReportResult> result;
  base::FilePath uploads_path =
      crashes_dir.Append(FILE_PATH_LITERAL("uploads.log"));
  if (base::ReadFileToString(uploads_path, &file_content)) {
    std::vector<std::string> reports = base::SplitString(
        file_content, "\n", base::TRIM_WHITESPACE, base::SPLIT_WANT_NONEMPTY);
    for (const std::string& report : reports) {
      std::vector<std::string> report_item = base::SplitString(
          report, ",", base::TRIM_WHITESPACE, base::SPLIT_WANT_NONEMPTY);
      int report_time = 0;
      if (report_item.size() >= 2 && base::StringToInt(report_item[0],
            &report_time)) {
        result.push_back(CrashReporter::UploadReportResult(report_time,
            report_item[1]));
      }
    }
  }
  return result;
}

void CrashReporter::InitBreakpad(const std::string& daemon_executable_name,
                                 const std::string& product_name,
                                 const std::string& company_name,
                                 const std::string& submit_url,
                                 const base::FilePath& crashes_dir,
                                 bool auto_submit,
                                 bool skip_system_crash_handler) {
}

void CrashReporter::SetUploadParameters() {
}

void CrashReporter::SetRestartCmdline(std::string restart_cmdline) {
	restart_cmdline_ = base::ASCIIToUTF16(restart_cmdline);
}

void CrashReporter::SetExtraParameter(const std::string& key,
                                      const std::string& value) {
}

void CrashReporter::RemoveExtraParameter(const std::string& key) {
}

#if defined(OS_MACOSX) && defined(MAS_BUILD)
// static
CrashReporter* CrashReporter::GetInstance() {
  static CrashReporter crash_reporter;
  return &crash_reporter;
}
#endif

void CrashReporter::StartInstance(const std::string& daemon_executable_name, StringMap& options) {
  auto reporter = GetInstance();
  if (!reporter) return;

  std::string product_name = options["productName"];
  std::string company_name = options["companyName"];
  std::string submit_url = options["submitURL"];
  std::string crashes_dir = options["crashesDirectory"];

  base::FilePath crashes_path;
  base::PathService::Get(base::DIR_EXE, &crashes_path);
  crashes_path = crashes_path.AppendASCII(crashes_dir.c_str()).NormalizePathSeparators();

  reporter->SetRestartCmdline(options["restartCmdline"]);

  StringMap extra_parameters;
  extra_parameters["_productName"] = product_name;
  extra_parameters["_companyName"] = company_name;

  reporter->Start(daemon_executable_name, product_name, company_name, submit_url, crashes_path, false,
                  false, extra_parameters);
}

void CrashReporter::ShutdownInstance() {
	auto reporter = GetInstance();
	if (!reporter) return;
	delete reporter;
}

}  // namespace crash_reporter
