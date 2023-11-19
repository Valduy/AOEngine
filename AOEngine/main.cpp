#include <iostream>

#include "../ECS/World.h"
#include "../Game/Executor.h"
#include "../Application/Application.h"
#include "../Core/Logger.h"
#include "../Core/Debug.h"
#include "../Graphics/GPUDevice.h"
#include "../Graphics/GPUSwapChain.h"

class TestGame : public aoe::IGame {
public:
	void Initialize() override {}
	
	void Terminate() override {}
	
	void PerTickUpdate(float dt) override {
		perTickSum += dt;
	}

	void PerFrameUpdate(float dt) override {
		perFrameSum += dt;

		if (perFrameSum >= 1.0f) {
			AOE_LOG_INFO("Another one second");
			AOE_LOG_INFO("PerTickSum: {}", perTickSum);
			AOE_LOG_INFO("PerFrameSum: {}", perFrameSum);

			perTickSum = 0;
			perFrameSum = 0;
		}
	}

private:
	float perTickSum = 0;
	float perFrameSum = 0;
};

int main() {
	aoe::Application application(L"Game", 800, 600);

	aoe::GPUDevice device;
	AOE_ASSERT(device.Initialize());
	
	aoe::GPUContext context = device.CreateContext();

	aoe::GPUSwapChain swap_chain(device, application.GetWindow());
	AOE_ASSERT(swap_chain.Initialize());
	swap_chain.Terminate();

	device.Terminate();

	TestGame game;
	aoe::Executor executor(game);
	application.SetExecutor(&executor);
	application.Start();

	return 0;
}