#pragma once

#include <d3dcompiler.h>

#include <string>

#include "../Core/Logger.h"

#include "DXHelper.h"
#include "GPUDevice.h"
#include "DXByteCode.h"

namespace aoe {

class GPUShadersCompiler {
public:
	inline static const std::string kEntyPoint = "Main";
	inline static const std::string kVsTarget = "vs_5_0";
	inline static const std::string kPsTarget = "ps_5_0";

	GPUShadersCompiler(const GPUDevice& device)
		: device_(device)
	{}

	DXByteCode CompileVertexShader(
		const std::wstring& path, 
		const std::string& entry_point = kEntyPoint, 
		const std::string& target = kVsTarget) 
	{
		ID3DBlob* byte_code = CompileByteCode(path, entry_point, target);
		return { byte_code };
	}

	DXByteCode CompilePixelShader(
		const std::wstring& path,
		const std::string& entry_point = kEntyPoint,
		const std::string& target = kPsTarget)
	{
		ID3DBlob* byte_code = CompileByteCode(path, entry_point, target);
		return { byte_code };
	}

private:
	const GPUDevice& device_;

	ID3DBlob* CompileByteCode(
		const std::wstring& path, 
		const std::string& entry_point, 
		const std::string& target) 
	{
		ID3DBlob* byte_code = nullptr;
		ID3DBlob* error_message = nullptr;
		HRESULT hr = D3DCompileFromFile(
			path.c_str(),
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			entry_point.c_str(),
			target.c_str(),
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // TODO: release configuration
			0,
			&byte_code,
			&error_message);

		if (FAILED(hr)) {
			if (error_message != nullptr) {
				const char* message = static_cast<char*>(error_message->GetBufferPointer());
				AOE_LOG_ERROR(message);
				error_message->Release();
			}

			AOE_DX_SAFE_RELEASE(error_message);
			AOE_DX_SAFE_RELEASE(byte_code);
			return nullptr;
		}

		return byte_code;
	}
};

} // namespace aoe