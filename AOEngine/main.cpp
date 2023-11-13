#include <iostream>

#include "../ECS/World.h"
#include "../Game/Executor.h"
#include "../Application/Application.h"
#include "../Core/Logger.h"

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
			AOE_LOG_INFO("Another one seccon");
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

	TestGame game;
	aoe::Executor executor(game);
	application.SetExecutor(&executor);
	application.Start();

	return 0;
}