#pragma once

#include <stdlib.h>

#include <cstdint>

#include "../Core/Logger.h"

namespace aoe {

class Platform {
public:
	static void Fatal(const char* msg);
	static void Exit(std::int32_t exit_code);
};

} // namespace aoe