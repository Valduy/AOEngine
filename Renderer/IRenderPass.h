#pragma once

namespace aoe {

class IRenderPass {
public:
	virtual void Initialize() {}
	virtual void Terminate() {}
	virtual void Render() = 0;
};

} // namespace aoe