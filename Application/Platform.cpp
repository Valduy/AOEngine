#include "pch.h"

#include "Platform.h"

namespace aoe {

void aoe::Platform::Fatal(const char* msg) {
	AOE_LOG_FATAL(msg);
	Exit(-1);
}

void aoe::Platform::Exit(std::int32_t exit_code) {
	exit(exit_code);
}

} // namespace aoe
