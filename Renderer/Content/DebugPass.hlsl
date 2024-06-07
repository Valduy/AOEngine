struct LineData {
    float4x4 world_view_projection;
    float3 color;
    float dummy;
};

cbuffer LineBuffer : register(b0) {
    LineData Line;
};

struct VertexIn {
    float3 position : POSITION0;
};

struct PixelIn {
    float4 position : SV_POSITION0;
    float4 color    : COLOR0;
};

PixelIn VertexMain(VertexIn input) {
    float4 position = float4(input.position, 1.0);
    
    PixelIn output;
    output.position = mul(position, Line.world_view_projection);
    output.color = float4(Line.color, 1.0);
    
    return output;
}

float4 PixelMain(PixelIn input) : SV_Target {
    return input.color;
}