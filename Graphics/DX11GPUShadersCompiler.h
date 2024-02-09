#pragma once

#include <d3dcompiler.h>

#include <string>

#include "DX11Helper.h"
#include "DX11GPUByteCode.h"

namespace aoe {

struct GPUShaderProgramDescription {
	inline static const std::string kEntryPoint = "Main";

	GPUShaderType shader_type;
	std::wstring path;
	std::string entry_point;

	GPUShaderProgramDescription(
		GPUShaderType shader_type, 
		std::wstring path, 
		std::string entry_point = kEntryPoint
	)
		: shader_type(shader_type)
		, path(std::move(path))
		, entry_point(std::move(entry_point))
	{}
};

class DX11GPUShadersCompiler {
public:
	DX11GPUShadersCompiler() = delete;

	static DX11GPUByteCode CompileShader(const GPUShaderProgramDescription& description);

private:
	inline static const std::string kVsTarget = "vs_5_0";
	inline static const std::string kPsTarget = "ps_5_0";

	static ID3DBlob* CompileByteCode(const std::wstring& path, const std::string& entry_point, const std::string& target);
};

} // namespace aoe