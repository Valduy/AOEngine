#pragma once

#include "../Graphics/DX11GPUShadersCompiler.h"
#include "../Graphics/DX11GPUVertexShader.h"
#include "../Graphics/DX11GPUPixelShader.h"

namespace aoe {

class DX11ShaderHelper {
public:
	DX11ShaderHelper() = delete;

	static DX11GPUVertexShader CreateVertexShader(std::wstring path, std::string entry_point = "VertexMain") {
		return { CompileByteCode(GPUShaderType::kVertex, path, entry_point) };
	}

	static DX11GPUPixelShader CreatePixelShader(std::wstring path, std::string entry_point = "PixelMain") {
		return { CompileByteCode(GPUShaderType::kPixel, path, entry_point) };
	}

private:
	static DX11GPUByteCode CompileByteCode(GPUShaderType shader_type, std::wstring path, std::string entry_point) {
		GPUShaderProgramDescription shader_program_desc{
			shader_type, path, entry_point,
		};

		return DX11GPUShadersCompiler::CompileShader(shader_program_desc);
	}
};

} // namespace aoe