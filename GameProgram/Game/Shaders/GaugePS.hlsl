#include "Gauge.hlsli"

Texture2D tex : register(t0);
Texture2D tex2 : register(t1);
SamplerState samLinear : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
    // ‰æ‘œ‚Ì•\Ž¦
    float4 output = tex.Sample(samLinear, input.tex);
    float smoothValue = alphadate;

    output.a *= lerp(1.0f, 0.0f, smoothstep(smoothValue, smoothValue + 0.1f, input.tex.y));

    return output;
}
