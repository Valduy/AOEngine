#pragma once

#include <stdlib.h>

#include <cstdint>

#include "../Core/Logger.h"

namespace aoe {

class Platform {
public:
	Platform() = delete;

	static void Fatal(const char* msg);
	static void Exit(int32_t exit_code);
	static std::wstring GetExecutableDirectory();

private:
	static std::wstring GetExecutableDirectoryInternal();
};

} // namespace aoe