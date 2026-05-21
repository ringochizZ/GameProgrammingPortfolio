#include "Background.hlsli"

Texture2D tex : register(t0);
Texture2D tex2 : register(t1);
SamplerState samLinear : register(s0);

float2 random2(float2 st)
{
    st = float2(dot(st, float2(127.1f, 311.7f)), dot(st, float2(269.5f, 183.3f)));
    return -1.0f + 2.0f * frac(sin(st) * 43758.5453f);
}

float perlinNoise(float2 uv)
{
    float2 p = floor(uv);
    float2 f = frac(uv);
    float2 u = f * f * (3.0 - 2.0 * f);
    float2 v00 = random2(p + float2(0, 0));
    float2 v10 = random2(p + float2(1, 0));
    float2 v01 = random2(p + float2(0, 1));
    float2 v11 = random2(p + float2(1, 1));
    return lerp(
			lerp(dot(v00, f - float2(0, 0)), dot(v10, f - float2(1, 0)), u.x),
			lerp(dot(v01, f - float2(0, 1)), dot(v11, f - float2(1, 1)), u.x),
			u.y) + 0.5f;
}

float fBm(float2 uv)
{
    float f = 0;
    float2 q = uv;
    
    f += 0.5000f * perlinNoise(q);
    q = q * 2.01f;
    f += 0.2500f * perlinNoise(q);
    q = q * 2.02f;
    f += 0.2500f * perlinNoise(q);
    
    return f;
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float2 calcUV = input.Tex;

    // 動くUV (直線移動 + sin揺れ)
    float2 motion = float2(time.x * 0.05f, sin(time.x * 0.2f) * 0.05f);
    calcUV += motion;

    // ノイズで流れる感じを追加
    float2 flow = float2(
        fBm(calcUV + float2(0.0f, time.x * 0.1f)),
        fBm(calcUV + float2(5.2f, time.x * 0.1f))
    );
    calcUV += (flow - 0.5f) * 0.1f;

    // ノイズでアルファ計算
    float n = fBm(calcUV * 10);
    float alpha = saturate(n);

    // 雲の色を合成
    float4 cloudColor = lerp(
        float4(0.8f, 0.8f, 1.0f, 0),
        float4(1.0f, 1.0f, 1.0f, 1),
        saturate((n - 0.4f) * 2.0f));

    cloudColor.a = alpha;

    return cloudColor;
}
