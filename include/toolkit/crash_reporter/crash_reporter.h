// Copyright (c) 2013 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ATOM_COMMON_CRASH_REPORTER_CRASH_REPORTER_H_
#define ATOM_COMMON_CRASH_REPORTER_CRASH_REPORTER_H_

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "base/strings/string16.h"
#include "base/files/file_path.h"
#include "base/macros.h"

namespace crash_reporter {

class CrashReporter {
 public:
  typedef std::map<std::string, std::string> StringMap;
  typedef std::pair<int, std::string> UploadReportResult;  // upload-date, id

  static CrashReporter* GetInstance();
  static void StartInstance(const std::string& daemon_executable_name, StringMap& options);
  static void ShutdownInstance();

  void Start(const std::string& daemon_executable_name, 
             const std::string& product_name,
             const std::string& company_name,
             const std::string& submit_url,
             const base::FilePath& crashes_dir,
             bool upload_to_server,
             bool skip_system_crash_handler,
             const StringMap& extra_parameters);

  virtual std::vector<CrashReporter::UploadReportResult> GetUploadedReports(
      const base::FilePath& crashes_dir);

  virtual void SetUploadToServer(bool upload_to_server);
  virtual bool GetUploadToServer();
  virtual void SetExtraParameter(const std::string& key,
                                 const std::string& value);
  virtual void RemoveExtraParameter(const std::string& key);

 protected:
  CrashReporter();
  virtual ~CrashReporter();

  virtual void InitBreakpad(const std::string& daemon_executable_name, 
                            const std::string& product_name,
                            const std::string& company_name,
                            const std::string& submit_url,
                            const base::FilePath& crashes_dir,
                            bool upload_to_server,
                            bool skip_system_crash_handler);
  
  virtual void SetUploadParameters();
  virtual void SetRestartCmdline(std::string restart_cmdline);

  StringMap upload_parameters_;
  base::string16 restart_cmdline_;

 private:
  void SetUploadParameters(const StringMap& parameters);

  DISALLOW_COPY_AND_ASSIGN(CrashReporter);
};

}  // namespace crash_reporter

#endif  // ATOM_COMMON_CRASH_REPORTER_CRASH_REPORTER_H_
