Texture2D Accumulator : register(t0);
SamplerState Sampler  : register(s0);

struct PixelIn {
    float4 position : SV_POSITION;
    float2 uv       : TEXCOORD;
};

PixelIn VertexMain(uint id : SV_VERTEXID) {
    PixelIn output;
    output.uv = float2(id & 1, (id & 2) >> 1);
    output.position = float4(output.uv * float2(2, -2) + float2(-1, 1), 0.5, 1);
    
    return output;
}

float4 PixelMain(PixelIn input) : SV_TARGET {
    float3 result = Accumulator.Sample(Sampler, input.uv).rgb;
    return float4(result, 1.0);
}