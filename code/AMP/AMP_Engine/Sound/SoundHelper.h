#pragma once
#include "SLES/OpenSLES.h"
#include "SLES/OpenSLES_Android.h"

/// Possible valid or invalidation states when making sles calls
static const char* result_strings[] = {
	"SUCCESS",
	"PRECONDITIONS_VIOLATED",
	"PARAMETER_INVALID",
	"MEMORY_FAILURE",
	"RESOURCE_ERROR",
	"RESOURCE_LOST",
	"IO_ERROR",
	"BUFFER_INSUFFICIENT",
	"CONTENT_CORRUPTED",
	"CONTENT_UNSUPPORTED",
	"CONTENT_NOT_FOUND",
	"PERMISSION_DENIED",
	"FEATURE_UNSUPPORTED",
	"INTERNAL_ERROR",
	"UNKNOWN_ERROR",
	"OPERATION_ABORTED",
	"CONTROL_LOST"
};

/// Converts result to string
static const char* result_to_string(SLresult result)
{
	static char buffer[32] = {0};
	if ( /* result >= 0 && */ result < sizeof(result_strings) / sizeof(result_strings[0]))
		return result_strings[result];
	return buffer;
}

/// Compares result against expected and exit suddenly if wrong
static void sles_check2(SLresult result, int line)
{
	if (SL_RESULT_SUCCESS != result) {
		const char* err = result_to_string(result);
		throw "Invalid Value";
	}
}

/// Macro to check the sles state, adds the source code line number
#ifndef sles_check
#define sles_check(result) sles_check2(result, __LINE__)
#endif 