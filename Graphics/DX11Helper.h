#pragma once

#include "../Core/Debug.h"

#include "DX11Headers.h"
#include "GPUPixelFormat.h"
#include "GPUEnums.h"
#include "Viewport.h"

#define AOE_DX_ERROR_MESSAGE(hr, msg) std::format("DX error occured [file: {}, line: {}, code: {}]:\n\t{}", __FILE__, __LINE__, hr, msg)

#define AOE_DX_TRY_LOG_ERROR_AND_RETURN(hr, msg)      \
	if (FAILED(hr)) {                                 \
		AOE_LOG_ERROR(AOE_DX_ERROR_MESSAGE(hr, msg)); \
		return;                                       \
	}

#define AOE_DX_TRY_LOG_ERROR_AND_THROW(hr, msg)                    \
	if (FAILED(hr)) {                                              \
		std::string error_message = AOE_DX_ERROR_MESSAGE(hr, msg); \
		AOE_LOG_ERROR(error_message);							   \
		throw std::runtime_error(error_message);                   \
	}

#define AOE_DX_SAFE_RELEASE(resource) \
	if (resource != nullptr) {        \
		resource->Release();          \
		resource = nullptr;           \
	}

namespace aoe {

class DX11Helper {
public:
	DX11Helper() = delete;

	static DXGI_FORMAT ToDxgiFormat(const GPUPixelFormat value) {
		return static_cast<DXGI_FORMAT>(value);
	}

	static GPUPixelFormat FromDxgiFormat(const DXGI_FORMAT value) {
		return static_cast<GPUPixelFormat>(value);
	}
};

} // namespace aoe