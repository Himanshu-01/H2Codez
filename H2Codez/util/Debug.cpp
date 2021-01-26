#include "stdafx.h"
#include <Dbghelp.h>
#include <Shlwapi.h>
#include <winnt.h>
#include "Debug.h"
#include "Version.h"
#include "process.h"

#define crash_reports_path "reports//crash_reports//"

using namespace Debug;

extern bool g_process_crashed = false;
extern bool g_assert_failed = false;
const wchar_t g_version_data[] = L"H2codez version: " version;

LPTOP_LEVEL_EXCEPTION_FILTER expection_filter = nullptr;
LONG WINAPI Debug::On_UnhandledException(struct _EXCEPTION_POINTERS* ExceptionInfo)
{
	static bool processing_exception = false;
	// did we crash while reporting a crash?
	static bool crash_loop = false;
	if (processing_exception) {
		if (crash_loop) {
			// abandon all hope
			expection_filter = nullptr;
			__fastfail(FAST_FAIL_FATAL_APP_EXIT);
		}
		crash_loop = true;
		LOG_FUNC("Error occurred while processing exception! Attempting to create second crash dump");
	}

	processing_exception = true;
	// print logs to console for easier debugging
	getLogger().console = true;

	if (crash_loop || !g_process_crashed) {
		g_process_crashed = true;

		// make sure the reports path exists
		MakeSureDirectoryPathExists(crash_reports_path);

		CHAR exe_path_buffer[MAX_PATH + 1];
		process::GetModuleFileNameA(NULL, exe_path_buffer, sizeof(exe_path_buffer));
		std::string exe_name = exe_path_buffer;
		exe_name = exe_name.substr(exe_name.find_last_of('\\') + 1);

		time_t timer;
		char timestamp[20];
		struct tm* tm_info;

		time(&timer);
		tm_info = localtime(&timer);

		strftime(timestamp, sizeof(timestamp), "_%Y%m%d-%H%M%S", tm_info);

		// create the crash reports path
		char full_dir_path[400];
		_fullpath(full_dir_path, crash_reports_path, 400);
		SHCreateDirectoryExA(NULL, full_dir_path, NULL);

		std::string dump_file_name = full_dir_path + exe_name + timestamp + ".dmp";

		HANDLE dump_file = CreateFile(dump_file_name.c_str(),
			GENERIC_WRITE,
			0,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		MINIDUMP_EXCEPTION_INFORMATION aMiniDumpInfo;
		aMiniDumpInfo.ThreadId = GetCurrentThreadId();
		aMiniDumpInfo.ExceptionPointers = ExceptionInfo;
		aMiniDumpInfo.ClientPointers = TRUE;

		MINIDUMP_USER_STREAM version_data;
		version_data.Type = CommentStreamW;
		version_data.Buffer = (void*) &g_version_data;
		version_data.BufferSize = sizeof(g_version_data);

		MINIDUMP_USER_STREAM_INFORMATION extra_data;
		extra_data.UserStreamCount = 1;
		extra_data.UserStreamArray = &version_data;

		MiniDumpWriteDump(GetCurrentProcess(),
			GetCurrentProcessId(),
			dump_file,
			(MINIDUMP_TYPE) (MiniDumpWithDataSegs | MiniDumpWithHandleData | MiniDumpWithUnloadedModules |
				MiniDumpWithProcessThreadData | MiniDumpWithIndirectlyReferencedMemory | MiniDumpFilterModulePaths
				),
			&aMiniDumpInfo,
			&extra_data,
			NULL);

		CloseHandle(dump_file);

		std::string error_type = g_assert_failed ? "an internal fatal error" : "a fatal error";

		std::string message = "H2EK has encountered " + error_type + " and needs to exit,\n"
			" a crash dump has been saved to '" + dump_file_name + "',\n"
			"please note the path if you want to report the issue, as the file may be necessary.";
		MessageBoxA(NULL, message.c_str(), "Crash!", 0);
	} else {
		LOG_FUNC("Ignoring exception %d @ %p as the process has already crashed!", ExceptionInfo->ExceptionRecord->ExceptionCode, ExceptionInfo->ExceptionRecord->ExceptionAddress);
	}

	processing_exception = false;

	if (expection_filter)
		return expection_filter(ExceptionInfo);
	else
		return EXCEPTION_CONTINUE_SEARCH;
}

void Debug::init()
{
	if (!is_debug_build()) {
		expection_filter = SetUnhandledExceptionFilter(Debug::On_UnhandledException);
		_set_abort_behavior(1, _WRITE_ABORT_MSG);
	}
}

void Debug::set_expection_filter(LPTOP_LEVEL_EXCEPTION_FILTER filter)
{
	expection_filter = filter;
}

LPTOP_LEVEL_EXCEPTION_FILTER Debug::get_expection_filter()
{
	return expection_filter;
}