#pragma once

#include "../Core/Debug.h"

#include "DXHeaders.h"
#include "GPUPixelFormat.h"
#include "GPUEnums.h"
#include "Viewport.h"

#define AOE_DX_TRY_LOG_ERROR_AND_RETURN(hr) \
	if (FAILED(hr)) { \
		AOE_LOG_ERROR("DX error occured (file: {}, line: {}, code: {})", __FILE__, __LINE__, hr); \
		return; \
	}

#define AOE_DX_SAFE_RELEASE(resource) \
	if (resource != nullptr) { \
		resource->Release(); \
		resource = nullptr; \
	}

#define AOE_DX_TERMINATE_AND_RETURN_ON_FAILURE(hr, return_value) \
	if (FAILED(hr)){ \
		static_cast<aoe::IGPUResource*>(this)->Terminate(); \
		return return_value; \
	}

namespace aoe {

class DXHelper {
public:
	DXHelper() = delete;

	static D3D11_VIEWPORT ToDXViewport(const Viewport& viewport) {
		return {
			viewport.x,
			viewport.y,
			viewport.width,
			viewport.height,
			viewport.min_depth,
			viewport.max_depth,
		};
	}

	static DXGI_FORMAT ToDxgiFormat(const GPUPixelFormat value) {
		return static_cast<DXGI_FORMAT>(value);
	}

	static uint32_t ToBindFlag(GPUTextureFlags value) {
		uint32_t result = 0;

		if ((value & GPUTextureFlags::kShaderResource) != GPUTextureFlags::kNone) {
			result |= D3D11_BIND_SHADER_RESOURCE;
		}
		if ((value & GPUTextureFlags::kRenderTarget) != GPUTextureFlags::kNone) {
			result |= D3D11_BIND_RENDER_TARGET;
		}
		if ((value & GPUTextureFlags::kDepthStencil) != GPUTextureFlags::kNone) {
			result |= D3D11_BIND_DEPTH_STENCIL;
		}
		if ((value & GPUTextureFlags::kUnorderedAccess) != GPUTextureFlags::kNone) {
			result |= D3D11_BIND_UNORDERED_ACCESS;
		}

		return result;
	}

	static D3D11_DEPTH_WRITE_MASK ToDepthWirteMask(const GPUDepthWriteMask value) {
		return static_cast<D3D11_DEPTH_WRITE_MASK>(value);
	}

	static D3D11_COMPARISON_FUNC ToComparsionFunc(const GPUComparsionFunc value) {
		return static_cast<D3D11_COMPARISON_FUNC>(value);
	}

	static D3D11_CULL_MODE ToCullMode(const GPUCullMode value) {
		return static_cast<D3D11_CULL_MODE>(value);
	}

	static D3D11_FILL_MODE ToFillMode(const GPUFillMode value) {
		return static_cast<D3D11_FILL_MODE>(value);
	}

	static D3D11_USAGE ToUsage(const GPUResourceUsage value) {
		return static_cast<D3D11_USAGE>(value);
	}

	static D3D_PRIMITIVE_TOPOLOGY ToPrimitiveTopology(const GPUPrimitiveTopology value) {
		return static_cast<D3D_PRIMITIVE_TOPOLOGY>(value);
	}

	static const char* ToSemanticName(LayoutElementSemantics value) {
		switch (value)
		{
		case aoe::LayoutElementSemantics::kBinormal:
			return "BINORMAL";
		case aoe::LayoutElementSemantics::kBlendIndices:
			return "BLENDINDICES";
		case aoe::LayoutElementSemantics::kBlendWeight:
			return "BLENDWEIGHT";
		case aoe::LayoutElementSemantics::kColor:
			return "COLOR";
		case aoe::LayoutElementSemantics::kNormal:
			return "NORMAL";
		case aoe::LayoutElementSemantics::kPosition:
			return "POSITION";
		case aoe::LayoutElementSemantics::kPositionT:
			return "POSITIONT";
		case aoe::LayoutElementSemantics::kPSize:
			return "PSIZE";
		case aoe::LayoutElementSemantics::kTangent:
			return "TANGENT";
		case aoe::LayoutElementSemantics::kTexcoord:
			return "TEXCOORD";
		default:
			AOE_ASSERT_MSG(false, "Unknown sematics {}.", static_cast<int>(value));
			return "";
		}
	}
};

} // namespace aoe