#include "pch.h"
#include "Executor.h"

namespace aoe {

Executor::Executor(IGame& game)
	: game_(game)
	, timestamp_(clock::now())
	, lag_(0.0f)
{}

void Executor::Initialize() {
	game_.Initialize();
}

void Executor::Terminate() {
	game_.Terminate();
}

void Executor::ResetDeltaTime() {
	timestamp_ = clock::now();
}

void Executor::Tick() {
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

} // namespace aoe