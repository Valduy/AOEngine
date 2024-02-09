#include "pch.h"

#include "../Core/Logger.h"

#include "DX11GPUShadersCompiler.h"

namespace aoe {

DX11GPUByteCode DX11GPUShadersCompiler::CompileShader(const GPUShaderProgramDescription& description) {
	ID3DBlob* byte_code = nullptr;

	switch (description.shader_type) {
	case GPUShaderType::kVertex:
		byte_code = CompileByteCode(description.path, description.entry_point, kVsTarget);
		break;
	case GPUShaderType::kPixel:
		byte_code = CompileByteCode(description.path, description.entry_point, kPsTarget);
		break;
	default:
		AOE_ASSERT_MSG(false, "Unexpected shader type");
	}

	return { byte_code };
}

ID3DBlob* DX11GPUShadersCompiler::CompileByteCode(const std::wstring& path, const std::string& entry_point, const std::string& target) {
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

} // namespace aoe