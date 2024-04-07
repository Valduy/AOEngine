#include "pch.h"

#include <Windows.h>

#include <algorithm>

#include "Platform.h"


namespace aoe {

void aoe::Platform::Fatal(const char* msg) {
	AOE_LOG_FATAL(msg);
	Exit(-1);
}

void aoe::Platform::Exit(std::int32_t exit_code) {
	exit(exit_code);
}

std::wstring Platform::GetExecutableDirectory() {
	TCHAR buffer[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, buffer, MAX_PATH);

	std::wstring exe_path(buffer);
	std::replace(exe_path.begin(), exe_path.end(), L'\\', L'/');

	std::wstring::size_type name_pos = exe_path.find_last_of(L'/');
	return exe_path.substr(0, name_pos);
}

} // namespace aoe
