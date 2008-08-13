// Copyright 2008, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//    * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//    * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "base/platform_thread.h"

#if defined(OS_POSIX)
#include <errno.h>
#include <sched.h>
#endif

#if defined(OS_MACOSX)
#include <mach/mach.h>
#elif defined(OS_LINUX)
#include <sys/types.h>
#endif

// static
PlatformThread PlatformThread::Current() {
  PlatformThread thread;

#if defined(OS_WIN)
  thread.thread_ = GetCurrentThread();
#elif defined(OS_POSIX)
  thread.thread_ = pthread_self();
#endif

  return thread;
}

// static
void PlatformThread::YieldCurrentThread() {
#if defined(OS_WIN)
  ::Sleep(0);
#elif defined(OS_POSIX)
  sched_yield();
#endif
}

// static
void PlatformThread::Sleep(int duration_ms) {
#if defined(OS_WIN)
  ::Sleep(duration_ms);
#elif defined (OS_POSIX)
  struct timespec sleep_time, remaining;
  // Contains the portion of duration_ms >= 1 sec.
  sleep_time.tv_sec = duration_ms / 1000;
  duration_ms -= sleep_time.tv_sec * 1000;
  // Contains the portion of duration_ms < 1 sec.
  sleep_time.tv_nsec = duration_ms * 1000 * 1000;  // nanoseconds.
  while (nanosleep(&sleep_time, &remaining) == -1 && errno == EINTR) {
    sleep_time = remaining;
  }
#endif
}

// static
int PlatformThread::CurrentId() {
#if defined(OS_WIN)
  return GetCurrentThreadId();
#elif defined(OS_MACOSX)
  return mach_thread_self();
#elif defined(OS_LINUX)
  return gettid();
#endif
}

bool PlatformThread::operator==(const PlatformThread& other_thread) {
#if defined(OS_WIN)
  return thread_ == other_thread.thread_;
#elif defined(OS_POSIX)
  return pthread_equal(thread_, other_thread.thread_);
#endif
}
