struct GBufferData {
    float3 diffuse;
    float3 specular;
    float shininess;
    float3 normal;
    float3 position;
};

GBufferData ReadGBufferData(Texture2D diffuse, Texture2D specular, Texture2D normal, Texture2D position, float2 uv) {
    float3 coords = float3(uv, 0);
    
    GBufferData result;
    result.diffuse = diffuse.Load(coords).rgb;
    result.specular = specular.Load(coords).rgb;
    result.shininess = specular.Load(coords).a;
    result.normal = normal.Load(coords).rgb;
    result.position = position.Load(coords).rgb;
    
    return result;
}