#pragma once

namespace aoe {

class IRenderPass {
public:
	virtual void Initialize() = 0;
	virtual void Terminate() = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;
};

} // namespace aoe