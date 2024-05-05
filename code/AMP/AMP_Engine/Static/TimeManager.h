#pragma once
#include "time.h"
#define NANOS_IN_SECOND 1000000000LL
#define BILLION_AS_DOUBLE 1000000000.0

/// This namespace is used to get the current time
/// 
/// The amp TimeEvent relies on this namespace
namespace TIME
{
	/// @return The current time of the application in nano seconds
	static int64_t currentTimeInNanos() {
		struct timespec res;
		clock_gettime(CLOCK_MONOTONIC, &res);
		return (int64_t)res.tv_sec * NANOS_IN_SECOND + res.tv_nsec;
	}

	/// @return the current time of the application in seconds (**no decimals**)
	static long currentTimeInSec() {
		struct timespec res;
		clock_gettime(CLOCK_MONOTONIC, &res);
		return res.tv_sec;
	}

	/// Calculates nano seconds to seconds with decimals
	/// 
	/// @param nanos Nanoseconds
	/// @return Seconds with decimals
	static double NanosToSeconds(int64_t nanos) {
		return nanos / BILLION_AS_DOUBLE;
	}

	/// @return The current time in seconds with decimals
	static double GetTime(){
		return NanosToSeconds(currentTimeInNanos());
	}
}
