struct GBufferData {
    float3 diffuse;
    float3 normal;
    float3 position;
};

GBufferData ReadGBufferData(Texture2D diffuse, Texture2D normal, Texture2D position, float2 uv) {
    GBufferData result;
    result.diffuse = diffuse.Load(float3(uv, 0)).rgb;
    result.normal = normal.Load(float3(uv, 0)).rgb;
    result.position = position.Load(float3(uv, 0)).rgb;
    
    return result;
}