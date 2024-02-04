#pragma once

#include "GPUEnums.h"

namespace aoe {

struct GPUBufferDescription {
	GPUBufferType buffer_type;
	GPUResourceUsage usage;

	bool IsVertexBuffer() const {
		return buffer_type == GPUBufferType::kVertexBuffer;
	}

	bool IsIndexBuffer() const {
		return buffer_type == GPUBufferType::kIndexBuffer;
	}

	bool IsConstantBuffer() const {
		return buffer_type == GPUBufferType::kConstantBuffer;
	}

	bool IsDynamic() const {
		return usage == GPUResourceUsage::kDynamic;
	}

	GPUBufferDescription(GPUBufferType buffer_type, GPUResourceUsage usage)
		: buffer_type(buffer_type)
		, usage(usage)
	{}

	GPUBufferDescription()
		: buffer_type(GPUBufferType::kVertexBuffer)
		, usage(GPUResourceUsage::kDefault)
	{}
};

class IGPUBuffer {
public:
	virtual void* GetNative() const = 0;
	virtual const GPUBufferDescription& GetDescription() const = 0;

	virtual size_t GetElementsCount() const = 0;
	virtual size_t GetSize() const = 0;
	virtual size_t GetStride() const = 0;

	virtual bool IsVertexBuffer() const = 0;
	virtual bool IsIndexBuffer() const = 0;
	virtual bool IsConstantBuffer() const = 0;
	virtual bool IsDynamic() const = 0;

	virtual ~IGPUBuffer() = default;
};

}; // namespace aoe