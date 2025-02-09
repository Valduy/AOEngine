#include "pch.h"

#include "Executor.h"

namespace aoe {

Executor::Executor(IScene& scene)
	: scene_(scene)
	, timestamp_(clock::now())
	, lag_(0.0f)
{
	scene_.Initialize();
	ResetDeltaTime();
}

Executor::~Executor() {
	scene_.Terminate();
}

void Executor::ResetDeltaTime() {
	timestamp_ = clock::now();
}

void Executor::Tick() {
	using namespace std::chrono;

	const time_point time = clock::now();
	const nanoseconds duration_ns = time - timestamp_;

	float dt = duration_cast<duration<float>>(duration_ns).count();
	size_t compinsations = 0;

	timestamp_ = time;
	lag_ += dt;

	while (lag_ >= kFrameSec) {
		lag_ -= kFrameSec;
		scene_.PerFrameUpdate(kFrameSec);
		compinsations += 1;

		if (compinsations >= kLagCompinsationLimitation) {
			dt = kFrameSec * kLagCompinsationLimitation;
			lag_ = 0;
			break;
		}
	}

	scene_.PerTickUpdate(dt);
}

} // namespace aoe