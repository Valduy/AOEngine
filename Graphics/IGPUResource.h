#pragma once

namespace aoe {

class IGPUResource {
public:
	virtual void Terminate() = 0;
	virtual ~IGPUResource() = default;
};

} // namespace aoe