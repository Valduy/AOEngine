#include "GBuffer.hlsl"

struct ColorData {
    float3 color;
    float dummy;
};

cbuffer ColorBuffer : register(b0) {
    ColorData Color;
}

Texture2D DiffuseMap  : register(t0);
Texture2D SpecularMap : register(t1);
Texture2D NormalMap   : register(t2);
Texture2D PositionMap : register(t3);

struct PixelIn {
    float4 position : SV_POSITION;
};

struct PixelOut {
    float4 accumulator : SV_TARGET0;
};

float4 CalculateLight(GBufferData gbuffer) {
    float3 color = gbuffer.diffuse * Color.color;
    return float4(color, 1.0f);
}

PixelIn VertexMain(uint id : SV_VERTEXID) {
    float2 uv = float2(id & 1, (id & 2) >> 1);
    
    PixelIn output;
    output.position = float4(uv * float2(2, -2) + float2(-1, 1), 0, 1);
    
    return output;
}

[earlydepthstencil]
PixelOut PixelMain(PixelIn input) {
    float2 uv = input.position.xy;
    GBufferData gbuffer = ReadGBufferData(DiffuseMap, SpecularMap, NormalMap, PositionMap, uv);
    
    PixelOut output;
    output.accumulator = CalculateLight(gbuffer);
    
    return output;
}