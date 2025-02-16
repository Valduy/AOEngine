struct CameraData {
    float4x4 view_projection;
};

struct LineData {
    float4x4 world;
};

struct ColorData {
    float3 color;
    float dummy;
};

cbuffer CameraBuffer : register(b0) {
    CameraData Camera;
};

cbuffer LineBuffer : register(b1) {
    LineData Line;
};

cbuffer ColorBuffer : register(b2) {
    ColorData Color;
};

struct VertexIn {
    float3 position : POSITION0;
};

struct PixelIn {
    float4 position : SV_POSITION0;
    float4 color    : COLOR0;
};

PixelIn VertexMain(VertexIn input) {
    float4x4 world_view_projection = mul(Line.world, Camera.view_projection);
    float4 position = float4(input.position, 1.0);
    
    PixelIn output;
    output.position = mul(position, world_view_projection);
    output.color = float4(Color.color, 1.0);
    
    return output;
}

float4 PixelMain(PixelIn input) : SV_Target {
    return input.color;
}