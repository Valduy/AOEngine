#pragma once

#include <chrono>

#include "IGame.h"

namespace aoe {

class Executor {
public:
	static constexpr float kFrameSec = 1.0f / 60;

	Executor(IGame& game)
		: game_(game)
		, timestamp_()
		, lag_(0.0f)
	{}

	void Initialize() {
		game_.Initialize();
	}

	void Terminate() {
		game_.Terminate();
	}

	void ResetDeltaTime() {
		timestamp_ = clock::now();
	}

	void Tick() {
		using namespace std::chrono;
		
		time_point time = clock::now();
		nanoseconds duration_ns = time - timestamp_;
		float dt = duration_cast<duration<float>>(duration_ns).count();
		size_t compinsations = 0;

		timestamp_ = time;
		lag_ += dt;

		while (lag_ >= kFrameSec) {
			lag_ -= kFrameSec;
			game_.PerFrameUpdate(kFrameSec);
			compinsations += 1;

			if (compinsations >= kLagCompinsationLimitation) {
				dt = kFrameSec * kLagCompinsationLimitation;
				lag_ = 0;
				break;
			}
		}

		game_.PerTickUpdate(dt);
	}

private:
	using clock = std::chrono::high_resolution_clock;
	using time_point = std::chrono::steady_clock::time_point;

	static const size_t kLagCompinsationLimitation = 3;

	IGame& game_;
	time_point timestamp_;
	float lag_;
};

} // namespace aoe