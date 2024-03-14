struct ModelData
{
    float4x4 world_view_projection;
    float4x4 normal_matrix;
};

struct LightData
{
    float3 view_position;
    float dummy0;
    float3 direction;
    float dummy1;
};

cbuffer constants : register(b0)
{
    ModelData model_data;
}

cbuffer light : register(b1)
{
    LightData light_data;
}

struct vs_in
{
    float3 position : POSITION;
    float3 normal   : NORMAL;
    float2 texcoord : TEXCOORD;
};

struct vs_out
{
    float4 position : SV_POSITION;
    float4 normal   : NORMAL;
    float2 texcoord : TEXCOORD;
};

Texture2D    mytexture : register(t0);
SamplerState mysampler : register(s0);

vs_out vs_main(vs_in input)
{
    vs_out output;
    output.position = mul(float4(input.position, 1.0), model_data.world_view_projection);
    output.normal = mul(float4(input.normal, 0.0), model_data.normal_matrix); 
    output.texcoord = input.texcoord;

    return output;
}

float4 ps_main(vs_out input) : SV_TARGET
{
    float4 color = mytexture.Sample(mysampler, input.texcoord);
    float3 light = float3(1.0, 1.0, 1.0);

    float ambient_factor = 0.1;
    float3 ambient = ambient_factor * light;

    float3 normal = normalize(input.normal.xyz);
    float diffuse_factor = max(0.0, dot(-light_data.direction, normal));
    float3 diffuse = diffuse_factor * light;

    float shininess = 32;
    float3 to_view_direction = normalize(light_data.view_position - input.position.xyz);
    float3 reflection_direction = normalize(reflect(light_data.direction, normal));
    float3 specular_factor = pow(max(0.0f, dot(to_view_direction, reflection_direction)), shininess);
    float3 specular = specular_factor * light;

    return float4((ambient + diffuse + specular), 1.0) * color;
}