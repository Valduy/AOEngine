#include <iostream>

#include "../ECS/World.h"
#include "../Game/Executor.h"

class TestGame : public aoe::IGame {
public:
	void Initialize() override {}
	
	void Terminate() override {}
	
	void PerTickUpdate(float dt) override {
		perTickSum += dt;
		//std::cout << "dt: " << dt << std::endl;
	}

	void PerFrameUpdate(float dt) override {
		perFrameSum += dt;
		//std::cout << "fixed delta time: " << dt << std::endl;

		if (perFrameSum >= 1.0f) {
			std::cout << "Another one second" << std::endl;
			std::cout << "PerTickSum: " << perTickSum << std::endl;
			std::cout << "PerFrameSum: " << perFrameSum << std::endl;

			perTickSum = 0;
			perFrameSum = 0;
		}
	}

private:
	float perTickSum = 0;
	float perFrameSum = 0;
};

int main() {
	TestGame game;
	aoe::Executor executor(game);
	executor.ResetDeltaTime();

	while (true) {
		executor.Tick();
	}

	return 0;
}