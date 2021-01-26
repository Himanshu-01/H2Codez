#pragma once
#include "stdafx.h"

class Logs
{
public:
	Logs(char* filename, bool console = true); //Constructor that creates and sets the Log name
	void Exit();      //close file
	void WriteLog(const char* line, ...); //Write to Log File
	bool console;


private:
	std::ofstream file;
};

namespace Debug
{
	void Start_Console();///AllocConsole		
};

Logs &getLogger();


inline void log_verify_output(const char *expression, const char *func_name, const char* file, const int line)
{
	getLogger().console = true;
	getLogger().WriteLog("'%s' failed in '%s' at '%s:%d'!", expression, func_name, file, line);
	DWORD last_error = GetLastError();
	if (last_error)
	{
		LPWSTR messageBuffer = NULL;
		size_t size = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, last_error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&messageBuffer, 0, NULL);
		if (size) {
			getLogger().WriteLog("Last error: '%ws'", messageBuffer);
			LocalFree(messageBuffer);
		} else {
			getLogger().WriteLog("Converting error %d to string failed!", last_error);
		}
		SetLastError(0);
	}
}

extern bool g_assert_failed;
template <typename T>
FORCEINLINE T verify_output(T output, const char *expression, const char *func_name, const char* file, const int line, bool fatal)
{
	if (!output) {
		log_verify_output(expression, func_name, file, line);
		if (fatal) {
			if (IsDebuggerPresent() || is_debug_build()) {
				__debugbreak();
			} else {
				g_assert_failed = true;
				DbgRaiseAssertionFailure();
			}
		}
	}
	return output;
}
#define LOG_CHECK(expression) \
	verify_output(expression, #expression, __FUNCTION__, __FILE__, __LINE__, false)
#define LOG_FUNC(msg, ...)  getLogger().WriteLog(__FUNCTION__  "(): " ## msg, __VA_ARGS__)
#define ASSERT_CHECK(expression) \
	verify_output(expression, #expression, __FUNCTION__, __FILE__, __LINE__, true)
