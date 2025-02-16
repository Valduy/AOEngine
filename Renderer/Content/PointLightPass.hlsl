#include "GBuffer.hlsl"

struct CameraData {
    float4x4 view_projection;
    float3 view_position;
    float dummy;
};

struct TransformData {
    float4x4 world;
    float3 position;
    float dummy;
};

struct ColorData {
    float3 color;
    float dummy;
};

cbuffer CameraBuffer : register(b0) {
    CameraData Camera;
};

cbuffer TransformBuffer : register(b1) {
    TransformData Transform;
};

cbuffer ColorBuffer : register(b2) {
    ColorData Color;
};

Texture2D DiffuseMap  : register(t0);
Texture2D SpecularMap : register(t1);
Texture2D NormalMap   : register(t2);
Texture2D PositionMap : register(t3);

struct VertexIn {
    float3 position : POSITION0;
    float3 normal   : NORMAL0;
    float2 uv       : TEXCOORD0;
};

struct PixelIn {
    float4 position       : SV_POSITION0;
    float3 light_position : POSITION0;
    float3 view_position  : POSITION1;
};

struct PixelOut {
    float4 accumulator : SV_TARGET0;
};

float4 CalculateLight(GBufferData gbuffer, float3 light_position, float3 view_position) {
    float3 light_direction = normalize(gbuffer.position - light_position);
    
    float3 normal = normalize(gbuffer.normal);
    float diffuse_factor = saturate(dot(-light_direction, normal));
    float3 diffuse = diffuse_factor * gbuffer.diffuse;
    
    float3 to_view_direction = normalize(view_position - gbuffer.position);
    float3 reflection_direction = normalize(reflect(light_direction, normal));
    float specular_factor = pow(max(0.0f, dot(to_view_direction, reflection_direction)), gbuffer.shininess);
    float3 specular = specular_factor * gbuffer.specular;
    
    float3 color = (diffuse + specular) * Color.color;
    return float4(color, 1.0);
}

PixelIn VertexMain(VertexIn input) {
    float4x4 world_view_projection = mul(Transform.world, Camera.view_projection);
    float4 position = float4(input.position, 1.0);
    
    PixelIn output;
    output.position = mul(position, world_view_projection);
    output.light_position = Transform.position;
    output.view_position = Camera.view_position;
    
    return output;
};

[earlydepthstencil]
PixelOut PixelMain(PixelIn input) {
    float2 uv = input.position.xy;
    GBufferData gbuffer = ReadGBufferData(DiffuseMap, SpecularMap, NormalMap, PositionMap, uv);
    
    PixelOut output;
    output.accumulator = CalculateLight(gbuffer, input.light_position, input.view_position);
    
    return output;
}
