#include "../Application/Application.h"

class SolarSystemScene : public aoe::IScene {
public:
	SolarSystemScene(const aoe::Window& window)
		: window_(window)
	{}

	void Initialize() override {};

	void Terminate() override {};

	void PerTickUpdate(float dt) override {}

	void PerFrameUpdate(float dt) override {}

private:
	const aoe::Window& window_;
};

int main() {
	aoe::Application application(L"Game", 800, 600);

	SolarSystemScene scene(application.GetWindow());
	application.Start(scene);

	return 0;
}