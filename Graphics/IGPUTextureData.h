#pragma once

namespace aoe {

class IGPUTextureData {
public:
	virtual size_t GetByteWidth() const = 0;
	virtual const void* GetBufferPointer() const = 0;
	virtual size_t GetBufferSize() const = 0;
	virtual ~IGPUTextureData() = default;
};

} // namespace aoe