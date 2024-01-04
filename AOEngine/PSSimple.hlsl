#include "ShaderStructs.hlsli"

float4 Main(PS_IN input) : SV_TARGET {
	return input.color;
}