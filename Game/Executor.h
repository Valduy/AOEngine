#pragma once

#include <chrono>

#include "IGame.h"

namespace aoe {

class Executor {
public:
	static constexpr int32_t kFps = 60; // TODO: move outside
	static constexpr float kFrameSec = 1.0f / kFps;

	Executor(IGame& game);

	void Initialize();
	void Terminate();
	void ResetDeltaTime();
	void Tick();

private:
	using clock = std::chrono::high_resolution_clock;
	using time_point = std::chrono::steady_clock::time_point;

	static const size_t kLagCompinsationLimitation = 3;

	IGame& game_;
	time_point timestamp_;
	float lag_;
};

} // namespace aoe