/* ------------------------------------------------------------------------- */
/*
 *  timer.h
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
 *  Last-modified: Wed 16 Jul 2008 14:38:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_WIN32_TIMER_H
#define CLX_WIN32_TIMER_H

#include <windows.h>
#pragma comment(lib, "winmm.lib")

namespace clx {
	/* --------------------------------------------------------------------- */
	//  gettime_policy
	/* --------------------------------------------------------------------- */
	struct use_time_get_time {
		use_time_get_time() { ::timeBeginPeriod(1); }
		
		static double gettime() {
			return static_cast<double>(::timeGetTime()) * 1e-3;
		}
		
		static double precision() { return 1.0 * 1e-3; }
	};
	
	struct use_query_performance {
		static double gettime() {
			LARGE_INTEGER freq, now;
			std::memset(&freq, 0, sizeof(freq));
			std::memset(&now, 0, sizeof(now));
			
			if (!::QueryPerformanceFrequency(&freq)) {
				return static_cast<double>(::timeGetTime()) * 1e-3;
			}
			::QueryPerformanceCounter(&now);
			return now.QuadPart / static_cast<double>(freq.QuadPart);
		}
		
		static double precision() {
			LARGE_INTEGER freq;
			std::memset(&freq, 0, sizeof(freq));
			if (!::QueryPerformanceFrequency(&freq)) {
				return 1.0 * 1e-3;
			}
			return 1.0 / static_cast<double>(freq.QuadPart);
		}
	};
	
#ifdef CLX_USE_HIGH_PRECISION_TIMER
	typedef use_query_performance gettime_policy;
#else
	typedef use_time_get_time gettime_policy;
#endif
	
	/* --------------------------------------------------------------------- */
	//  sleep
	/* --------------------------------------------------------------------- */
	inline int sleep(double sec) {
		::Sleep(static_cast<int>(sec * 1000));
		return 0;
	}
}

#endif // CLX_WIN32_TIMER_H
