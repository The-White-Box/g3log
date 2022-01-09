/** ==========================================================================
 * 2012 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
 * with no warranties. This code is yours to share, use and modify with no
 * strings attached and no restrictions or obligations.
 *
 * For more information see g3log/LICENSE or refer refer to http://unlicense.org
 * ============================================================================*/

#include "g3log/active.hpp"

#include <string>

#ifdef _WIN32
#include <Windows.h>
#else
#include <pthread.h>
#endif

namespace kjellkod {
   namespace internal {
      void setThreadName(native_thread_name &&threadNamePrefix) {
         static unsigned threadNo = 0;

#ifdef WIN32
         const native_thread_name threadName(threadNamePrefix + std::to_wstring(threadNo++));
#else
         const native_thread_name threadName(threadNamePrefix + std::to_string(threadNo++));
#endif  // !WIN32
   
#ifdef _WIN32
         static auto setThreadDescription =
            reinterpret_cast<HRESULT(WINAPI *)(HANDLE, PCWSTR)>(GetProcAddress(
               GetModuleHandleW(L"Kernel32.dll"), "SetThreadDescription"));
         if (setThreadDescription == nullptr) {
            return;
         }

         setThreadDescription(::GetCurrentThread(), threadName.c_str());
#elif defined(__APPLE__)
         // The thread name is a meaningful C language string, whose length is
         // restricted to 64 characters, including the terminating null byte
         // ('\0').
         ::pthread_setname_np(threadName.c_str());
#elif defined(__linux__)
         // The thread name is a meaningful C language string, whose length is
         // restricted to 16 characters, including the terminating null byte
         // ('\0').
         ::pthread_setname_np(::pthread_self(), threadName.c_str());
#else
#error "Please define setThreadName for your platform."
#endif  // !_WIN32
      }
   }  // namespace internal
}  // namespace kjellkod