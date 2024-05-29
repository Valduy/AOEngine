#pragma once

#include "../Graphics/DX11GPUShadersCompiler.h"
#include "../Graphics/DX11GPUVertexShader.h"
#include "../Graphics/DX11GPUPixelShader.h"

namespace aoe {

class DX11ShaderHelper {
public:
	DX11ShaderHelper() = delete;

	static DX11GPUVertexShader CreateVertexShader(std::wstring path, std::string entry_point = "VertexMain");
	static DX11GPUPixelShader CreatePixelShader(std::wstring path, std::string entry_point = "PixelMain");

private:
	static DX11GPUByteCode CompileByteCode(GPUShaderType shader_type, std::wstring path, std::string entry_point);
};

} // namespace aoe