#pragma once

namespace aoe {

class IScene {
public:
	virtual void Initialize() = 0;
	virtual void Terminate() = 0;
	virtual void PerTickUpdate(float dt) = 0;
	virtual void PerFrameUpdate(float dt) = 0;
	virtual ~IScene() = default;
};

} // namespace aoe