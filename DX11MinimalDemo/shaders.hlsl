cbuffer constants : register(b0)
{
    float4x4 transform;
    float4x4 projection;
    float3   lightvector;
}

struct vs_in
{
    float3 position : POSITION;
    float3 normal   : NORMAL;
    float2 texcoord : TEXCOORD;
    float3 color    : COLOR;
};

struct vs_out
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
    float4 color    : COLOR;
};

Texture2D    mytexture : register(t0);
SamplerState mysampler : register(s0);

vs_out vs_main(vs_in input)
{
    float light = clamp(dot(mul(input.normal, transform), normalize(-lightvector)), 0.0f, 1.0f) * 0.8f + 0.2f;
    
    vs_out output;

    output.position = mul(float4(input.position, 1.0f), mul(transform, projection));
    output.texcoord = input.texcoord;
    output.color    = float4(input.color * light, 1.0f);

    return output;
}

float4 ps_main(vs_out input) : SV_TARGET
{
    return mytexture.Sample(mysampler, input.texcoord) * input.color;
}