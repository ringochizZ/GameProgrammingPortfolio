Texture2D<float4> Texture : register(t0);
sampler TextureSampler : register(s0);

Texture2D<float4> MaskTexture : register(t1);

cbuffer Parameters : register(b1)
{
    float Rate;
};

float4 main(float4 color : COLOR0, float2 texCoord : TEXCOORD0) : SV_Target0
{
    float4 col = Texture.Sample(TextureSampler, texCoord) * color;
    float4 mask = MaskTexture.Sample(TextureSampler, texCoord);

    // マスク用テクスチャを使用
    float rate = saturate(-mask.x + Rate * 2.0f);
    
    float2 uv = texCoord;
    
    uv.x *= 16.0f;
    uv.y *= 9.0f;
    
    int x = trunc(uv.x) % 2;
    int y = trunc(uv.y) % 2;
    
    int r = x ^ y;

    float alpha = Rate * 4 - frac(uv.x) - r * 2.0f;
    
    return float4(col.rgb, alpha);
}
