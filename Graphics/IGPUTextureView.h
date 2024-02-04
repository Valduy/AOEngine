#pragma once

namespace aoe {

class IGPUTexture2D;

class IGPUTextureView {
public:
	virtual IGPUTexture2D* GetTexture() const = 0;

	virtual ~IGPUTextureView() = default;
};

} // namespace aoe