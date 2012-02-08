/* ------------------------------------------------------------------------- */
/*
 *  unix/once.h
 *
 *  Copyright (c) 2004 - 2008, clown. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    - Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    - Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    - No names of its contributors may be used to endorse or promote
 *      products derived from this software without specific prior written
 *      permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See
 *  accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 *  Last-modified: Tue 12 Aug 2008 16:17:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_UNIX_ONCE_H
#define CLX_UNIX_ONCE_H

#include <climits>
#include <cstdlib>
#include <pthread.h>

#define CLX_ONCE_INIT 0

namespace clx {
	typedef unsigned long once_flag;
	
	namespace detail {
		once_flag once_global_epoch = ULONG_MAX;
		pthread_mutex_t once_epoch_mutex = PTHREAD_MUTEX_INITIALIZER;
		pthread_cond_t once_epoch_cv = PTHREAD_COND_INITIALIZER;
		
		pthread_key_t epoch_tss_key;
		pthread_once_t epoch_tss_key_flag = PTHREAD_ONCE_INIT;
		
		inline void delete_epoch_tss_data(void* data) {
			free(data);
		}
		
		inline void create_epoch_tss_key() {
			pthread_key_create(&epoch_tss_key, delete_epoch_tss_data);
		}
		
		inline once_flag& get_once_per_thread_epoch() {
			pthread_once(&epoch_tss_key_flag, create_epoch_tss_key);
			void* data = pthread_getspecific(epoch_tss_key);
			if (!data) {
				data = std::malloc(sizeof(once_flag));
				pthread_setspecific(epoch_tss_key, data);
				*static_cast<once_flag*>(data) = ULONG_MAX;
			}
			return *static_cast<once_flag*>(data);
		}
	};
	
	/* --------------------------------------------------------------------- */
	/*
	 *  call_once
	 *
	 *  Based on Mike Burrows fast_pthread_once algorithm as described in
	 *  http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2444.html
	 */
	/* --------------------------------------------------------------------- */
	template <class Functor>
	inline void call_once(once_flag& flag, Functor f) {
		static once_flag const uninitialized_flag = CLX_ONCE_INIT;
		static once_flag const being_initialized = uninitialized_flag + 1;
		once_flag const epoch = flag;
		once_flag& this_thread_epoch = detail::get_once_per_thread_epoch();
		
		if (epoch < this_thread_epoch) {
			pthread_mutex_lock(&detail::once_epoch_mutex);
			while (flag <= being_initialized) {
				if (flag == uninitialized_flag) {
					flag = being_initialized;
					pthread_mutex_unlock(&detail::once_epoch_mutex);
					f();
					pthread_mutex_lock(&detail::once_epoch_mutex);
					
					--detail::once_global_epoch;
					flag = detail::once_global_epoch;
					pthread_cond_broadcast(&detail::once_epoch_cv);
				}
				else {
					while (flag == being_initialized) {
						pthread_cond_wait(&detail::once_epoch_cv, &detail::once_epoch_mutex);
					}
				}
			}
			
			this_thread_epoch = detail::once_global_epoch;
			pthread_mutex_unlock(&detail::once_epoch_mutex);
		}
	}
}

#endif // CLX_UNIX_ONCE_H
