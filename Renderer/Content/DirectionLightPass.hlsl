#include "GBuffer.hlsl"

struct LightData {
    float3 view_position;
    float dummy0;
    float3 direction;
    float dummy1;
    float3 color;
    float dummy2;
};

cbuffer LightBuffer : register(b0) {
    LightData Light;
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
    float3 normal = normalize(gbuffer.normal);
    float diffuse_factor = saturate(dot(-Light.direction, normal));
    float3 diffuse = diffuse_factor * gbuffer.diffuse;
    
    float3 to_view_direction = normalize(Light.view_position - gbuffer.position);
    float3 reflection_direction = normalize(reflect(Light.direction, normal));
    float specular_factor = pow(max(0.0f, dot(to_view_direction, reflection_direction)), gbuffer.shininess);
    float3 specular = specular_factor * gbuffer.specular;

    float3 color = (diffuse + specular) * Light.color;
    return float4(color, 1.0);
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