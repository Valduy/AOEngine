#pragma once

#include <stdlib.h>

#include <cstdint>

#include "../Core/Logger.h"

namespace aoe {

class Platform {
public:
	static void Fatal(const char* msg) {
		AOE_LOG_FATAL(msg);
		Exit(-1);
	}

	static void Exit(std::int32_t exit_code) {
		exit(exit_code);
	}
};

} // namespace aoe