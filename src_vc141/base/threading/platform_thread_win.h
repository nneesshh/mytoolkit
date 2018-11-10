// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_THREADING_PLATFORM_THREAD_WIN_H_
#define BASE_THREADING_PLATFORM_THREAD_WIN_H_

#include "base/threading/platform_thread.h"

#include "base/base_export.h"
//#include "base/feature_list.h"

namespace base {
namespace features {

// Specifies whether a given feature is enabled or disabled by default.
enum FeatureState {
  FEATURE_DISABLED_BY_DEFAULT,
  FEATURE_ENABLED_BY_DEFAULT,
};

// The Feature struct is used to define the default state for a feature. See
// comment below for more details. There must only ever be one struct instance
// for a given feature name - generally defined as a constant global variable or
// file static. It should never be used as a constexpr as it breaks
// pointer-based identity lookup.
struct BASE_EXPORT Feature {
  // The name of the feature. This should be unique to each feature and is used
  // for enabling/disabling features via command line flags and experiments.
  // It is strongly recommended to use CamelCase style for feature names, e.g.
  // "MyGreatFeature".
  const char* const name;

  // The default state (i.e. enabled or disabled) for this feature.
  const FeatureState default_state;
};

// Use THREAD_MODE_BACKGROUND_BEGIN instead of THREAD_PRIORITY_LOWEST for
// ThreadPriority::BACKGROUND threads. This lowers the disk and network I/O
// priority of the thread in addition to the CPU scheduling priority. MSDN
// recommends using this setting for threads that perform background work.
// https://docs.microsoft.com/en-us/windows/desktop/api/processthreadsapi/nf-processthreadsapi-setthreadpriority
BASE_EXPORT extern const Feature kWindowsThreadModeBackground;

}  // namespace features
}  // namespace base

#endif  // BASE_THREADING_PLATFORM_THREAD_WIN_H_
