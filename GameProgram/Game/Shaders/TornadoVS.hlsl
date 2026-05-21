// TornadoVS.hlsl
cbuffer TornadoBuffer : register(b1) // スロットを1に合わせる
{
    matrix WorldViewProj;
}

struct VS_INPUT
{
    float4 Pos : POSITION;
    float3 Normal : NORMAL; // sdkmeshが持っているデータを受け取れるように追加
    float2 Tex : TEXCOORD; // これを定義しないと不一致エラーになることがあります
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    // C++側で (world * view * proj) を計算済みなので mul 1回でOK
    output.Pos = mul(input.Pos, WorldViewProj);
    return output;
}