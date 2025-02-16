#include "GBuffer.hlsl"

struct CameraData {
    float3 view_position;
    float dummy;
};

struct TransformData {
    float3 direction;
    float dummy;
};

struct ColorData {
    float3 color;
    float dummy;
};

cbuffer CameraBuffer : register(b0) {
    CameraData Camera;
}

cbuffer TransformBuffer : register(b1) {
    TransformData Transform;
}

cbuffer ColorBuffer : register(b2) {
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
    float3 normal = normalize(gbuffer.normal);
    float diffuse_factor = saturate(dot(-Transform.direction, normal));
    float3 diffuse = diffuse_factor * gbuffer.diffuse;
    
    float3 to_view_direction = normalize(Camera.view_position - gbuffer.position);
    float3 reflection_direction = normalize(reflect(Transform.direction, normal));
    float specular_factor = pow(max(0.0f, dot(to_view_direction, reflection_direction)), gbuffer.shininess);
    float3 specular = specular_factor * gbuffer.specular;

    float3 color = (diffuse + specular) * Color.color;
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