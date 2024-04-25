struct TransformData {
    float4x4 world_view_projection;
    float4x4 inverse_transpose_world;
};

struct LightData {
    float3 view_position;
    float dummy0;
    float3 direction;
    float dummy1;
};
 
cbuffer constants : register(b0) {
    TransformData Transform;
}

cbuffer light : register(b1) {
    LightData Light;
}

struct VertexIn {
    float3 position : POSITION;
    float3 normal   : NORMAL;
    float2 texcoord : TEXCOORD;
};

struct PixelIn {
    float4 position : SV_POSITION;
    float4 normal   : NORMAL;
    float2 texcoord : TEXCOORD;
};

Texture2D    Diffuse : register(t0);
SamplerState Sampler : register(s0);

PixelIn VertexMain(VertexIn input) {
    PixelIn output;
    output.position = mul(float4(input.position, 1.0), Transform.world_view_projection);
    output.normal = mul(float4(input.normal, 0.0), Transform.inverse_transpose_world);
    output.texcoord = input.texcoord;

    return output;
}

float4 PixelMain(PixelIn input) : SV_TARGET {
    float4 color = Diffuse.Sample(Sampler, input.texcoord);
    float3 light = float3(1.0, 1.0, 1.0);

    float ambient_factor = 0.1;
    float3 ambient = ambient_factor * light;

    float3 normal = normalize(input.normal.xyz);
    float diffuse_factor = max(0.0, dot(-Light.direction, normal));
    float3 diffuse = diffuse_factor * light;

    float shininess = 32;
    float3 to_view_direction = normalize(Light.view_position - input.position.xyz);
    float3 reflection_direction = normalize(reflect(Light.direction, normal));
    float specular_factor = pow(max(0.0f, dot(to_view_direction, reflection_direction)), shininess);
    float3 specular = specular_factor * light;

    return float4((ambient + diffuse + specular), 1.0) * color;
}