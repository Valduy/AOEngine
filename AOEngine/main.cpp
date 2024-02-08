#include <iostream>

#include "../Application/Application.h"
#include "../Core/Logger.h"
#include "../Core/Debug.h"

class TestGame : public aoe::IGame {
public:
	TestGame(const aoe::Window& window)
		: window_(window)
	{}

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
	const aoe::Window& window_;
	
	float perTickSum = 0;
	float perFrameSum = 0;
};

int main() {
	aoe::Application application(L"Game", 800, 600);

	TestGame game(application.GetWindow());
	application.Start(game);

	return 0;
}