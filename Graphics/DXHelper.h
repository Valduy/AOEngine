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

	static DXGI_FORMAT ToDxgiFormat(const GPUPixelFormat value) {
		return static_cast<DXGI_FORMAT>(value);
	}
};

} // namespace aoe