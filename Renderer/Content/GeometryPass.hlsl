struct TransformData {
    float4x4 world;
    float4x4 world_view_projection;
    float4x4 inverse_transpose_world;
};

struct MaterialData {
    float3 diffuse;
    float dummy0;
    float3 specular;
    float shininess;
};

cbuffer TransformBuffer : register(b0) {
    TransformData Transform;
};

cbuffer MaterialBuffer : register(b1) {
    MaterialData Material;
};

Texture2D DiffuseMap : register(t0);
SamplerState Sampler : register(s0);

struct VertexIn {
    float3 position : POSITION0;
    float3 normal   : NORMAL0;
    float2 uv       : TEXCOORD0;
};

struct PixelIn {
    float4 position       : SV_POSITION0;
    float4 normal         : NORMAL0;
    float2 uv             : TEXCOORD0;
    float4 world_position : TEXCOORD1;
};

struct PixelOut {
    float4 diffuse  : SV_TARGET0;
    float4 specular : SV_TARGET1;
    float4 normal   : SV_TARGET2;
    float4 position : SV_TARGET3;
};

PixelIn VertexMain(VertexIn input) {
    float4 position = float4(input.position, 1.0);
    float4 normal = float4(input.normal, 0.0);
    
    PixelIn output;
    output.position = mul(position, Transform.world_view_projection);
    output.normal = mul(normal, Transform.inverse_transpose_world);
    output.uv = input.uv;
    output.world_position = mul(position, Transform.world);
    
    return output;
}

[earlydepthstencil]
PixelOut PixelMain(PixelIn input) {
    float4 diffuse = float4(Material.diffuse, 1.0);
    
    PixelOut output;
    output.diffuse = DiffuseMap.Sample(Sampler, input.uv) * diffuse;
    output.specular.rgb = Material.specular;
    output.specular.a = Material.shininess;
    output.normal = input.normal;
    output.position = input.world_position;
    
    return output;
}