/** ==========================================================================
 * 2010 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
 * with no warranties. This code is yours to share, use and modify with no
 * strings attached and no restrictions or obligations.
 *
 * For more information see g3log/LICENSE or refer refer to http://unlicense.org
 * ============================================================================
 *
 * Example of a Active Object, using C++11 std::thread mechanisms to make it
 * safe for thread communication.
 *
 * This was originally published at http://sites.google.com/site/kjellhedstrom2/active-object-with-cpp0x
 * and inspired from Herb Sutter's C++11 Active Object
 * http://herbsutter.com/2010/07/12/effective-concurrency-prefer-using-active-objects-instead-of-naked-threads
 *
 * Last update 2013-12-19 by Kjell Hedstrom,
 * e-mail: hedstrom at kjellkod dot cc
 * linkedin: http://linkedin.com/se/kjellkod */

#pragma once

#include <thread>
#include <functional>
#include <memory>
#include "g3log/shared_queue.hpp"

namespace kjellkod {
   typedef std::function<void() > Callback;

#ifdef WIN32
   typedef std::wstring native_thread_name;
   const inline native_thread_name DefaultThreadNamePrefix{L"G3log_Worker#"};
#else
   typedef std::string native_thread_name;
   const inline native_thread_name DefaultThreadNamePrefix{"G3log_Worker#"};
#endif  // !WIN32

   namespace internal {
      native_thread_name getThreadName(std::thread::native_handle_type handle);
      void setThreadName(const native_thread_name &threadNamePrefix);
   }

   class Active {
   private:
      Active() : done_(false) {} // Construction ONLY through factory createActive();
      Active(const Active &) = delete;
      Active &operator=(const Active &) = delete;

      void run(const native_thread_name &threadNamePrefix) {
         internal::setThreadName(threadNamePrefix);

         while (!done_) {
            Callback func;
            mq_.wait_and_pop(func);
            func();
         }
      }

      shared_queue<Callback> mq_;
      std::thread thd_;
      bool done_;


   public:
      virtual ~Active() {
         send([this]() noexcept { done_ = true;});
         thd_.join();
      }

      void send(Callback msg_) {
         mq_.push(msg_);
      }

      /// Factory: safe construction of object before thread start
      static std::unique_ptr<Active> createActive(
         const native_thread_name &threadNamePrefix = DefaultThreadNamePrefix) {
         std::unique_ptr<Active> aPtr(new Active());
         aPtr->thd_ = std::thread(&Active::run, aPtr.get(), threadNamePrefix);
         return aPtr;
      }
   };



} // kjellkod
