#include "pch.h"

#include "DX11ShaderHelper.h"

namespace aoe {

DX11GPUVertexShader DX11ShaderHelper::CreateVertexShader(std::wstring path, std::string entry_point) {
	return { CompileByteCode(GPUShaderType::kVertex, path, entry_point) };
}

DX11GPUPixelShader DX11ShaderHelper::CreatePixelShader(std::wstring path, std::string entry_point) {
	return { CompileByteCode(GPUShaderType::kPixel, path, entry_point) };
}

DX11GPUByteCode DX11ShaderHelper::CompileByteCode(GPUShaderType shader_type, std::wstring path, std::string entry_point) {
	GPUShaderProgramDescription shader_program_desc{
		shader_type, path, entry_point,
	};

	return DX11GPUShadersCompiler::CompileShader(shader_program_desc);
}

} // namespace aoe