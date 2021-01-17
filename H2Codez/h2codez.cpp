#include "stdafx.h"
#include "H2Tool\H2tool.h"
#include "H2Guerilla\H2Guerilla.h"
#include "H2Sapien\H2Sapien.h"
#include "Common\H2EKCommon.h"
#include "Common\DiscordInterface.h"
#include "Util\Debug.h"
#include "Util\Process.h"
#include "Util\numerical.h"

char app_directory[256];

H2EK H2Toolz::detect_type()
{
	// try and detect type based on module name.
	if (GetModuleHandleW(L"h2Guerilla.exe"))
	{
		game.base = GetModuleHandleW(L"h2Guerilla.exe");
		return H2EK::H2Guerilla;
	}
	else if (GetModuleHandleW(L"h2Sapien.exe"))
	{
		game.base = GetModuleHandleW(L"h2Sapien.exe");
		return H2EK::H2Sapien;
	}
	else if (GetModuleHandleW(L"h2Tool.exe"))
	{
		game.base = GetModuleHandleW(L"h2Tool.exe");
		return H2EK::H2Tool;
	}
	// fallback to checking file information in case the file was renamed.
	// don't bother error handling
	wchar_t exe_file_path[_MAX_PATH + 1];
	size_t module_filename_length = process::GetModuleFileNameW(NULL, exe_file_path, ARRAYSIZE(exe_file_path));
	LOG_CHECK((numerical::is_between<size_t, size_t, size_t>(module_filename_length, 0x0, _MAX_PATH)));

	DWORD version_info_size = GetFileVersionInfoSizeW(exe_file_path, NULL);
	assert(version_info_size != 0);

	void *version_info = new BYTE[version_info_size];
	assert(GetFileVersionInfoW(exe_file_path, NULL, version_info_size, version_info) != 0);
	wchar_t *orginal_filename;
	size_t filename_len;
	// shouldn't be hardcoded but who cares
	VerQueryValueW(version_info, L"\\StringFileInfo\\040904b0\\OriginalFilename", (LPVOID*)&orginal_filename, &filename_len);

	game.base = GetModuleHandleW(NULL);
	std::wstring exe_orginal_filename = orginal_filename;
	delete[] version_info;
	if (exe_orginal_filename == L"H2Guerilla.exe")
		return H2EK::H2Guerilla;
	else if (exe_orginal_filename == L"h2sapien.exe")
		return H2EK::H2Sapien;
	else if (exe_orginal_filename == L"h2tool.exe")
		return H2EK::H2Tool;

	return H2EK::Invalid;
}

void H2Toolz::minimal_init()
{
	game.process_type = detect_type();
	if (game.process_type == H2EK::H2Tool)
		H2ToolPatches::fix_command_line();
}

bool H2Toolz::Init()
{
	Debug::init();
	GetCurrentDirectory(sizeof(app_directory), app_directory);
	game.process_type = detect_type();
	if (game.process_type == H2EK::Invalid) {
		MessageBoxA(0, "H2toolz loaded into unsupported process, will now exit!", "ERROR!", MB_OK);
		return false;
	}
	if (is_debug_build() || game.process_type != H2EK::H2Tool && conf.getBoolean("enable_debug_console", false))
		Debug::Start_Console();

	DiscordInterface::setAppType(game.process_type);
	H2CommonPatches::Init();

	switch (game.process_type) {
	case H2EK::H2Tool:
	{
		H2ToolPatches::Initialize();
		return true;
	}
	case H2EK::H2Guerilla:
	{
		H2GuerrilaPatches::Init();
		return true;
	}
	case H2EK::H2Sapien:
	{
		H2SapienPatches::Init();
		return true;
	}
	case H2EK::Invalid:
	default:
		abort(); // should be unreachable
	}
}