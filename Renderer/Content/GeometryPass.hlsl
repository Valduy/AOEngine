struct TransformData {
    matrix world;
    matrix world_view_projection;
    matrix inverse_transpose_world;
};

cbuffer TransformBuffer : register(b0) {
    TransformData Transform;
};

Texture2D DiffuseMap : register(t0);
Texture2D NormalMap  : register(t1);

SamplerState Sampler : register(s0);

struct VertexIn {
    float3 position : POSITIONT0;
    float3 normal   : NORMAL0;
    float2 uv       : TEXCOORD0;
};

struct PixelIn {
    float4 position       : SV_POSITION0;
    float4 normal         : NORMAL;
    float2 uv             : TEXCOORD0;
    float4 world_position : TEXCOORD1;
};

struct PixelOut {
    float4 diffuse  : SV_Target0;
    float4 normal   : SV_Target1;
    float4 position : SV_Target2;
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

PixelOut PixelMain(PixelIn input) {
    PixelOut output;
    output.diffuse = DiffuseMap.Sample(Sampler, input.uv);
    output.normal = input.normal;
    output.position = input.position;
    
    return output;
}